/** c8func - object representing a function or method call
 *
 * Copyright (c) 2017 Andrew Wedgbury <wedge@sconemad.com>
 *
 * This file is part of c8r.
 *
 * c8r is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * c8r is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with c8r.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "c8func.h"
#include "c8obj.h"
#include "c8buf.h"
#include "c8objimp.h"
#include "c8ops.h"
#include "c8list.h"

#include <assert.h>
#include <stdlib.h>

struct c8func {
  struct c8obj base;
  c8func_func func;
  struct c8obj* object;
};

static void c8func_destroy(struct c8obj* o)
{
  struct c8func* fo = to_c8func(o);
  assert(fo);
  c8obj_unref(fo->object);
  free(fo);
}

static struct c8obj* c8func_copy(const struct c8obj* o)
{
  const struct c8func* fo = to_const_c8func(o);
  assert(fo);
  return (struct c8obj*)c8func_create(fo->func, fo->object);
}

static int c8func_int(const struct c8obj* o)
{
  const struct c8func* fo = to_const_c8func(o);
  assert(fo);
  return fo->func || fo->object;
}

static void c8func_str(const struct c8obj* o, struct c8buf* buf, int f)
{
  const struct c8func* fo = to_const_c8func(o);
  assert(fo);
  c8buf_append_str(buf, "function");
}

static struct c8obj* c8func_op(struct c8obj* o, int op, struct c8obj* p)
{
  struct c8func* fo = to_c8func(o);
  assert(fo);
  if (C8_OP_LIST == op) {
    struct c8list* args = to_c8list(p);
    if (args) return c8func_call(fo, args);
  }
  return 0;
}

static const struct c8obj_imp c8func_imp = {
  c8func_destroy,
  c8func_copy,
  c8func_int,
  c8func_str,
  c8func_op
};

const struct c8func* to_const_c8func(const struct c8obj* o)
{
  return (o && o->imp && o->imp == &c8func_imp) ?
    (const struct c8func*)o : 0;
}

struct c8func* to_c8func(struct c8obj* o)
{
  return (struct c8func*)to_const_c8func(o);
}

struct c8func* c8func_create(c8func_func f, struct c8obj* obj)
{
  assert(f);
  struct c8func* fo = malloc(sizeof(struct c8func));
  assert(fo);
  fo->base.refs = 1;
  fo->base.imp = &c8func_imp;
  fo->func = f;
  fo->object = 0;
  if (obj) fo->object = c8obj_ref(obj);
  return fo;
}

struct c8obj* c8func_call(struct c8func* fo, struct c8list* args)
{
  assert(fo);
  if (fo->object) c8list_push_front(args, fo->object);
  return (fo->func)(args);
}
