#include "bscl_packet.h"
#include <stddef.h>
#include "bscl_crc.h"

#define PACKET_MAGIC 0x7e
#define PACKET_HEADER_SIZE sizeof(packet_head_t)
#define PACKET_LEN_POS offsetof(packet_head_t, len)
#define PACKET_MSGID_POS offsetof(packet_head_t, msgid)
#define PACKET_CRC_SIZE sizeof(packet_crc_t)

#define CRC_BUFFER_LEN 256

typedef union {
  struct {
    uint8_t magic;
    uint8_t msgid;
    uint16_t len;
    uint32_t seq;
  };
  uint32_t value[2];
} packet_head_t;

typedef uint32_t packet_crc_t;

enum PACKET_STAT_TYPE {
  PACKET_STAT_SYNC = 0,
  PACKET_STAT_HEAD,
  PACKET_STAT_DATA,
  PACKET_STAT_CRC,
  PACKET_STAT_PARSE,
};

// len:packet data len, not include head
static packet_crc_t packet_crc_buffer(bscl_packet_t *pak, int len) {
  buffer_t *mo = &pak->mo;
  uint8_t crc_buffer[CRC_BUFFER_LEN];
  packet_crc_t checksum = 0;
  int n_byte = len + PACKET_HEADER_SIZE; // include head
  int n_byte_offset = 0;

  while (n_byte_offset < n_byte) {
    int left = n_byte - n_byte_offset;
    int n = left < (CRC_BUFFER_LEN) ? left : (CRC_BUFFER_LEN);
    buffer_copy(mo, n_byte_offset, crc_buffer, n);
    checksum = bscl_crc32(crc_buffer, n, checksum);
    n_byte_offset += n;
  }

  return checksum;
}

void bscl_packet_init(bscl_packet_t *pak, void *buf, size_t size) {
  memset(pak, 0, sizeof(*pak));

  buffer_init(&pak->mo, buf, size);
  pak->stat = PACKET_STAT_SYNC;
  pak->left = 0;
}

// *size : in:buf size;out: consumed size
int bscl_packet_receive(bscl_packet_t *pak, uint8_t *buf, int *size) {
  buffer_t *mo = &pak->mo;
  int space = buffer_get_space(mo);
  int i = 0;
  int len = *size;

  if (pak->stat == PACKET_STAT_SYNC) {
    while (i < len) {
      if (buf[i] == PACKET_MAGIC) {
        if (space < len - i) {
          len = space + i;
        }
        pak->stat = PACKET_STAT_HEAD;
        pak->left = PACKET_HEADER_SIZE;
        break;
      } else {
        ++i;
      }
    }
    if (i == len) {
      // all invalid
      goto _more_data;
    }
  }

  if (pak->stat == PACKET_STAT_HEAD) {
    if (len - i < pak->left) {
      pak->left -= (len - i);
      buffer_write(mo, buf + i, len - i);
      goto _more_data;
    } else {
      pak->stat = PACKET_STAT_DATA;
      uint16_t len = 0;

      if (PACKET_HEADER_SIZE - pak->left <= PACKET_LEN_POS) {
        memcpy(&len, buf + i + PACKET_LEN_POS - (PACKET_HEADER_SIZE - pak->left), sizeof(len));
      } else if (PACKET_HEADER_SIZE - pak->left == PACKET_LEN_POS + 1) {
        buffer_copy(mo, PACKET_LEN_POS, &len, 1);
        memcpy((uint8_t *)&len + 1, buf + i, 1);
      } else {
        buffer_copy(mo, PACKET_LEN_POS, &len, sizeof(len));
      }

      pak->left += len;
    }
  }

  if (pak->stat == PACKET_STAT_DATA) {
    if (len - i < pak->left) {
      buffer_write(mo, buf + i, len - i);
      pak->left -= len - i;
      goto _more_data;
    } else {
      pak->stat = PACKET_STAT_CRC;
      pak->left += PACKET_CRC_SIZE;
    }
  }

  if (pak->stat == PACKET_STAT_CRC) {
    if (len - i < pak->left) {
      buffer_write(mo, buf + i, len - i);
      pak->left -= len - i;
      goto _more_data;
    } else {
      buffer_write(mo, buf + i, pak->left);
      // packet ready
      *size = i + pak->left;
      return BSCL_PACKET_READY;
    }
  }

_more_data:
  *size = len;
  return BSCL_PACKET_MOREDATA;
}

int bscl_packet_receive_byte(bscl_packet_t *pak, uint8_t c) {
  buffer_t *mo = &pak->mo;
  int parse = 0;

  if (buffer_putc(mo, c) < 0)
    goto _do_parse;

  if (buffer_get_space(mo) == 0)
    goto _do_parse;

  switch (pak->stat) {
  case PACKET_STAT_SYNC:
    if (c == PACKET_MAGIC) {
      pak->stat = PACKET_STAT_HEAD;
      pak->left = PACKET_HEADER_SIZE - 1;
    } else {
      buffer_drop_char(mo);
    }
    break;
  case PACKET_STAT_HEAD:
    pak->left--;
    if (pak->left == 0) {
      int len = buffer_copy_char(mo, PACKET_LEN_POS);

      if (len < 0)
        goto _do_parse;

      pak->left = len;
      pak->stat = PACKET_STAT_DATA;

      if (pak->left == 0) {
        pak->stat = PACKET_STAT_CRC;
        pak->left = PACKET_CRC_SIZE;
      }
    }
    break;
  case PACKET_STAT_DATA:
    pak->left--;
    if (pak->left == 0) {
      pak->stat = PACKET_STAT_CRC;
      pak->left = PACKET_CRC_SIZE;
    }
    break;
  case PACKET_STAT_CRC:
    pak->left--;
    if (pak->left == 0) {
      pak->stat = PACKET_STAT_PARSE;
      parse = 1;
    }
    break;
  default:
    goto _do_parse;
  }
  return parse;

_do_parse:
  if (pak->stat != PACKET_STAT_PARSE)
    parse = 1;
  pak->stat = PACKET_STAT_PARSE;
  return parse;
}

int bscl_packet_resync(bscl_packet_t *pak) {
  buffer_t *mo = &pak->mo;
  int count = 0;

  for (;;) {
    int len;
    packet_head_t head;
    count = 0;
    pak->stat = PACKET_STAT_SYNC;

    // search sync char
    for (;;) {
      int c = buffer_copy_first(mo);
      if (c < 0)
        goto _parse_abort;
      if (c == PACKET_MAGIC)
        break;
      buffer_drop_char(mo);
    }
    count = buffer_get_count(mo);
    pak->stat = PACKET_STAT_HEAD;
    if (count < PACKET_HEADER_SIZE) {
      pak->left = PACKET_HEADER_SIZE - count;
      goto _parse_abort;
    }

    buffer_copy(mo, 0, &head, sizeof(head));
    len = head.len;

    if ((len + PACKET_HEADER_SIZE + PACKET_CRC_SIZE) > buffer_get_size(mo)) {
      buffer_drop_char(mo);
      continue;
    }

    pak->stat = PACKET_STAT_DATA;
    if (count < (PACKET_HEADER_SIZE + len)) {
      pak->left = len - (count - PACKET_HEADER_SIZE);
      goto _parse_abort;
    }

    pak->stat = PACKET_STAT_CRC;
    if (count < (PACKET_HEADER_SIZE + len + PACKET_CRC_SIZE)) {
      pak->left = PACKET_CRC_SIZE - (count - PACKET_HEADER_SIZE - len);
      goto _parse_abort;
    }

    // need parse
    pak->stat = PACKET_STAT_PARSE;

    return BSCL_PACKET_READY;
  }

_parse_abort:
  return BSCL_PACKET_MOREDATA;
}

int bscl_packet_parse(bscl_packet_t *pak, bscl_packet_msginf_t *inf, void *buf, size_t size) {
  packet_head_t head;
  int len, crc_pos;
  buffer_t *mo = &pak->mo;
  packet_crc_t crc, checksum;

  if (buffer_copy(mo, 0, &head, sizeof(head)) != sizeof(head)) {
    return BSCL_PACKET_ERROR_NOHEAD;
  }

  len = head.len;

  if (len > buffer_get_size(mo)) {
    buffer_drop_char(mo);
    return BSCL_PACKET_ERROR;
  }

  crc_pos = PACKET_HEADER_SIZE + len;
  if (buffer_copy(mo, crc_pos, &crc, sizeof(crc)) != sizeof(crc)) {
    return BSCL_PACKET_ERROR_NOCRC;
  }

  // calc checksum
  checksum = packet_crc_buffer(pak, len);

  // if crc error, drop one char and return -1
  if (checksum != crc) {
    buffer_drop_char(mo);
    return BSCL_PACKET_BADCRC;
  }

  // skip header
  buffer_drop(mo, PACKET_HEADER_SIZE);

  // read out data
  if (buf) {
    if (size < len) {
      buffer_copy(mo, 0, buf, size);
    } else {
      buffer_copy(mo, 0, buf, len);
    }
  }
  buffer_drop(mo, len);

  // drop crc
  buffer_drop(mo, PACKET_CRC_SIZE);

  if (inf) {
    inf->len = head.len;
    inf->msgid = head.msgid;
    inf->seq = head.seq;
  }

  if (buffer_get_count(mo) > 0) {
    return BSCL_PACKET_RESYNC;
  } else {
    pak->stat = PACKET_STAT_SYNC;
    return BSCL_PACKET_MOREDATA;
  }
}

int bscl_packet_write(bscl_packet_t *pak, uint8_t msgid, uint32_t seq, const void *buf, int len) {
  packet_head_t head = {.magic = PACKET_MAGIC, .len = len, .msgid = msgid, .seq = seq};
  buffer_t *mo = &pak->mo;
  packet_crc_t checksum;
  int space = buffer_get_space(mo);

  int msg_size = len + sizeof(head) + sizeof(checksum);

  if (msg_size > space) {
    return -1;
  }

  checksum = 0;
  checksum = bscl_crc32(head.value, PACKET_HEADER_SIZE, checksum);
  checksum = bscl_crc32(buf, len, checksum);

  if (buffer_write(mo, &head, PACKET_HEADER_SIZE) != PACKET_HEADER_SIZE) {
    return -1;
  }

  if (buffer_write(mo, buf, len) != len) {
    return -1;
  }

  if (buffer_write(mo, &checksum, sizeof(checksum)) != sizeof(checksum)) {
    return -1;
  }

  return 0;
}
