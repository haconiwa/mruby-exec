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
#include "mruby/error.h"
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
  mrb_value strv;
  char *buf;
  int i, j;

  mrb_get_args(mrb, "*", &mrb_argv, &argc);
  if(argc < 1) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "exec must have at least 1 argument");
    return mrb_nil_value();
  }

  argv = (char **)malloc(sizeof(char *) * (argc + 1));

  for(i = 0; i < argc; i++) {
    strv = mrb_convert_type(mrb, mrb_argv[i], MRB_TT_STRING, "String", "to_str");
    buf = mrb_string_value_cstr(mrb, &strv);
    *argv = buf;
    argv++;
  }
  *argv = NULL;

  // return to the top of array
  argv -= i;

  for(j = 0; j < argc + 1; j++) {
    _DEBUGP("[mruby-exec] argv(%i): %s\n", j, argv[j]);
  }

  execv(argv[0], argv);

  perror("execv");
  mrb_sys_fail(mrb, "execv failed");
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
