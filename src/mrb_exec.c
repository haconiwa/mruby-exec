/*
** mrb_exec.c - Exec class
**
** Copyright (c) Uchio Kondo 2016
**
** See Copyright Notice in LICENSE
*/

#define _GNU_SOURCE

// clang-format off
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>

#include <mruby.h>
#include <mruby/data.h>
#include <mruby/string.h>
#include <mruby/array.h>
#include <mruby/hash.h>
#include <mruby/error.h>
#include "mrb_exec.h"
// clang-format on

#define DONE mrb_gc_arena_restore(mrb, 0);

#ifdef DEBUG
#define _DEBUGP printf
#else
#define _DEBUGP 1 ? (void)0 : printf
#endif

#define SYS_FAIL_MESSAGE_LENGTH 2048

static char** mrb_hash_to_strv(mrb_state *mrb, mrb_value *mrb_hash)
{
  int ai = mrb_gc_arena_save(mrb);
  mrb_value keys = mrb_hash_keys(mrb, *mrb_hash);
  int env_len = RARRAY_LEN(keys);
  char **envp = (char **)mrb_malloc(mrb, sizeof(char *) * (env_len + 1));
  int i, j;

  for (i = 0; i < env_len; ++i) {
    mrb_value key = mrb_ary_ref(mrb, keys, i);
    mrb_value value = mrb_hash_get(mrb, *mrb_hash, key);
    mrb_value strv = mrb_convert_type(mrb, value, MRB_TT_STRING, "String", "to_str");
    asprintf(envp, "%s=%s", mrb_string_value_cstr(mrb, &key), mrb_string_value_cstr(mrb, &strv));
    envp++;
  }
  *envp = NULL;
  envp -= i;
  mrb_gc_arena_restore(mrb, ai);

  for (i = 0; i < env_len + 1; i++) {
    _DEBUGP("[mruby-exec] envp(%i): %s\n", i, envp[i]);
  }

  return envp;
}

static int mrb_value_to_strv(mrb_state *mrb, mrb_value *array, mrb_int len, char **result)
{
  mrb_value strv;
  char *buf;
  int i, j;

  if (len < 1) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "must have at least 1 argument");
  }

  int ai = mrb_gc_arena_save(mrb);
  for (i = 0; i < len; i++) {
    strv = mrb_convert_type(mrb, array[i], MRB_TT_STRING, "String", "to_str");
    buf = (char *)mrb_string_value_cstr(mrb, &strv);
    *result = buf;
    result++;
  }
  *result = NULL;

  // return to the top of array
  result -= i;

  for (j = 0; j < len + 1; j++) {
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

  if (mrb_value_to_strv(mrb, mrb_argv, len, result) < 0) {
    mrb_free(mrb, result);
    mrb_sys_fail(mrb, "[BUG] mrb_value_to_strv failed");
  }

  execv(result[0], result);

  mrb_free(mrb, result);
  mrb_sys_fail(mrb, "execv failed");

  return mrb_nil_value();
}

static mrb_value mrb_exec_do_execve(mrb_state *mrb, mrb_value self)
{
  mrb_value mrb_env;
  mrb_value *mrb_argv;
  mrb_int len;
  char **result, **envp;

  mrb_get_args(mrb, "H*", &mrb_env, &mrb_argv, &len);
  result = (char **)mrb_malloc(mrb, sizeof(char *) * (len + 1));

  if (mrb_value_to_strv(mrb, mrb_argv, len, result) < 0) {
    mrb_sys_fail(mrb, "[BUG] mrb_value_to_strv failed");
  }

  envp = mrb_hash_to_strv(mrb, &mrb_env);

  execve(result[0], result, envp);

  mrb_sys_fail(mrb, "execve failed");

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

  if (mrb_value_to_strv(mrb, argv, len, result) < 0) {
    mrb_sys_fail(mrb, "[BUG] mrb_value_to_strv failed");
  }

  execname = strdup(result[0]);
  result[0] = procname;
  execv(execname, result);

  mrb_sys_fail(mrb, "execv failed");

  return mrb_nil_value();
}

static mrb_value mrb_exec_execve_override_procname(mrb_state *mrb, mrb_value self)
{
  mrb_value mrb_env;
  mrb_value *argv;
  mrb_int len;
  char *procname, *execname;
  char **result, **envp;

  mrb_get_args(mrb, "Hz*", &mrb_env, &procname, &argv, &len);
  result = (char **)mrb_malloc(mrb, sizeof(char *) * (len + 1));

  if (mrb_value_to_strv(mrb, argv, len, result) < 0) {
    mrb_sys_fail(mrb, "[BUG] mrb_value_to_strv failed");
  }

  envp = mrb_hash_to_strv(mrb, &mrb_env);

  execname = strdup(result[0]);
  result[0] = procname;
  execve(execname, result, envp);

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
  mrb_define_class_method(mrb, ex, "execve_override_procname", mrb_exec_execve_override_procname, MRB_ARGS_ANY());

  mrb_define_class_method(mrb, ex, "execve", mrb_exec_do_execve, MRB_ARGS_ANY());

  mrb_define_method(mrb, mrb->kernel_module, "exec", mrb_exec_do_exec, MRB_ARGS_ANY());
  DONE;
}

void mrb_mruby_exec_gem_final(mrb_state *mrb)
{
}
