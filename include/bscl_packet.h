#pragma once

#include "bscl_buffer.h"

#define BSCL_PACKET_MOREDATA 0
#define BSCL_PACKET_READY 1
#define BSCL_PACKET_RESYNC 2
#define BSCL_PACKET_ERROR -1
#define BSCL_PACKET_BADCRC -2
#define BSCL_PACKET_ERROR_NOHEAD -3
#define BSCL_PACKET_ERROR_NOCRC -4

typedef struct {
    buffer_t mo;
    uint16_t left;
    uint8_t stat;
} bscl_packet_t;

typedef struct {
    uint16_t len;
    uint8_t msgid;
    uint32_t seq;
} bscl_packet_msginf_t;

void bscl_packet_init(bscl_packet_t *pak, void *buf, size_t size);

// return 0: More data, >0: Packet ready (need parse), -1: error
int bscl_packet_receive_byte(bscl_packet_t *pak, uint8_t c);
int bscl_packet_receive(bscl_packet_t *pak, uint8_t *buf, int *size);
int bscl_packet_resync(bscl_packet_t *pak);

// return >0: Packet length, -1: More data
int bscl_packet_parse(bscl_packet_t *pak, bscl_packet_msginf_t *inf, void *buf, size_t size);

// write packet to buffer, return 0 for success, -1 for error
int bscl_packet_write(bscl_packet_t *pak, uint8_t msgid, uint32_t seq, const void *buf, int len);
