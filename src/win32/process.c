#include "bscl.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#define COMMAND_LEN_MAX 1024

struct bscl_process_t_ {
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
};

bscl_process_t *bscl_process_exec(const char *command, int argc, char *argv[]) {
  char buf[COMMAND_LEN_MAX];
  int offset = 0;
  bscl_process_t *process = (bscl_process_t *)calloc(1, sizeof(bscl_process_t));
  process->si.cb = sizeof(STARTUPINFO);
  for (int i = 0; i < argc; ++i) {
    int size = sizeof(buf) - offset;
    offset = snprintf(buf + offset, size, "%s ", argv[i]);
    if (offset < 0 || offset >= size) {
      break;
    }
  }
  BOOL ret = CreateProcess(command, buf, NULL, NULL, FALSE, 0, NULL, NULL, &process->si, &process->pi);
  if (!ret) {
    free(process);
    return NULL;
  }
  return process;
}
