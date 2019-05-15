#include "bscl_getopt.h"
#include "bscl_list_head.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  bscl_list_head_t head;
  const char *str;
} bscl_getopt_arg_t;

struct bscl_getopt_option_s {
  bscl_list_head_t head;
  const bscl_getopt_patern_t *patern;
  bscl_getopt_arg_t *current_arg;
  bscl_list_head_t args;
};

struct bscl_getopt_contex_s {
  const bscl_getopt_patern_t *paterns;
  unsigned int patern_num;
  bscl_list_head_t options;
  bscl_getopt_option_t *current_option;
};

static const bscl_getopt_patern_t *find_patern_with_name(bscl_getopt_contex_t *ctx, const char *name) {
  for (int i = 0; i < ctx->patern_num; ++i) {
    if (!strncmp(name, ctx->paterns[i].name, BSCL_GETOPT_PATERN_STR_MAX)) {
      return &ctx->paterns[i];
    }
  }
  return NULL;
}

static const bscl_getopt_patern_t *find_patern_with_sname(bscl_getopt_contex_t *ctx, const char sname) {
  for (int i = 0; i < ctx->patern_num; ++i) {
    if (sname == ctx->paterns[i].sname) {
      return &ctx->paterns[i];
    }
  }
  return NULL;
}

static bscl_getopt_option_t *find_option(bscl_getopt_contex_t *ctx, const bscl_getopt_patern_t *patern) {
  bscl_getopt_option_t *option;
  bscl_list_for_each_entry(option, &ctx->options, bscl_getopt_option_t, head) {
    if (option->patern == patern) {
      return option;
    }
  }
  return NULL;
}

static bscl_getopt_option_t *option_new(bscl_getopt_contex_t *ctx, const bscl_getopt_patern_t *patern) {
  bscl_getopt_option_t *option = (bscl_getopt_option_t *)malloc(sizeof(bscl_getopt_option_t));
  if (option) {
    option->patern = patern;
    option->current_arg = NULL;
    bscl_list_init(&option->args);
    bscl_list_insert_tail(&ctx->options, &option->head);
  }
  return option;
}

static void option_append_arg(bscl_getopt_option_t *option, bscl_getopt_arg_t *arg) {
  bscl_list_insert_tail(&option->args, &arg->head);
}

static int bscl_getopt_init(bscl_getopt_contex_t *ctx, const bscl_getopt_patern_t *paterns, unsigned int num) {
  ctx->patern_num = num;
  ctx->paterns = paterns;
  ctx->current_option = NULL;
  bscl_list_init(&ctx->options);
  return 0;
}

bscl_getopt_contex_t *bscl_getopt_new(const bscl_getopt_patern_t *paterns, unsigned int num) {
  bscl_getopt_contex_t *ctx = (bscl_getopt_contex_t *)malloc(sizeof(bscl_getopt_contex_t));
  if (ctx) {
    bscl_getopt_init(ctx, paterns, num);
  }
  return ctx;
}
void bscl_getopt_delete(bscl_getopt_contex_t *ctx);

static int bscl_getopt_parse_next(bscl_getopt_contex_t *ctx, const char *str) {
  int len = strlen(str);
  if (len < 1) {
    return -1;
  }
  const char *arg_str = NULL;
  const bscl_getopt_patern_t *patern = NULL;
  /*find patern*/
  if (str[0] == '-') {
    if (len < 2) {
      return -1;
    } else if (str[1] == '-') {
      patern = find_patern_with_name(ctx, str + 2);
    } else {
      patern = find_patern_with_sname(ctx, str[1]);
      if (len > 2) {
        arg_str = str + 2;
      }
    }
  } else {
    if (ctx->current_option) {
      patern = ctx->current_option->patern;
    } else {
      patern = find_patern_with_sname(ctx, '\0');
      if (!patern) {
        return -1;
      }
    }
    arg_str = str;
  }

  if (!patern) {
    // no such patern
    return -1;
  }
  /*find option*/
  bscl_getopt_option_t *option = find_option(ctx, patern);
  if (!option) {
    /*new option*/
    option = option_new(ctx, patern);
    if (!option) {
      return -1;
    }
  }

  ctx->current_option = option;
  if (arg_str) {
    bscl_getopt_arg_t *arg = (bscl_getopt_arg_t *)malloc(sizeof(bscl_getopt_arg_t));
    arg->str = arg_str;
    bscl_list_insert_tail(&option->args, &arg->head);
  }
  return 0;
}

int bscl_getopt_parse_end(bscl_getopt_contex_t *ctx) {
  ctx->current_option = NULL;
  return 0;
}

int bscl_getopt_parse(bscl_getopt_contex_t *ctx, int argc, char *argv[]) {
  for (int i = 1; i < argc; ++i) {
    if (bscl_getopt_parse_next(ctx, argv[i]) < 0) {
      return -1;
    }
  }
  bscl_getopt_parse_end(ctx);
  return 0;
}

bscl_getopt_option_t *bscl_getopt_next_option(bscl_getopt_contex_t *ctx) {
  if (!ctx->current_option) {
    ctx->current_option = bscl_list_first_entry(&ctx->options, bscl_getopt_option_t, head);
  } else {
    if (bscl_list_is_last(&ctx->options, &ctx->current_option->head)) {
      ctx->current_option = NULL;
    } else {
      ctx->current_option = bscl_list_next_entry(ctx->current_option, bscl_getopt_option_t, head);
    }
  }
  return ctx->current_option;
}

const bscl_getopt_patern_t *bscl_getopt_option_patern(bscl_getopt_option_t *option) { return option->patern; }

const char *bscl_getopt_option_next_arg(bscl_getopt_option_t *option) {
  if (!option->current_arg) {
    option->current_arg = bscl_list_first_entry(&option->args, bscl_getopt_arg_t, head);
  } else {
    if (bscl_list_is_last(&option->args, &option->current_arg->head)) {
      option->current_arg = NULL;
    } else {
      option->current_arg = bscl_list_next_entry(option->current_arg, bscl_getopt_arg_t, head);
    }
  }
  if (option->current_arg) {
    return option->current_arg->str;
  } else {
    return NULL;
  }
}
