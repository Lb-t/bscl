#include "bscl_getopt.h"
#include <stdio.h>

const bscl_getopt_patern_t paterns[] = {
    {.name = "version", .sname = 'v'}, {.name = "input", .sname = 'i'}, {.name = "output", .sname = 'o'}};

int main(int argc, char *argv[]) {
  bscl_getopt_contex_t *ctx = bscl_getopt_new(paterns, sizeof(paterns) / sizeof(paterns[0]));
  if (bscl_getopt_parse(ctx, argc, argv) < 0) {
    fprintf(stderr, "fail.\n");
    return -1;
  }

  bscl_getopt_option_t *opt;
  const char *arg;

  while ((opt = bscl_getopt_next_option(ctx))) {
    const bscl_getopt_patern_t *patern = bscl_getopt_option_patern(opt);
    printf("%s: ", patern->name);
    while ((arg = bscl_getopt_option_next_arg(opt))) {
      printf("%s ", arg);
    }
    printf("\n");
  }
  return 0;
}
