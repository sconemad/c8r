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
#include "c8ops.h"

#include <assert.h>
#include <stdlib.h>

struct c8bool {
  struct c8obj base;
  int value;
};

static void c8bool_destroy(struct c8obj* o)
{
  struct c8bool* oo = to_c8bool(o);
  assert(oo);
  free(oo);
}

static struct c8obj* c8bool_copy(const struct c8obj* o)
{
  const struct c8bool* oo = to_const_c8bool(o);
  assert(oo);
  return (struct c8obj*)c8bool_create(oo->value);
}

static int c8bool_int(const struct c8obj* o)
{
  const struct c8bool* oo = to_const_c8bool(o);
  assert(oo);
  return oo->value;
}

static void c8bool_str(const struct c8obj* o, struct c8buf* buf, int f)
{
  const struct c8bool* oo = to_const_c8bool(o);
  assert(oo);
  c8buf_append_str(buf, oo->value ? "true" : "false");
}

static struct c8obj* c8bool_binary_op(struct c8bool* oo, int op,
                                      struct c8bool* bp)
{
  switch (op) {
    case C8_OP_EQUALITY:
      return (struct c8obj*)c8bool_create(oo->value == bp->value);
    case C8_OP_INEQUALITY:
      return (struct c8obj*)c8bool_create(oo->value != bp->value);

    case C8_OP_LOGIC_OR:
      return (struct c8obj*)c8bool_create(oo->value || bp->value);
    case C8_OP_LOGIC_AND:
      return (struct c8obj*)c8bool_create(oo->value && bp->value);

    case C8_OP_ASSIGN:
      oo->value = bp->value;
      return c8obj_ref((struct c8obj*)oo);
  }
  return 0;
}

static struct c8obj* c8bool_op(struct c8obj* o, int op, struct c8obj* p)
{
  struct c8bool* oo = to_c8bool(o);
  assert(oo);

  switch(op) {
    case C8_OP_LOGIC_NOT: {
      return (struct c8obj*)c8bool_create(!oo->value);
    }
  }

  struct c8bool* bp = to_c8bool(p);
  if (bp) return c8bool_binary_op(oo, op, bp);
  
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
  struct c8bool* oo = malloc(sizeof(struct c8bool));
  assert(oo);
  c8obj_init(&oo->base, &c8bool_imp);
  oo->value = value;
  return oo;
}

void c8bool_set(struct c8bool* oo, int value)
{
  assert(oo);
  oo->value = value;
}

int c8bool_value(const struct c8bool* oo)
{
  assert(oo);
  return oo->value;
}
