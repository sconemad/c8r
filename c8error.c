/** c8error - object representing an error
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

#include "c8error.h"
#include "c8objimp.h"
#include "c8buf.h"

#include <assert.h>
#include <stdlib.h>

struct c8error {
  struct c8obj base;
  int code;
};

static void c8error_destroy(struct c8obj* o)
{
  struct c8error* eo = to_c8error(o);
  assert(eo);
  free(eo);
}

static struct c8obj* c8error_copy(const struct c8obj* o)
{
  const struct c8error* eo = to_const_c8error(o);
  assert(eo);
  return (struct c8obj*)c8error_create(eo->code);
}

static int c8error_int(const struct c8obj* o)
{
  const struct c8error* eo = to_const_c8error(o);
  assert(eo);
  return 0; // Errors are always zero/false
}

static void c8error_str(const struct c8obj* o, struct c8buf* buf, int f)
{
  const struct c8error* eo = to_const_c8error(o);
  assert(eo);
  c8buf_append_fmt(buf, "error(%d): ", eo->code);
  switch (eo->code) {
    case C8_ERROR_UNDEFINED_NAME:
      c8buf_append_str(buf, "undefined name"); break;
    case C8_ERROR_ARGUMENT:
      c8buf_append_str(buf, "argument"); break;
    case C8_ERROR_MAP_INIT:
      c8buf_append_str(buf, "bad map initializer"); break;
    case C8_ERROR_LIST_INIT:
      c8buf_append_str(buf, "bad list initializer"); break;
    case C8_ERROR_PARENTHESIS:
      c8buf_append_str(buf, "mismatched parenthesis"); break;
    default:
      c8buf_append_str(buf, "unknown"); break;
  }
}

static struct c8obj* c8error_op(struct c8obj* o, int op, struct c8obj* p)
{
  struct c8error* eo = to_c8error(o);
  assert(eo);
  return 0;
}

static const struct c8obj_imp c8error_imp = {
  c8error_destroy,
  c8error_copy,
  c8error_int,
  c8error_str,
  c8error_op
};

const struct c8error* to_const_c8error(const struct c8obj* o)
{
  return (o && o->imp && o->imp == &c8error_imp) ?
    (const struct c8error*)o : 0;
}

struct c8error* to_c8error(struct c8obj* o)
{
  return (struct c8error*)to_const_c8error(o);
}

struct c8error* c8error_create(int code)
{
  struct c8error* eo = malloc(sizeof(struct c8error));
  assert(eo);
  eo->base.refs = 1;
  eo->base.imp = &c8error_imp;
  eo->code = code;
  return eo;
}

void c8error_set(struct c8error* eo, int code)
{
  assert(eo);
  eo->code = code;
}

int c8error_code(const struct c8error* eo)
{
  assert(eo);
  return eo->code;
}
