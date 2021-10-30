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
#include "c8obj.h"
#include "c8objimp.h"
#include "c8ops.h"
#include "c8bool.h"
#include "c8error.h"
#include "c8func.h"
#include "c8ctx.h"
#include "c8list.h"
#include "c8buf.h"

#include "c8mpz.h"
#include "c8mpfr.h"
#include "c8mpc.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct c8num {
  struct c8obj base;
  struct c8obj* value;
};

static void c8num_destroy(struct c8obj* o)
{
  struct c8num* oo = to_c8num(o);
  assert(oo);
  c8obj_unref(oo->value);
  free(oo);
}

static struct c8obj* c8num_copy(const struct c8obj* o)
{
  const struct c8num* oo = to_const_c8num(o);
  assert(oo);
  struct c8num* no = c8num_create();
  no->value = c8obj_copy(oo->value);
  return (struct c8obj*)no;
}

static int c8num_int(const struct c8obj* o)
{
  const struct c8num* oo = to_const_c8num(o);
  assert(oo);
  return c8obj_int(oo->value);
}

static void c8num_str(const struct c8obj* o, struct c8buf* buf, int f)
{
  const struct c8num* oo = to_const_c8num(o);
  assert(oo);
  c8obj_str(oo->value, buf, f);

}

static struct c8obj* c8num_binary_op(struct c8obj* o, int op, 
                                      struct c8obj* p)
{
  //TODO: conversions
  return c8obj_op(o, op, p);
}

static struct c8obj* c8num_op(struct c8obj* o, int op, struct c8obj* p)
{
  struct c8num* oo = to_c8num(o);
  assert(oo);
  if (!p) return c8obj_op(oo->value, op, p); // Unary op
  if (op == C8_OP_LOOKUP || op == C8_OP_RESOLVE) return c8obj_op(oo->value, op, p);

  struct c8num* np = to_c8num(p);
  if (np) return c8num_binary_op(oo->value, op, np->value); // Binary op with c8num

  // Otherwise convert p to c8num and perform the op
  struct c8buf buf; c8buf_init(&buf);
  c8obj_str(p, &buf, 0);
  np = c8num_create_str(c8buf_str(&buf));
  c8buf_clear(&buf);
  if (!np) return 0;
  struct c8obj* r = c8num_binary_op(oo->value, op, np->value);
  c8obj_unref((struct c8obj*)np);
  return r;
}

static const struct c8obj_imp c8num_imp = {
  c8num_destroy,
  c8num_copy,
  c8num_int,
  c8num_str,
  c8num_op
};

const struct c8num* to_const_c8num(const struct c8obj* o)
{
  return (o && o->imp && o->imp == &c8num_imp) ?
    (const struct c8num*)o : 0;
}

struct c8num* to_c8num(struct c8obj* o)
{
  return (struct c8num*)to_const_c8num(o);
}

struct c8num* c8num_create()
{
  struct c8num* oo = malloc(sizeof(struct c8num));
  assert(oo);
  oo->base.refs = 1;
  oo->base.imp = &c8num_imp;
  oo->value = 0;
  return oo;
}

struct c8num* c8num_create_int(int value)
{
  struct c8num* oo = c8num_create();
  oo->value = (struct c8obj*)c8mpz_create_int(value);
  return oo;
}

struct c8num* c8num_create_double(long double value)
{
  struct c8num* oo = c8num_create();
  oo->value = (struct c8obj*)c8mpfr_create_double(value);
  return oo;
}

struct c8num* c8num_create_cplx(long double real, long double imag)
{
  struct c8num* oo = c8num_create();
  oo->value = (struct c8obj*)c8mpc_create_double(real, imag);
  return oo;
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
  struct c8num* oo = c8num_create();
  if (ex || dd) {
    oo->value = (struct c8obj*)c8mpfr_create_str(c8buf_str(&str));
  } else {
    oo->value = (struct c8obj*)c8mpz_create_str(c8buf_str(&str));
  }
  c8buf_clear(&str);
  return oo;
}

struct c8num* c8num_create_c8obj(const struct c8obj* obj)
{
  assert(obj);
  struct c8num* oo = c8num_create();
  oo->value = c8obj_copy(oo->value);
  return oo;
}

const struct c8obj* c8num_const_value(const struct c8num* oo)
{
  assert(oo);
  return oo->value;
}

struct c8obj* c8num_value(struct c8num* oo)
{
  assert(oo);
  return oo->value;
}

void c8num_init_ctx(struct c8ctx* ctx)
{
  c8mpz_init_ctx(ctx);
  c8mpfr_init_ctx(ctx);
  c8mpc_init_ctx(ctx);
}

