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
#include "mruby/data.h"
#include "mrb_exec.h"

#define DONE mrb_gc_arena_restore(mrb, 0);

static mrb_value mrb_exec_do_exec(mrb_state *mrb, mrb_value self)
{
  char *newargv[] = { NULL, "-l", NULL };
  char *newenviron[] = { NULL };

  execve("/bin/bash", newargv, newenviron);

  perror("execve");
  return mrb_nil_value();
}

void mrb_mruby_exec_gem_init(mrb_state *mrb)
{
    struct RClass *e;
    e = mrb_define_class(mrb, "Exec", mrb->object_class);
    mrb_define_class_method(mrb, e, "exec", mrb_exec_do_exec, MRB_ARGS_ANY());
    mrb_define_class_method(mrb, e, "execve", mrb_exec_do_exec, MRB_ARGS_ANY());
    DONE;
}

void mrb_mruby_exec_gem_final(mrb_state *mrb)
{
}
