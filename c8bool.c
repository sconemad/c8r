/** c8bool - boolean type
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

#include "c8bool.h"
#include "c8obj.h"
#include "c8buf.h"
#include "c8objimp.h"

#include <assert.h>
#include <stdlib.h>

struct c8bool {
  struct c8obj base;
  int value;
};

static void c8bool_destroy(struct c8obj* o)
{
  struct c8bool* bo = to_c8bool(o);
  assert(bo);
  free(bo);
}

static struct c8obj* c8bool_copy(const struct c8obj* o)
{
  const struct c8bool* bo = to_const_c8bool(o);
  assert(bo);
  return (struct c8obj*)c8bool_create(bo->value);
}

static int c8bool_int(const struct c8obj* o)
{
  const struct c8bool* bo = to_const_c8bool(o);
  assert(bo);
  return bo->value;
}

static void c8bool_str(const struct c8obj* o, struct c8buf* buf, int f)
{
  const struct c8bool* bo = to_const_c8bool(o);
  assert(bo);
  c8buf_append_str(buf, bo->value ? "true" : "false");
}

static struct c8obj* c8bool_op(struct c8obj* o, int op, struct c8obj* p)
{
  struct c8bool* bo = to_c8bool(o);
  assert(bo);
  return 0;
}

static const struct c8obj_imp c8bool_imp = {
  c8bool_destroy,
  c8bool_copy,
  c8bool_int,
  c8bool_str,
  c8bool_op
};

const struct c8bool* to_const_c8bool(const struct c8obj* o)
{
  return (o && o->imp && o->imp == &c8bool_imp) ?
    (const struct c8bool*)o : 0;
}

struct c8bool* to_c8bool(struct c8obj* o)
{
  return (struct c8bool*)to_const_c8bool(o);
}

struct c8bool* c8bool_create(int value)
{
  struct c8bool* bo = malloc(sizeof(struct c8bool));
  assert(bo);
  bo->base.refs = 1;
  bo->base.imp = &c8bool_imp;
  bo->value = value;
  return bo;
}

void c8bool_set(struct c8bool* bo, int value)
{
  assert(bo);
  bo->value = value;
}

int c8bool_value(const struct c8bool* bo)
{
  assert(bo);
  return bo->value;
}
