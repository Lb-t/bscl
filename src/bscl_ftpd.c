#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include "bscl_ftpd.h"

#ifdef _NODEBUG
#define LOG(level, format, ...)
#else
#define LOG(level, format, ...) fprintf(stdout, "%s %d:" #level " " format, __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define SYS_ERROR(format, ...) LOG(error, format, ##__VA_ARGS__)
#define SYS_MSG(format, ...) LOG(msg, format, ##__VA_ARGS__)
#define SYS_WARNING(format, ...) LOG(warning, format, ##__VA_ARGS__)

#define FTPD_SYSTYPE "windows" //"UNIX Type: L8"

//#define PORT 20
#if 1
#ifndef SOCKET
#define SOCKET int
#endif
#endif

#define SESSION_MAX 10

#define FTPD_SERVER_MESSAGE "ftpd ready."
#define FTPD_CMD_BUF_SIZE 256       /* Size for temporary buffers */
#define FTPD_DATA_BUF_SIZE 4 * 1024 /* Size for file transfer buffers */

#define FTPD_MODE_BINARY 0
#define FTPD_MODE_ASCII 1

typedef struct {
  bscl_os_thread_t tid;
  struct sockaddr_in ctrl_addr; /* Control connection self address */
  struct sockaddr_in data_addr; /* Data address set by PORT command */
  SOCKET ctrl_socket;           /* Socket for ctrl connection */
  SOCKET data_socket;           /* Socket for data connection */
  int xfer_mode;                /* Transfer mode (ASCII/binary) */
  bool pasive;
  uint16_t self_control_port;
  uint8_t data_buf[FTPD_DATA_BUF_SIZE];
} ftpd_session_t;

static const ftpd_callback_t *cb;

static char const *serr(void) {
  int err = errno;
  errno = 0;
  return strerror(err);
}

static int set_socket_timeout(int s, int seconds) {
  int res = 0;
  struct timeval tv;
  int len = sizeof(tv);

  if (seconds < 0)
    seconds = 0;
  tv.tv_usec = 0;
  tv.tv_sec = seconds;
  if (0 != setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv, len))
    SYS_ERROR("ftpd: Can't set send timeout on socket: %s.", serr());
  else if (0 != setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, len))
    SYS_ERROR("ftpd: Can't set receive timeout on socket: %s.", serr());
  else
    res = 1;
  return res;
}

static void close_stream(ftpd_session_t *session) {
  if (session->ctrl_socket < 0) {
    return;
  }
  if (closesocket(session->ctrl_socket))
    SYS_ERROR("ftpd: Could not close control socket: %s", serr());

  session->ctrl_socket = -1;
}

static int can_write(void) {
  return -1;
}

static void send_ctrl_reply(ftpd_session_t *session, int code, char *text) {
  text = text != NULL ? text : "";
  char buf[FTPD_CMD_BUF_SIZE];
  int res;
  res = snprintf(buf, sizeof(buf), "%d %s\r\n", code, text);
  send(session->ctrl_socket, buf, res, 0);
  LOG(debug, "resp:%s\n", buf);
}

static int recv_line(SOCKET s, char *buf, int len) {
  int i = 0;
  int res;
  while (i < len) {
    if ((res = recv(s, buf + i, 1, 0)) < 0) {
      return res;
    }
    ++i;
    if (buf[i - 1] == '\n') {
      break;
    }
  }
  buf[i] = 0;
  return i;
}

static void skip_options(char **p) {
  char *buf = *p;
  char *last = NULL;
  while (1) {
    while (isspace((unsigned char)*buf))
      ++buf;
    if (*buf == '-') {
      if (*++buf == '-') { /* `--' should terminate options */
        if (isspace((unsigned char)*++buf)) {
          last = buf;
          do
            ++buf;
          while (isspace((unsigned char)*buf));
          break;
        }
      }
      while (*buf && !isspace((unsigned char)*buf))
        ++buf;
      last = buf;
    } else
      break;
  }
  if (last)
    *last = '\0';
  *p = buf;
}

static void split_command(char *buf, char **cmd, char **opts, char **args) {
  char *eoc;
  char *p = buf;
  while (isspace((unsigned char)*p))
    ++p;
  *cmd = p;
  while (*p && !isspace((unsigned char)*p)) {
    *p = toupper((unsigned char)*p);
    ++p;
  }
  eoc = p;
  if (*p)
    *p++ = '\0';
  while (isspace((unsigned char)*p))
    ++p;
  *opts = p;
  skip_options(&p);
  *args = p;
  if (*opts == p)
    *opts = eoc;
  while (*p && *p != '\r' && *p != '\n')
    ++p;
  if (*p)
    *p++ = '\0';
}

static int data_socket(ftpd_session_t *session) {
  SOCKET s = session->data_socket;
  if (s < 0) {
    char on = 1; // int on = 1;
    s = socket(PF_INET, SOCK_STREAM, 0);
    if (0 > s)
      send_ctrl_reply(session, 425, "Can't create data socket.");
    else if (0 > setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) {
      closesocket(s);
      s = -1;
      LOG(debug, "setsockopt fail\n");
    } else {
      struct sockaddr_in data_source;
      int tries;

      /* anchor socket to avoid multi-homing problems */
      data_source = session->ctrl_addr;
      data_source.sin_port = htons(session->self_control_port - 1); /* ftp-data port */
      for (tries = 1; tries < 10; ++tries) {
        errno = 0;
        if (bind(s, (struct sockaddr *)&data_source, sizeof(data_source)) >= 0)
          break;
        if (errno != EADDRINUSE)
          tries = 10;
      }
      if (tries >= 10) {
        send_ctrl_reply(session, 425, "Can't bind data socket.");
        closesocket(s);
        s = -1;
      } else {
        struct sockaddr_in *data_dest = &session->data_addr;
        if (0 > connect(s, (struct sockaddr *)data_dest, sizeof(*data_dest))) {
          send_ctrl_reply(session, 425, "Can't connect data socket.");
          closesocket(s);
          s = -1;
        } else {
          session->data_socket = s;
        }
      }
    }
  }
  return (int)s;
}

static void close_data_socket(ftpd_session_t *session) {
  int s = session->data_socket;
  if (s < 0) {
    LOG(debug, "data socket need not close\n");
    return;
  }

  if (closesocket(s))
    fprintf(stderr, "ftpd: Error closing data socket.");

  LOG(debug, "close data socket\n");
  session->data_socket = -1;
}

static void send_mode_reply(ftpd_session_t *session) {
  if (session->xfer_mode == FTPD_MODE_BINARY)
    send_ctrl_reply(session, 150, "Opening BINARY mode data connection.");
  else
    send_ctrl_reply(session, 150, "Opening ASCII mode data connection.");
}

static void command_retrieve(ftpd_session_t *session, char const *filename) {
  int s = -1;
  int fd = -1;
  int res = 0;
  if (session->xfer_mode == FTPD_MODE_BINARY) {
    fd = cb->open(filename, OS_FS_MODE_READ);
  } else {
    return;
  }

  if (fd < 0) {
    send_ctrl_reply(session, 550, "Error opening file.");
    return;
  }

  send_mode_reply(session);

  s = data_socket(session);

  if (0 <= s) {
    int n = -1;

    while ((n = cb->read(fd, session->data_buf, FTPD_DATA_BUF_SIZE)) > 0) {
      if (send(s, (char *)session->data_buf, n, 0) != n) {
        SYS_ERROR("send error\n");
        break;
      }
      LOG(debug, "send count %d\n", n);
    }

    if (0 == n) {
      res = 1;
    }
  }

  if (fd >= 0)
    cb->close(fd);

  if (0 == res)
    send_ctrl_reply(session, 451, "File read error.");
  else
    send_ctrl_reply(session, 226, "Transfer complete.");

  close_data_socket(session);
  return;
}
#if 0
static int discard(int fd, void const *buf, int count)
{
    (void)fd;
    (void)buf;
    return count;
}
#endif

static void command_store(ftpd_session_t *session, char const *filename) {
  send_ctrl_reply(session, 550, "Access denied.");
  return;
}

static int send_dirline(int s, int wide, time_t curTime, char const *path, char const *add, char const *fname, char *buf) {
  if (wide) {
    struct os_fs_stat_buf stat_buf;
    int plen = strlen(path);
    int alen = strlen(add);
    if (plen == 0) {
      buf[plen++] = '/';
      buf[plen] = '\0';
    } else {
      strcpy(buf, path);
      if (alen > 0 && buf[plen - 1] != '/') {
        buf[plen++] = '/';
        if (plen >= FTPD_CMD_BUF_SIZE)
          return 0;
        buf[plen] = '\0';
      }
    }
    if (plen + alen >= FTPD_CMD_BUF_SIZE)
      return 0;
    strcpy(buf + plen, add);
    if (cb->stat(buf, &stat_buf) == 0) {
      int len;
      struct tm bt;
      time_t tf = stat_buf.time;
      enum { SIZE = 80 };
      time_t SIX_MONTHS = (365L * 24L * 60L * 60L) / 2L;
      char timeBuf[SIZE];
      gmtime_s(&bt, &tf);
      if (curTime > tf + SIX_MONTHS || tf > curTime + SIX_MONTHS)
        strftime(timeBuf, SIZE, "%b %d  %Y", &bt);
      else
        strftime(timeBuf, SIZE, "%b %d %H:%M", &bt);

      len = snprintf(buf, FTPD_CMD_BUF_SIZE, "%crwxrwxrwx  1 0 0 %11u %s %s\r\n", (stat_buf.type == OS_FS_TYPE_DIR) ? 'd' : '-',
                     stat_buf.size, timeBuf, fname);
      if (send(s, buf, len, 0) != len)
        return 0;
    }
  } else {
    int len = snprintf(buf, FTPD_CMD_BUF_SIZE, "%s\r\n", fname);
    if (send(s, buf, len, 0) != len)
      return 0;
  }
  return 1;
}

static void command_list(ftpd_session_t *session, char const *fname, int wide) {
  int s;
  char buf[FTPD_CMD_BUF_SIZE];
  time_t curTime;
  int sc = 1;
  struct os_fs_stat_buf stat_buf;
  struct os_fs_dirent dirent;

  send_ctrl_reply(session, 150, "Opening ASCII mode data connection for LIST.");

  s = data_socket(session);
  if (0 > s) {
    fprintf(stderr, "ftpd: Error connecting to data socket.");
    return;
  }

  if (fname[0] == '\0')
    fname = ".";

  if (cb->stat(fname, &stat_buf) < 0) {
    snprintf(buf, FTPD_CMD_BUF_SIZE, "can not stat %s.\r\n", fname);
    send(s, buf, strlen(buf), 0);
  } else {
    time(&curTime);

    if (stat_buf.type == OS_FS_TYPE_DIR) {
      int fd = cb->opendir(fname);
      if (fd < 0) {
        return;
      }

      while (cb->readdir(fd, &dirent) >= 0) {
        sc = sc && send_dirline(s, wide, curTime, fname, dirent.name, dirent.name, buf);
      }
      cb->closedir(fd);
    } else {
      sc = sc && send_dirline(s, wide, curTime, fname, "", fname, buf);
    }
  }

  close_data_socket(session);

  if (sc)
    send_ctrl_reply(session, 226, "Transfer complete.");
  else
    send_ctrl_reply(session, 426, "Connection aborted.");
}

static void command_cwd(ftpd_session_t *session, char *dir) {
  if (cb->chdir(dir) == 0)
    send_ctrl_reply(session, 250, "CWD command successful.");
  else
    send_ctrl_reply(session, 550, "CWD command failed.");
}

static void command_pwd(ftpd_session_t *session) {
  char buf[FTPD_CMD_BUF_SIZE] = "\"";
  char const *cwd;
  cwd = cb->getcwd(buf + 1, FTPD_CMD_BUF_SIZE - 4);
  if (cwd) {
    int len = strlen(cwd);
    static char const txt[] = "\" is the current directory.";
    int size = sizeof(txt);
    if (len + size + 1 >= FTPD_CMD_BUF_SIZE)
      size = FTPD_CMD_BUF_SIZE - len - 2;

    memcpy(buf + len + 1, txt, size);
    // memcpy(buf, txt, size);
    buf[len + size + 1] = '\0';
    printf("len %d,size %d,%s\n", len, size, buf);
    send_ctrl_reply(session, 250, buf);
  } else {
    SYS_ERROR("getcwd fail");
    snprintf(buf, FTPD_CMD_BUF_SIZE, "Error: %s.", serr());
    send_ctrl_reply(session, 452, buf);
  }
}

static void command_mdtm(ftpd_session_t *session, char const *fname) {
  send_ctrl_reply(session, 550, "Access denied.");
}

static void command_size(ftpd_session_t *session, char const *fname) {
  struct os_fs_stat_buf stbuf;
  char buf[FTPD_CMD_BUF_SIZE];

  if (session->xfer_mode != FTPD_MODE_BINARY || 0 > cb->stat(fname, &stbuf) || stbuf.size < 0) {
    send_ctrl_reply(session, 550, "Could not get file size.");
  } else {
    snprintf(buf, FTPD_CMD_BUF_SIZE, "%" PRIuMAX, (uintmax_t)stbuf.size);
    send_ctrl_reply(session, 213, buf);
  }
}

static void command_pasv(ftpd_session_t *session) {
  int s = -1;
  int err = 1;

  close_data_socket(session);

  s = socket(PF_INET, SOCK_STREAM, 0);
  if (s < 0)
    fprintf(stderr, "ftpd: Error creating PASV socket: %s", serr());
  else {
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);

    addr = session->ctrl_addr;
    addr.sin_port = htons(0);

    if (0 > bind(s, (struct sockaddr *)&addr, addrLen))
      fprintf(stderr, "ftpd: Error binding PASV socket: %s", serr());
    else if (0 > listen(s, 1))
      fprintf(stderr, "ftpd: Error listening on PASV socket: %s", serr());
    else if (set_socket_timeout(s, 100)) {
      char buf[FTPD_CMD_BUF_SIZE];
      unsigned char const *ip, *p;

      getsockname(s, (struct sockaddr *)&addr, &addrLen);
      ip = (unsigned char const *)&(addr.sin_addr);
      p = (unsigned char const *)&(addr.sin_port);
      snprintf(buf, FTPD_CMD_BUF_SIZE, "Entering passive mode (%u,%u,%u,%u,%u,%u).", ip[0], ip[1], ip[2], ip[3], p[0], p[1]);
      send_ctrl_reply(session, 227, buf);

      session->data_socket = accept(s, (struct sockaddr *)&addr, &addrLen);
      if (0 > session->data_socket) {
        fprintf(stderr, "ftpd: Error accepting PASV connection: %s", serr());
        session->pasive = true;
      } else {
        closesocket(s);
        s = -1;
        err = 0;
      }
    }
  }
  if (err) {
    send_ctrl_reply(session, 425, "Can't open passive connection.");
    closesocket(s);
  }
}

static void command_port(ftpd_session_t *session, char const *args) {
  enum { NUM_FIELDS = 6 };
  unsigned int a[NUM_FIELDS];
  int n;

  close_data_socket(session);

  n = sscanf(args, "%u,%u,%u,%u,%u,%u", a + 0, a + 1, a + 2, a + 3, a + 4, a + 5);
  if (NUM_FIELDS == n) {
    int i;
    union {
      uint8_t b[NUM_FIELDS];
      struct {
        uint32_t ip;
        uint16_t port;
      } u;
    } ip_info;

    for (i = 0; i < NUM_FIELDS; ++i) {
      if (a[i] > 255)
        break;
      ip_info.b[i] = (uint8_t)a[i];
    }

    if (i == NUM_FIELDS) {
      /* Note: while it contradicts with RFC959, we don't allow PORT command
       * to specify IP address different than those of the originating client
       * for the sake of safety. */
      if (ip_info.u.ip == session->data_addr.sin_addr.s_addr) {
        session->data_addr.sin_addr.s_addr = ip_info.u.ip;
        session->data_addr.sin_port = ip_info.u.port;
        session->data_addr.sin_family = AF_INET;
        memset(session->data_addr.sin_zero, 0, sizeof(session->data_addr.sin_zero));

        send_ctrl_reply(session, 200, "PORT command successful.");
        session->pasive = false;
        SYS_MSG("port %d\n", ntohs(ip_info.u.port));
        return; /* success */
      } else {
        send_ctrl_reply(session, 425, "Address doesn't match peer's IP.");
        SYS_ERROR("Address doesn't match peer's IP.");
        return;
      }
    }
  }
  send_ctrl_reply(session, 501, "Syntax error.");
}

static void exec_command(ftpd_session_t *session, char *cmd, char *args) {
  char fname[FTPD_CMD_BUF_SIZE];
  int wrong_command = 0;

  fname[0] = '\0';
  if (!strcmp("PORT", cmd)) {
    command_port(session, args);
  } else if (!strcmp("PASV", cmd)) {
    command_pasv(session);
  } else if (!strcmp("RETR", cmd)) {
    strncpy(fname, args, 254);
    command_retrieve(session, fname);
  } else if (!strcmp("STOR", cmd)) {
    strncpy(fname, args, 254);
    command_store(session, fname);
  } else if (!strcmp("LIST", cmd)) {
    strncpy(fname, args, 254);
    command_list(session, fname, 1);
  } else if (!strcmp("NLST", cmd)) {
    strncpy(fname, args, 254);
    command_list(session, fname, 0);
  } else if (!strcmp("MDTM", cmd)) {
    strncpy(fname, args, 254);
    command_mdtm(session, fname);
  } else if (!strcmp("SIZE", cmd)) {
    strncpy(fname, args, 254);
    command_size(session, fname);
  } else if (!strcmp("SYST", cmd)) {
    send_ctrl_reply(session, 215, FTPD_SYSTYPE);
  } else if (!strcmp("TYPE", cmd)) {
    if (args[0] == 'I') {
      session->xfer_mode = FTPD_MODE_BINARY;
      send_ctrl_reply(session, 200, "Type set to I.");
    } else if (args[0] == 'A') {
      session->xfer_mode = FTPD_MODE_ASCII;
      send_ctrl_reply(session, 200, "Type set to A.");
    } else {
      session->xfer_mode = FTPD_MODE_BINARY;
      send_ctrl_reply(session, 504, "Type not implemented.  Set to I.");
    }
  } else if (!strcmp("USER", cmd)) {
    send_ctrl_reply(session, 230, "User logged in.");
  } else if (!strcmp("PASS", cmd)) {
    send_ctrl_reply(session, 230, "User logged in.");
  } else if (!strcmp("DELE", cmd)) {
    if (strncpy(fname, args, 254) && cb->remove(fname) == 0) {
      send_ctrl_reply(session, 257, "DELE successful.");
    } else {
      send_ctrl_reply(session, 550, "DELE failed.");
    }
  } else if (!strcmp("SITE", cmd)) {
    char *opts;
    split_command(args, &cmd, &opts, &args);
    if (!strcmp("CHMOD", cmd)) {
      send_ctrl_reply(session, 550, "Access denied.");

    } else
      wrong_command = 1;
  } else if (!strcmp("RMD", cmd)) {
    if (!can_write()) {
      send_ctrl_reply(session, 550, "Access denied.");
    } else if (strncpy(fname, args, 254) && cb->remove(fname) == 0) {
      send_ctrl_reply(session, 257, "RMD successful.");
    } else {
      send_ctrl_reply(session, 550, "RMD failed.");
    }
  } else if (!strcmp("MKD", cmd)) {
    if (!can_write()) {
      send_ctrl_reply(session, 550, "Access denied.");
    } else if (strncpy(fname, args, 254) && 0) {
      send_ctrl_reply(session, 257, "MKD successful.");
    } else {
      send_ctrl_reply(session, 550, "MKD failed.");
    }
  } else if (!strcmp("CWD", cmd)) {
    strncpy(fname, args, 254);
    command_cwd(session, fname);
  } else if (!strcmp("CDUP", cmd)) {
    command_cwd(session, "..");
  } else if (!strcmp("PWD", cmd)) {
    command_pwd(session);
  } else
    wrong_command = 1;

  if (wrong_command)
    send_ctrl_reply(session, 500, "Command not understood.");
}

void *session_task(void *arg) {
  ftpd_session_t *const session = (ftpd_session_t *)arg;
  send_ctrl_reply(session, 220, FTPD_SERVER_MESSAGE);
  while (1) {
    char buf[FTPD_CMD_BUF_SIZE];
    char *cmd, *opts, *args;

    if (recv_line(session->ctrl_socket, buf, sizeof(buf)) <= 0) {
      SYS_ERROR("ftpd: Connection aborted.");
      break;
    }
    LOG(debug, "recv:%s\n", buf);
    split_command(buf, &cmd, &opts, &args);

    if (!strcmp("QUIT", cmd)) {
      send_ctrl_reply(session, 221, "Goodbye.");
      break;
    } else {
      exec_command(session, cmd, args);
    }
  }
  /* Close connection and put ourselves back into the task pool. */
  close_data_socket(session);
  close_stream(session);

  return NULL;
}

void ftpd_run(uint16_t port, const ftpd_callback_t *callback) {
  SOCKET s;
  socklen_t addrLen;
  struct sockaddr_in addr;

  cb = callback;

  WSADATA wsaData;
  int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (res != 0) {
    SYS_ERROR("WSAStartup failed:\n");
  }

  s = socket(PF_INET, SOCK_STREAM, 0);
  if (s < 0)
    SYS_ERROR("ftpd: Error creating socket: %s", serr());

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

  if (0 > bind(s, (struct sockaddr *)&addr, sizeof(addr)))
    SYS_ERROR("ftpd: Error binding control socket: %d", WSAGetLastError());
  else if (0 > listen(s, 1))
    SYS_ERROR("ftpd: Error listening on control socket: %s", serr());
  else
    while (1) {
      int ss;
      addrLen = sizeof(addr);
      ss = accept(s, (struct sockaddr *)&addr, &addrLen);
      if (0 > ss) {
        fprintf(stderr, "ftpd: Error accepting control connection: %s", serr());
      } else {
        ftpd_session_t *session = malloc(sizeof(ftpd_session_t));
        if (NULL == session) {
          closesocket(ss);
        } else {
          session->ctrl_socket = ss;
          /* Initialize corresponding SessionInfo structure */
          if (0 > getsockname(ss, (struct sockaddr *)&addr, &addrLen)) {
            fprintf(stderr, "ftpd: getsockname(): %s", serr());
          } else {
            session->self_control_port = port;
            session->pasive = false;
            session->ctrl_addr = addr;
            session->data_socket = -1;
            session->data_addr = addr;
            session->data_addr.sin_port = htons(ntohs(session->ctrl_addr.sin_port) - 1);

            // start session
            bscl_os_thread_create(&session->tid, NULL, session_task, session);
          }
        }
      }
    }
}
