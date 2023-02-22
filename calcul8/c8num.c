/** c8num - generic numeric object
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

#include "c8num.h"
#include "c8numimp.h"
#include "c8obj.h"
#include "c8ops.h"
#include "c8bool.h"
#include "c8error.h"
#include "c8func.h"
#include "c8ctx.h"
#include "c8list.h"
#include "c8buf.h"
#include "c8debug.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static c8num_type_create_func c8num_int_create_func = 0;
static c8num_type_create_func c8num_real_create_func = 0;
static c8num_type_create_func c8num_cplx_create_func = 0;

static void c8num_destroy(struct c8obj* o)
{
  struct c8num* oo = to_c8num(o);
  assert(oo);
  (oo->imp->destroy)(o);
}

static struct c8obj* c8num_copy(const struct c8obj* o)
{
  const struct c8num* oo = to_const_c8num(o);
  assert(oo);
  return (oo->imp->copy)(o);
}

static int c8num_int(const struct c8obj* o)
{
  const struct c8num* oo = to_const_c8num(o);
  assert(oo);
  return (oo->imp->to_int)(o);
}

static void c8num_str(const struct c8obj* o, struct c8buf* buf, int f)
{
  const struct c8num* oo = to_const_c8num(o);
  assert(oo);
  (oo->imp->to_str)(o, buf, f);
}

static struct c8obj* c8num_op(struct c8obj* o, int op, struct c8obj* p)
{
  struct c8num* oo = to_c8num(o);
  assert(oo);

  struct c8obj* r = (oo->imp->op)(o, op, p);
  struct c8error* re = to_c8error(r);
  if (re) {
    if (c8error_code(re) == C8_ERROR_PRECISION_REAL) {
      struct c8buf buf; c8buf_init(&buf);
      c8obj_str(o, &buf, 0);
      struct c8obj* or = (struct c8obj*)c8num_real_create_func(c8buf_str(&buf));
      c8buf_clear(&buf);
      struct c8obj* pr = 0;
      if (p) {
        c8obj_str(p, &buf, 0);
        pr = (struct c8obj*)c8num_real_create_func(c8buf_str(&buf));
        c8buf_clear(&buf);
      }
      c8obj_unref(r);
      r = c8obj_op(or, op, pr);
      c8obj_unref(or);
      c8obj_unref(pr);
    }
  }
  return r;
}

static const struct c8obj_imp c8num_imp = {
  c8num_destroy,
  c8num_copy,
  c8num_int,
  c8num_str,
  c8num_op
};

void c8num_init(struct c8num* oo, const struct c8obj_imp* imp)
{
  c8obj_init(&oo->base, &c8num_imp);
  oo->imp = imp;
}

const struct c8num* to_const_c8num(const struct c8obj* o)
{
  return (o && o->imp && o->imp == &c8num_imp) ?
    (const struct c8num*)o : 0;
}

struct c8num* to_c8num(struct c8obj* o)
{
  return (struct c8num*)to_const_c8num(o);
}

struct c8num* c8num_create_str(const char* str)
{
  const char** c = &str;
  return c8num_parse(c);
}

struct c8num* c8num_parse(const char** c)
{
  int ends = 1;
  int ex = 0; // counts exponentiations
  int dd = 0; // counts decimal points
  int hex = 0;

  const char* start = *c;
  char pc = *start; // previous character

  for (; **c; ++*c) {
    ends = !isalnum(**c);
    switch (**c) {
      case '.':
        ends = 0;
        ++dd;
        break;
      case 'x': case 'X':
        ends = (++hex>1);
        break;
      case 'a': case 'A':
      case 'b': case 'B':
      case 'c': case 'C':
      case 'd': case 'D':
      case 'f': case 'F':
        if (hex==1) ends = 0;
        break;
      case 'e': case 'E':
        if (hex==1) ends=0;
        else ends = (++ex>1);
        break;
      case 'p': case 'P':
        if (hex==0) ends=1;
        else ends = (++ex>1);
        break;
      case 'o': case 'O':
        ends = 0;
        break;
      case '+': case '-':
        ends = (pc != 'e' && pc != 'E');
        break;
    }
    if (ends) break;
    pc = **c;
  }

  struct c8buf str; c8buf_init(&str);
  c8buf_append_strn(&str, start, *c - start);
  struct c8num* oo = 0;
  if (ex || dd) {
    oo = c8num_real_create_func(c8buf_str(&str));
  } else {
    oo = c8num_int_create_func(c8buf_str(&str));
  }
  c8buf_clear(&str);
  return oo;
}

struct c8obj* c8num_to_int(struct c8list* args)
{
  if (c8list_size(args) != 1)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  struct c8obj* a = c8list_at(args, 0);
  if (!a)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  
  struct c8buf buf; c8buf_init(&buf);
  c8obj_str(a, &buf, C8_FMT_DEC);
  c8obj_unref(a);

  struct c8obj* o = (struct c8obj*)c8num_int_create_func(c8buf_str(&buf));
  c8buf_clear(&buf);
  return o;
}

struct c8obj* c8num_to_real(struct c8list* args)
{
  if (c8list_size(args) != 1)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  struct c8obj* a = c8list_at(args, 0);
  if (!a)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  
  struct c8buf buf; c8buf_init(&buf);
  c8obj_str(a, &buf, C8_FMT_DEC);
  c8obj_unref(a);
  
  struct c8obj* o = (struct c8obj*)c8num_real_create_func(c8buf_str(&buf));
  c8buf_clear(&buf);
  return o;
}

struct c8obj* c8num_to_cplx(struct c8list* args)
{
  if (c8list_size(args) != 1)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  struct c8obj* a = c8list_at(args, 0);
  if (!a)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  
  struct c8buf buf; c8buf_init(&buf);
  c8obj_str(a, &buf, C8_FMT_DEC);
  c8obj_unref(a);
  
  struct c8obj* o = (struct c8obj*)c8num_cplx_create_func(c8buf_str(&buf));
  c8buf_clear(&buf);
  return o;
}

void c8num_init_ctx(struct c8ctx* ctx)
{
  c8ctx_add(ctx, "int", (struct c8obj*)c8func_create(c8num_to_int));
  c8ctx_add(ctx, "real", (struct c8obj*)c8func_create(c8num_to_real));
  c8ctx_add(ctx, "cplx", (struct c8obj*)c8func_create(c8num_to_cplx));
}

void c8num_register_int_create(c8num_type_create_func f)
{
  c8num_int_create_func = f;
}

void c8num_register_real_create(c8num_type_create_func f)
{
  c8num_real_create_func = f;
}

void c8num_register_cplx_create(c8num_type_create_func f)
{
  c8num_cplx_create_func = f;
}
