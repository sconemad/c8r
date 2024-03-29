/** c8string - string object
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

#include "c8string.h"
#include "c8buf.h"
#include "c8obj.h"
#include "c8objimp.h"
#include "c8ops.h"
#include "c8bool.h"
#include "c8ctx.h"
#include "c8func.h"
#include "c8list.h"
#include "c8error.h"
#include "c8mpz.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct c8string {
  struct c8obj base;
  struct c8buf buf;
};

static void c8string_destroy(struct c8obj* o)
{
  struct c8string* oo = to_c8string(o);
  assert(oo);
  c8buf_clear(&oo->buf);
  free(oo);
}

static struct c8obj* c8string_copy(const struct c8obj* o)
{
  const struct c8string* oo = to_const_c8string(o);
  assert(oo);
  return (struct c8obj*)c8string_create_buf(&oo->buf);
}

static int c8string_int(const struct c8obj* o)
{
  const struct c8string* oo = to_const_c8string(o);
  assert(oo);
  return c8buf_len(&oo->buf);
}

static void c8string_str(const struct c8obj* o, struct c8buf* buf, int f)
{
  const struct c8string* oo = to_const_c8string(o);
  assert(oo);
  c8buf_append_buf(buf, &oo->buf);
}

static struct c8obj* c8string_op(struct c8obj* o, int op, struct c8obj* p)
{
  struct c8string* oo = to_c8string(o);
  assert(oo);

  if (op == C8_OP_LOOKUP && p) {
    struct c8buf nb; c8buf_init(&nb); c8obj_str(p, &nb, 0);
    const char* name = c8buf_str(&nb);
    struct c8func* fn = 0;
    if (strcmp("size", name)==0) fn = c8func_create_method(c8string_size, o);
    c8buf_clear(&nb);
    return (struct c8obj*)fn;
  }

  switch (op) {
    case C8_OP_ADD: {
      struct c8string* sr = (struct c8string*)c8string_copy(o);
      c8obj_str(p, &sr->buf, 0);
      return (struct c8obj*)sr;
    }
    case C8_OP_ASSIGN: {
      c8buf_clear(&oo->buf);
      c8obj_str(p, &oo->buf, 0);
      return c8obj_ref(o);
    }
    case C8_OP_ADD_ASSIGN: {
      c8obj_str(p, &oo->buf, 0);
      return c8obj_ref(o);
    }
    case C8_OP_EQUALITY: case C8_OP_INEQUALITY: {
      struct c8buf bp; c8buf_init(&bp);
      c8obj_str(p, &bp, 0);
      int cmp = strcmp(c8buf_str(&oo->buf), c8buf_str(&bp));
      int ret = (op==C8_OP_EQUALITY) ? cmp==0 : cmp!=0;
      c8buf_clear(&bp);
      return (struct c8obj*)c8bool_create(ret);
    }
  }
  return 0;
}

static const struct c8obj_imp c8string_imp = {
  c8string_destroy,
  c8string_copy,
  c8string_int,
  c8string_str,
  c8string_op
};

const struct c8string* to_const_c8string(const struct c8obj* o)
{
  return (o && o->imp && o->imp == &c8string_imp) ?
    (const struct c8string*)o : 0;
}

struct c8string* to_c8string(struct c8obj* o)
{
  return (struct c8string*)to_const_c8string(o);
}

struct c8string* c8string_create()
{
  struct c8string* oo = malloc(sizeof(struct c8string));
  assert(oo);
  c8obj_init(&oo->base, &c8string_imp);
  c8buf_init(&oo->buf);
  return oo;
}

struct c8string* c8string_create_str(const char* str)
{
  struct c8string* oo = c8string_create();
  c8buf_init_str(&oo->buf, str);
  return oo;
}

struct c8string* c8string_create_buf(const struct c8buf* buf)
{
  struct c8string* oo = c8string_create();
  c8buf_init_copy(&oo->buf, buf);
  return oo;
}

void c8string_init_ctx(struct c8ctx* ctx)
{
  c8ctx_add(ctx, "str", (struct c8obj*)c8func_create(c8string_to_str));
}

struct c8obj* c8string_to_str(struct c8list* args)
{
  if (c8list_size(args) != 1)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  struct c8obj* a = c8list_at(args, 0);
  if (!a)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  
  struct c8string* oo = c8string_create();
  c8obj_str(a, &oo->buf, C8_FMT_DEC);
  c8obj_unref(a);
  return (struct c8obj*)oo;
}

struct c8obj* c8string_size(struct c8list* args)
{
  if (c8list_size(args) != 1)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  struct c8obj* a = c8list_at(args, 0);
  if (!a)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  
  struct c8string* s = to_c8string(a);
  if (!s) {
    c8obj_unref(a);
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  }

  int len = c8buf_len(&s->buf);
  c8obj_unref(a);
  return (struct c8obj*)c8mpz_create_int(len);
}
