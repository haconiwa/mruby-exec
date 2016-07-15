/*
** mrb_exec.c - Exec class
**
** Copyright (c) Uchio Kondo 2016
**
** See Copyright Notice in LICENSE
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mruby.h"
#include "mruby/data.h"
#include "mruby/string.h"
#include "mruby/array.h"
#include "mruby/error.h"
#include "mrb_exec.h"

#define DONE mrb_gc_arena_restore(mrb, 0);

#ifdef DEBUG
#define _DEBUGP printf
#else
#define _DEBUGP 1 ? (void) 0 : printf
#endif

typedef struct {
  char *dummy;
} mrb_exec_data;

static const struct mrb_data_type mrb_exec_data_type = {
  "mrb_exec_data", mrb_free,
};

static mrb_value mrb_exec_do_exec(mrb_state *mrb, mrb_value self)
{
  mrb_value *mrb_argv;
  mrb_int argc_;
  char **argv_;
  mrb_value strv;
  char *buf;
  int i, j, ai;

  mrb_get_args(mrb, "*", &mrb_argv, &argc_);
  if(argc_ < 1) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "exec must have at least 1 argument");
    return mrb_nil_value();
  }

  argv_ = (char **)mrb_malloc(mrb, sizeof(char *) * (argc_ + 1));

  ai = mrb_gc_arena_save(mrb);
  for(i = 0; i < argc_; i++) {
    strv = mrb_convert_type(mrb, mrb_argv[i], MRB_TT_STRING, "String", "to_str");
    buf = (char *)mrb_string_value_cstr(mrb, &strv);
    *argv_ = buf;
    argv_++;
  }
  *argv_ = NULL;

  // return to the top of array
  argv_ -= i;

  for(j = 0; j < argc_ + 1; j++) {
    _DEBUGP("[mruby-exec] argv_(%i): %s\n", j, argv_[j]);
  }

  mrb_gc_arena_restore(mrb, ai);
  execv(argv_[0], argv_);

  mrb_sys_fail(mrb, "execv failed");
  return mrb_nil_value();
}

static mrb_value mrb_exec_exec_override_procname(mrb_state *mrb, mrb_value self)
{
  mrb_value argv;
  int argc;
  char *procname, *execname;
  char **argv_;
  mrb_value strv;
  char *buf;
  int i, j, ai;

  mrb_get_args(mrb, "zA", &procname, &argv);
  argc = RARRAY_LEN( argv );
  if(argc < 1) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "exec must have at least 1 argument");
    return mrb_nil_value();
  }

  argv_ = (char **)mrb_malloc(mrb, sizeof(char *) * (argc + 1));

  ai = mrb_gc_arena_save(mrb);
  for(i = 0; i < argc; i++) {
    strv = mrb_convert_type(mrb, mrb_ary_ref( mrb, argv, i ), MRB_TT_STRING, "String", "to_str");
    buf = (char *)mrb_string_value_cstr(mrb, &strv);
    *argv_ = buf;
    argv_++;
  }
  *argv_ = NULL;

  // return to the top of array
  argv_ -= i;

  for(j = 0; j < argc + 1; j++) {
    _DEBUGP("[mruby-exec] argv_(%i): %s\n", j, argv_[j]);
  }

  mrb_gc_arena_restore(mrb, ai);

  execname = strdup(argv_[0]);
  argv_[0] = procname;
  execv(execname, argv_);

  mrb_sys_fail(mrb, "execv failed");
  return mrb_nil_value();
}

void mrb_mruby_exec_gem_init(mrb_state *mrb)
{
    struct RClass *ex;

    ex = mrb_define_module(mrb, "Exec");
    mrb_define_class_method(mrb, ex, "exec", mrb_exec_do_exec, MRB_ARGS_ANY());
    mrb_define_class_method(mrb, ex, "execv", mrb_exec_do_exec, MRB_ARGS_ANY());
    mrb_define_class_method(mrb, ex, "exec_override_procname", mrb_exec_exec_override_procname, MRB_ARGS_REQ(2));

    mrb_define_method(mrb, mrb->kernel_module, "exec", mrb_exec_do_exec, MRB_ARGS_ANY());
    DONE;
}

void mrb_mruby_exec_gem_final(mrb_state *mrb)
{
}
