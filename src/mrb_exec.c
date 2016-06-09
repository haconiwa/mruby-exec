/*
** mrb_exec.c - Exec class
**
** Copyright (c) Uchio Kondo 2016
**
** See Copyright Notice in LICENSE
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mruby.h"
#include "mruby/string.h"
#include "mrb_exec.h"

#define DONE mrb_gc_arena_restore(mrb, 0);

#ifdef DEBUG
#define _DEBUGP printf
#else
#define _DEBUGP 1 ? (void) 0 : printf
#endif

static mrb_value mrb_exec_do_exec(mrb_state *mrb, mrb_value self)
{
  mrb_value *mrb_argv;
  mrb_int argc;
  char **argv;
  int i, j;
  mrb_get_args(mrb, "*", &mrb_argv, &argc);
  if(argc < 1) {
    return mrb_nil_value();
  }

  for(i = 0; i < argc; i++) {
    *argv = mrb_str_to_cstr(mrb, mrb_argv[i]);
    argv++;
  }
  *argv = NULL;

  // move to start of array
  argv = argv - i;

  for(j = 0; j < argc + 1; j++) {
    _DEBUGP("[mruby-exec] argv(%i): %s\n", j, argv[j]);
  }

  execv(argv[0], argv);

  perror("execv");
  return mrb_nil_value();
}

void mrb_mruby_exec_gem_init(mrb_state *mrb)
{
    struct RClass *e;
    e = mrb_define_class(mrb, "Exec", mrb->object_class);
    mrb_define_class_method(mrb, e, "exec", mrb_exec_do_exec, MRB_ARGS_ANY());
    mrb_define_class_method(mrb, e, "execv", mrb_exec_do_exec, MRB_ARGS_ANY());
    DONE;
}

void mrb_mruby_exec_gem_final(mrb_state *mrb)
{
}
