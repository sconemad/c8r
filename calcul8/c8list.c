/** c8list - list object
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

#include "c8list.h"
#include "c8obj.h"
#include "c8objimp.h"
#include "c8vec.h"
#include "c8buf.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

struct c8list {
  struct c8obj base;
  struct c8vec vec;
};

static void c8list_destroy(struct c8obj* o)
{
  struct c8list* oo = to_c8list(o);
  assert(oo);
  int size = c8vec_size(&oo->vec);
  for (int i=0; i<size; ++i) {
    struct c8obj* item = (struct c8obj*)c8vec_at(&oo->vec, i);
    c8obj_unref(item);
  }
  c8vec_clear(&oo->vec);
  free(oo);
}

static struct c8obj* c8list_copy(const struct c8obj* o)
{
  const struct c8list* oo = to_const_c8list(o);
  assert(oo);
  return 0;
}

static int c8list_int(const struct c8obj* o)
{
  const struct c8list* oo = to_const_c8list(o);
  assert(oo);
  return c8vec_size(&oo->vec);
}

static void c8list_str(const struct c8obj* o, struct c8buf* buf, int f)
{
  const struct c8list* oo = to_const_c8list(o);
  assert(oo);
  c8buf_append_str(buf, "[");
  int size = c8vec_size(&oo->vec);
  for (int i=0; i<size; ++i) {
    if (i!=0) c8buf_append_str(buf, ",");
    const struct c8obj* item =
      (const struct c8obj*)c8vec_const_at(&oo->vec, i);
    if (item) c8obj_str(item, buf, f);
  }
  c8buf_append_str(buf, "]");
}

static struct c8obj* c8list_op(struct c8obj* o, int op, struct c8obj* p)
{
  struct c8list* oo = to_c8list(o);
  assert(oo);
  return 0;
}

static const struct c8obj_imp c8list_imp = {
  c8list_destroy,
  c8list_copy,
  c8list_int,
  c8list_str,
  c8list_op
};

const struct c8list* to_const_c8list(const struct c8obj* o)
{
  return (o && o->imp && o->imp == &c8list_imp) ?
    (const struct c8list*)o : 0;
}

struct c8list* to_c8list(struct c8obj* o)
{
  return (struct c8list*)to_const_c8list(o);
}

struct c8list* c8list_create()
{
  struct c8list* oo = malloc(sizeof(struct c8list));
  assert(oo);
  oo->base.refs = 1;
  oo->base.imp = &c8list_imp;
  c8vec_init(&oo->vec);
  return oo;
}

int c8list_size(const struct c8list* oo)
{
  assert(oo);
  return c8vec_size(&oo->vec);
}

struct c8obj* c8list_at(struct c8list* oo, int i)
{
  assert(oo);
  struct c8obj* r = (struct c8obj*)c8vec_at(&oo->vec, i);
  if (r) return c8obj_ref(r);
  return 0;
}

void c8list_push_back(struct c8list* oo, struct c8obj* p)
{
  assert(oo);
  if (p) c8obj_ref(p);
  c8vec_push_back(&oo->vec, p);
}

void c8list_pop_back(struct c8list* oo)
{
  assert(oo);
  struct c8obj* p = (struct c8obj*)c8vec_pop_back(&oo->vec);
  c8obj_unref(p);
}

void c8list_push_front(struct c8list* oo, struct c8obj* p)
{
  assert(oo);
  if (p) c8obj_ref(p);
  c8vec_push_front(&oo->vec, p);
}

void c8list_pop_front(struct c8list* oo)
{
  assert(oo);
  struct c8obj* p = (struct c8obj*)c8vec_pop_front(&oo->vec);
  c8obj_unref(p);
}
