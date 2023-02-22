/** c8mpz - numeric object using GNU mpz
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

#include "c8mpz.h"
#include "c8obj.h"
#include "c8buf.h"
#include "c8num.h"
#include "c8numimp.h"
#include "c8ops.h"
#include "c8bool.h"
#include "c8list.h"
#include "c8func.h"
#include "c8mpfr.h"
#include "c8ctx.h"
#include "c8list.h"
#include "c8error.h"

#include <gmp.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct c8mpz {
  struct c8num base;
  mpz_t value;
};

struct c8mpz* c8mpz_create_mpz(const mpz_t value);

static void c8mpz_destroy(struct c8obj* o)
{
  struct c8mpz* oo = to_c8mpz(o);
  assert(oo);
  mpz_clear(oo->value);
  free(oo);
}

static struct c8obj* c8mpz_copy(const struct c8obj* o)
{
  const struct c8mpz* oo = to_const_c8mpz(o);
  assert(oo);
  return (struct c8obj*)c8mpz_create_mpz(oo->value);
}

static int c8mpz_int(const struct c8obj* o)
{
  const struct c8mpz* oo = to_const_c8mpz(o);
  assert(oo);
  return (int)mpz_get_si(oo->value);
}

static void c8mpz_str(const struct c8obj* o, struct c8buf* buf, int f)
{
  const struct c8mpz* oo = to_const_c8mpz(o);
  assert(oo);
  int base = 10;
  switch (f & C8_FMT_MASK_BASE) {
    case C8_FMT_BIN: base = 2; break;
    case C8_FMT_OCT: base = 8; break;
    case C8_FMT_HEX: base = 16; break;
  }

  char* cs = mpz_get_str(0, base, oo->value);
  if (cs) {
    switch (base) {
      case 2: c8buf_append_str(buf, "0b"); break;
      case 8: c8buf_append_str(buf, "0o"); break;
      case 16: c8buf_append_str(buf, "0x"); break;
    }
    c8buf_append_str(buf, cs);
    free(cs);
  }
}

static struct c8obj* c8mpz_binary_op(struct c8mpz* oo, int op,
                                     struct c8mpz* np)
{
  switch (op) {
    case C8_OP_ADD: {
      struct c8mpz* nr = c8mpz_create();
      mpz_add(nr->value, oo->value, np->value);
      return (struct c8obj*)nr;
    }
    case C8_OP_SUBTRACT: {
      struct c8mpz* nr = c8mpz_create();
      mpz_sub(nr->value, oo->value, np->value);
      return (struct c8obj*)nr;
    }
    case C8_OP_MULTIPLY: {
      struct c8mpz* nr = c8mpz_create();
      mpz_mul(nr->value, oo->value, np->value);
      return (struct c8obj*)nr;
    }
    case C8_OP_DIVIDE: {
      if (mpz_divisible_p(oo->value, np->value)) {
        struct c8mpz* nr = c8mpz_create();
        mpz_tdiv_q(nr->value, oo->value, np->value);
        return (struct c8obj*)nr;
      }
      return (struct c8obj*)c8error_create(C8_ERROR_PRECISION_REAL);
    }
    case C8_OP_MODULUS: {
      struct c8mpz* nr = c8mpz_create();
      mpz_tdiv_r(nr->value, oo->value, np->value);
      return (struct c8obj*)nr;
    }
    case C8_OP_POWER: {
      struct c8mpz* nr = c8mpz_create();
      mpz_pow_ui(nr->value, oo->value, mpz_get_ui(np->value));
      return (struct c8obj*)nr;
    }
    case C8_OP_BIT_OR: {
      struct c8mpz* nr = c8mpz_create();
      mpz_ior(nr->value, oo->value, np->value);
      return (struct c8obj*)nr;
    }
    case C8_OP_BIT_XOR: {
      struct c8mpz* nr = c8mpz_create();
      mpz_xor(nr->value, oo->value, np->value);
      return (struct c8obj*)nr;
    }
    case C8_OP_BIT_AND: {
      struct c8mpz* nr = c8mpz_create();
      mpz_and(nr->value, oo->value, np->value);
      return (struct c8obj*)nr;
    }
    case C8_OP_SHIFT_LEFT: {
      struct c8mpz* nr = c8mpz_create();
      mpz_mul_2exp(nr->value, oo->value, mpz_get_si(np->value));
      return (struct c8obj*)nr;
    }
    case C8_OP_SHIFT_RIGHT: {
      struct c8mpz* nr = c8mpz_create();
      mpz_tdiv_q_2exp(nr->value, oo->value, mpz_get_si(np->value));
      return (struct c8obj*)nr;
    }

    case C8_OP_EQUALITY:
      return (struct c8obj*)c8bool_create(mpz_cmp(oo->value, np->value) == 0);
    case C8_OP_INEQUALITY:
      return (struct c8obj*)c8bool_create(mpz_cmp(oo->value, np->value) != 0);
    case C8_OP_GREATER:
      return (struct c8obj*)c8bool_create(mpz_cmp(oo->value, np->value) > 0);
    case C8_OP_LESS:
      return (struct c8obj*)c8bool_create(mpz_cmp(oo->value, np->value) < 0);
    case C8_OP_GREATER_OR_EQUAL:
      return (struct c8obj*)c8bool_create(mpz_cmp(oo->value, np->value) >= 0);
    case C8_OP_LESS_OR_EQUAL:
      return (struct c8obj*)c8bool_create(mpz_cmp(oo->value, np->value) <= 0);

    case C8_OP_ASSIGN: {
      mpz_set(oo->value, np->value);
      return c8obj_ref((struct c8obj*)oo);
    }
    case C8_OP_ADD_ASSIGN: {
      mpz_add(oo->value, oo->value, np->value);
      return c8obj_ref((struct c8obj*)oo);
    }
    case C8_OP_SUBTRACT_ASSIGN: {
      mpz_sub(oo->value, oo->value, np->value);
      return c8obj_ref((struct c8obj*)oo);
    }
    case C8_OP_MULTIPLY_ASSIGN: {
      mpz_mul(oo->value, oo->value, np->value);
      return c8obj_ref((struct c8obj*)oo);
    }
    case C8_OP_DIVIDE_ASSIGN: {
      mpz_tdiv_q(oo->value, oo->value, np->value);
      return c8obj_ref((struct c8obj*)oo);
    }
  }
  return 0;
}

static struct c8obj* c8mpz_lookup(struct c8obj* o, const char* name)
{
  if (strcmp("abs", name)==0) return (struct c8obj*)c8func_create_method(c8mpz_abs, o);
  if (strcmp("gcd", name)==0) return (struct c8obj*)c8func_create_method(c8mpz_gcd, o);
  if (strcmp("lcm", name)==0) return (struct c8obj*)c8func_create_method(c8mpz_lcm, o);
  if (strcmp("fib", name)==0) return (struct c8obj*)c8func_create_method(c8mpz_fib, o);
  return 0;
}

static struct c8obj* c8mpz_op(struct c8obj* o, int op, struct c8obj* p)
{
  struct c8mpz* oo = to_c8mpz(o);
  assert(oo);
  
  switch (op) {
    case C8_OP_LOOKUP: {
      struct c8buf nb; c8buf_init(&nb);
      c8obj_str(p, &nb, 0); 
      struct c8obj* ret = c8mpz_lookup(o, c8buf_str(&nb));
      c8buf_clear(&nb);
      return ret;
    }
    case C8_OP_POSITIVE: {
      struct c8mpz* nr = c8mpz_create();
      mpz_set(nr->value, oo->value);
      return (struct c8obj*)nr;
    }
    case C8_OP_NEGATIVE: {
      struct c8mpz* nr = c8mpz_create();
      mpz_neg(nr->value, oo->value);
      return (struct c8obj*)nr;
    }
    case C8_OP_BIT_NOT: {
      struct c8mpz* nr = c8mpz_create();
      mpz_com(nr->value, oo->value);
      return (struct c8obj*)nr;
    }
    case C8_OP_PRE_INC: {
      mpz_add_ui(oo->value, oo->value, 1);
      c8obj_ref(o);
      return o;
    }
    case C8_OP_PRE_DEC: {
      mpz_sub_ui(oo->value, oo->value, 1);
      c8obj_ref(o);
      return o;
    }
    case C8_OP_FACTORIAL: {
      struct c8mpz* nr = c8mpz_create();
      mpz_fac_ui(nr->value, mpz_get_ui(oo->value));
      return (struct c8obj*)nr;
    }
    case C8_OP_POST_INC: {
      struct c8mpz* nr = c8mpz_create_mpz(oo->value);
      mpz_add_ui(oo->value, oo->value, 1);
      return (struct c8obj*)nr;
    }
    case C8_OP_POST_DEC: {
      struct c8mpz* nr = c8mpz_create_mpz(oo->value);
      mpz_sub_ui(oo->value, oo->value, 1);
      return (struct c8obj*)nr;
    }
  }

  struct c8mpz* np = to_c8mpz(p);
  if (np) return c8mpz_binary_op(oo, op, np);

  return (struct c8obj*)c8error_create(C8_ERROR_PRECISION_REAL);
  //  return c8mpz_op_mpfr(oo, op, p);
}

static const struct c8obj_imp c8mpz_imp = {
  c8mpz_destroy,
  c8mpz_copy,
  c8mpz_int,
  c8mpz_str,
  c8mpz_op
};

const struct c8mpz* to_const_c8mpz(const struct c8obj* o)
{
  const struct c8num* on = to_const_c8num(o);
  return (on && on->imp && on->imp == &c8mpz_imp) ?
    (const struct c8mpz*)o : 0;
}

struct c8mpz* to_c8mpz(struct c8obj* o)
{
  return (struct c8mpz*)to_const_c8mpz(o);
}

struct c8mpz* c8mpz_create()
{
  struct c8mpz* oo = (struct c8mpz*)malloc(sizeof(struct c8mpz));
  assert(oo);
  c8num_init(&oo->base, &c8mpz_imp);
  mpz_init(oo->value);
  return oo;
}

struct c8mpz* c8mpz_create_mpz(const mpz_t value)
{
  struct c8mpz* oo = c8mpz_create();
  mpz_set(oo->value, value);
  return oo;
}

struct c8mpz* c8mpz_create_int(int value)
{
  struct c8mpz* oo = c8mpz_create();
  mpz_set_si(oo->value, value);
  return oo;
}

struct c8mpz* c8mpz_create_double(double value)
{
  struct c8mpz* oo = c8mpz_create();
  mpz_set_d(oo->value, value);
  return oo;
}

struct c8mpz* c8mpz_create_str(const char* str)
{
  struct c8mpz* oo = c8mpz_create();
  int len = strlen(str);
  int base = 10;
  if (len > 2 && str[0] == '0') {
    switch (str[1]) {
      case 'b': base = 2; str+=2; break;
      case 'o': base = 8; str+=2; break;
      case 'd': base = 10; str+=2; break;
      case 'x': base = 16; str+=2; break;
    }
  }
  mpz_set_str(oo->value, str, base);
  return oo;
}

static struct c8num* c8mpz_int_create(const char* str)
{
  return (struct c8num*)c8mpz_create_str(str);
}

void c8mpz_init_ctx(struct c8ctx* ctx)
{
  c8num_register_int_create(c8mpz_int_create);
}

struct c8obj* c8mpz_abs(struct c8list* args)
{
  if (c8list_size(args) != 1)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  struct c8obj* a = c8list_at(args, 0);
  if (!a)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  struct c8mpz* na = to_c8mpz(a);
  struct c8mpz* nr = 0;
  if (na) {
    nr = c8mpz_create();
    mpz_abs(nr->value, na->value);
  }
  c8obj_unref(a);
  if (nr) return (struct c8obj*)nr;
  
  return c8mpfr_abs(args); // Go on to try the mpfr version
}

struct c8obj* c8mpz_gcd(struct c8list* args)
{
  int n = c8list_size(args);
  if (n == 0)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  struct c8obj* a = c8list_at(args, 0);
  if (!a)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  struct c8mpz* na = to_c8mpz(a);
  struct c8mpz* nr = c8mpz_create_mpz(na->value);
  
  for (int i=1; i<n; ++i) {
    struct c8obj* b = c8list_at(args, i);
    if (a && b) {
      struct c8mpz* nb = to_c8mpz(b);
      if (nb) mpz_gcd(nr->value, nr->value, nb->value);
      na = nb;
    }
    c8obj_unref(a);
    a = b;
  }
  
  c8obj_unref(a);
  return (struct c8obj*)nr;
}

struct c8obj* c8mpz_lcm(struct c8list* args)
{
  int n = c8list_size(args);
  if (n == 0)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  struct c8obj* a = c8list_at(args, 0);
  if (!a)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  struct c8mpz* na = to_c8mpz(a);
  struct c8mpz* nr = c8mpz_create_mpz(na->value);
  
  for (int i=1; i<n; ++i) {
    struct c8obj* b = c8list_at(args, i);
    if (a && b) {
      struct c8mpz* nb = to_c8mpz(b);
      if (nb) mpz_lcm(nr->value, nr->value, nb->value);
      na = nb;
    }
    c8obj_unref(a);
    a = b;
  }
  
  c8obj_unref(a);
  return (struct c8obj*)nr;
}

struct c8obj* c8mpz_fib(struct c8list* args)
{
  if (c8list_size(args) != 1)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  struct c8obj* a = c8list_at(args, 0);
  if (!a)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  struct c8mpz* na = to_c8mpz(a);
  struct c8mpz* nr = 0;
  if (na) {
    nr = c8mpz_create();
    mpz_fib_ui(nr->value, mpz_get_ui(na->value));
  }
  c8obj_unref(a);
  return (struct c8obj*)nr;
}
