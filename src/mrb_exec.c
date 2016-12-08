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

#include <mruby.h>
#include <mruby/data.h>
#include <mruby/string.h>
#include <mruby/array.h>
#include <mruby/error.h>
#include "mrb_exec.h"

#define DONE mrb_gc_arena_restore(mrb, 0);

#ifdef DEBUG
#define _DEBUGP printf
#else
#define _DEBUGP 1 ? (void) 0 : printf
#endif

static int mrb_value_to_strv(mrb_state *mrb, mrb_value *array, mrb_int len, char **result)
{
  mrb_value strv;
  char *buf;
  int i;

  if(len < 1) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "must have at least 1 argument");
    return -1;
  }

  int ai = mrb_gc_arena_save(mrb);
  for(i = 0; i < len; i++) {
    strv = mrb_convert_type(mrb, array[i], MRB_TT_STRING, "String", "to_str");
    buf = (char *)mrb_string_value_cstr(mrb, &strv);
    *result = buf;
    result++;
  }
  *result = NULL;

  // return to the top of array
  result -= i;

  for(int j = 0; j < len + 1; j++) {
    _DEBUGP("[mruby-exec] result(%i): %s\n", j, result[j]);
  }

  mrb_gc_arena_restore(mrb, ai);
  return 0;
}

static mrb_value mrb_exec_do_exec(mrb_state *mrb, mrb_value self)
{
  mrb_value *mrb_argv;
  mrb_int len;
  char **result;

  mrb_get_args(mrb, "*", &mrb_argv, &len);
  result = (char **)mrb_malloc(mrb, sizeof(char *) * (len + 1));

  if(mrb_value_to_strv(mrb, mrb_argv, len, result) < 0){
    mrb_sys_fail(mrb, "[BUG] mrb_value_to_strv failed");
  }

  execv(result[0], result);

  mrb_sys_fail(mrb, "execv failed");
  return mrb_nil_value();
}

static mrb_value mrb_exec_exec_override_procname(mrb_state *mrb, mrb_value self)
{
  mrb_value *argv;
  mrb_int len;
  char *procname, *execname;
  char **result;

  mrb_get_args(mrb, "z*", &procname, &argv, &len);
  result = (char **)mrb_malloc(mrb, sizeof(char *) * (len + 1));

  if(mrb_value_to_strv(mrb, argv, len, result) < 0){
    mrb_sys_fail(mrb, "[BUG] mrb_value_to_strv failed");
  }

  execname = strdup(result[0]);
  result[0] = procname;
  execv(execname, result);

  mrb_sys_fail(mrb, "execv failed");
  return mrb_nil_value();
}

void mrb_mruby_exec_gem_init(mrb_state *mrb)
{
  struct RClass *ex;

  ex = mrb_define_module(mrb, "Exec");
  mrb_define_class_method(mrb, ex, "exec", mrb_exec_do_exec, MRB_ARGS_ANY());
  mrb_define_class_method(mrb, ex, "execv", mrb_exec_do_exec, MRB_ARGS_ANY());
  mrb_define_class_method(mrb, ex, "exec_override_procname", mrb_exec_exec_override_procname, MRB_ARGS_ANY());

  mrb_define_method(mrb, mrb->kernel_module, "exec", mrb_exec_do_exec, MRB_ARGS_ANY());
  DONE;
}

void mrb_mruby_exec_gem_final(mrb_state *mrb)
{
}
