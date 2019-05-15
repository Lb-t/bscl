#pragma once


#define BSCL_GETOPT_PATERN_STR_MAX 64

typedef struct {
  char name[BSCL_GETOPT_PATERN_STR_MAX];
  char sname; // short name
} bscl_getopt_patern_t;


typedef struct bscl_getopt_contex_s bscl_getopt_contex_t;
typedef struct bscl_getopt_option_s bscl_getopt_option_t;

bscl_getopt_contex_t *bscl_getopt_new(const bscl_getopt_patern_t *paterns, unsigned int num);
void bscl_getopt_delete(bscl_getopt_contex_t *ctx);
int bscl_getopt_parse(bscl_getopt_contex_t *ctx,int argc, char *argv[]);

bscl_getopt_option_t *bscl_getopt_next_option(bscl_getopt_contex_t *ctx);
const bscl_getopt_patern_t *bscl_getopt_option_patern(bscl_getopt_option_t *option);
const char *bscl_getopt_option_next_arg(bscl_getopt_option_t *option);
