/** c8mpfr - numeric object using GNU mpfr
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

#include "c8mpfr.h"
#include "c8obj.h"
#include "c8ops.h"
#include "c8bool.h"
#include "c8error.h"
#include "c8func.h"
#include "c8ctx.h"
#include "c8list.h"
#include "c8buf.h"
#include "c8num.h"
#include "c8numimp.h"

#include <mpfr.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct c8mpfr {
  struct c8num base;
  mpfr_t value;
};

static mpfr_rnd_t rnd = GMP_RNDN;
static int dp = 0;

struct c8mpfr* c8mpfr_create_mpfr(const mpfr_t value);

static void c8mpfr_destroy(struct c8obj* o)
{
  struct c8mpfr* oo = to_c8mpfr(o);
  assert(oo);
  mpfr_clear(oo->value);
  free(oo);
}

static struct c8obj* c8mpfr_copy(const struct c8obj* o)
{
  const struct c8mpfr* oo = to_const_c8mpfr(o);
  assert(oo);
  return (struct c8obj*)c8mpfr_create_mpfr(oo->value);
}

static int c8mpfr_int(const struct c8obj* o)
{
  const struct c8mpfr* oo = to_const_c8mpfr(o);
  assert(oo);
  return (int)mpfr_get_si(oo->value, rnd);
}

static void c8mpfr_str(const struct c8obj* o, struct c8buf* buf, int f)
{
  const struct c8mpfr* oo = to_const_c8mpfr(o);
  assert(oo);

  if (!mpfr_number_p(oo->value)) {
    if (mpfr_inf_p(oo->value)) {
      c8buf_append_str(buf, "Infinity");
    } else {
      c8buf_append_str(buf, "Not a number");
    }
    return;
  }

  if (dp == 0) {
    // Convert bit precision to decimal places
    dp = (unsigned int)(mpfr_get_default_prec() * 0.301); // log(2)
  }
  
  char* cs = 0;
  switch (f & C8_FMT_MASK_BASE) {
    case C8_FMT_BIN:
      mpfr_asprintf(&cs, "%-.*Rb", dp, oo->value);
      break;
    case C8_FMT_HEX:
      mpfr_asprintf(&cs, "%-.*Ra", dp, oo->value);
      break;
    case C8_FMT_SCI:
      mpfr_asprintf(&cs, "%-.Re", oo->value);
      break;
    case C8_FMT_FIX:
      mpfr_asprintf(&cs, "%-.Rf", oo->value);
      break;
    default:
      mpfr_asprintf(&cs, "%-.*Rg", dp, oo->value);
      break;
  }
  c8buf_append_str(buf, cs);
  mpfr_free_str(cs);
}

static struct c8obj* c8mpfr_binary_op(struct c8mpfr* oo, int op,
                                      struct c8mpfr* np)
{
  switch (op) {
    case C8_OP_ADD: {
      struct c8mpfr* nr = c8mpfr_create();
      mpfr_add(nr->value, oo->value, np->value, rnd);
      return (struct c8obj*)nr;
    }
    case C8_OP_SUBTRACT: {
      struct c8mpfr* nr = c8mpfr_create();
      mpfr_sub(nr->value, oo->value, np->value, rnd);
      return (struct c8obj*)nr;
    }
    case C8_OP_MULTIPLY: {
      struct c8mpfr* nr = c8mpfr_create();
      mpfr_mul(nr->value, oo->value, np->value, rnd);
      return (struct c8obj*)nr;
    }
    case C8_OP_DIVIDE: {
      struct c8mpfr* nr = c8mpfr_create();
      mpfr_div(nr->value, oo->value, np->value, rnd);
      return (struct c8obj*)nr;
    }
    case C8_OP_MODULUS: {
      struct c8mpfr* nr = c8mpfr_create();
      mpfr_fmod(nr->value, oo->value, np->value, rnd);
      return (struct c8obj*)nr;
    }
    case C8_OP_POWER: {
      struct c8mpfr* nr = c8mpfr_create();
      mpfr_pow(nr->value, oo->value, np->value, rnd);
      return (struct c8obj*)nr;
    }

    case C8_OP_EQUALITY:
      return (struct c8obj*)
        c8bool_create(mpfr_equal_p(oo->value, np->value));
    case C8_OP_INEQUALITY:
      return (struct c8obj*)
        c8bool_create(!mpfr_equal_p(oo->value, np->value));
    case C8_OP_GREATER:
      return (struct c8obj*)
        c8bool_create(mpfr_greater_p(oo->value, np->value));
    case C8_OP_LESS:
      return (struct c8obj*)
        c8bool_create(mpfr_less_p(oo->value, np->value));
    case C8_OP_GREATER_OR_EQUAL:
      return (struct c8obj*)
        c8bool_create(mpfr_greaterequal_p(oo->value, np->value));
    case C8_OP_LESS_OR_EQUAL:
      return (struct c8obj*)
        c8bool_create(mpfr_lessequal_p(oo->value, np->value));

    case C8_OP_ASSIGN: {
      mpfr_set(oo->value, np->value, rnd);
      return c8obj_ref((struct c8obj*)oo);
    }
    case C8_OP_ADD_ASSIGN: {
      mpfr_add(oo->value, oo->value, np->value, rnd);
      return c8obj_ref((struct c8obj*)oo);
    }
    case C8_OP_SUBTRACT_ASSIGN: {
      mpfr_sub(oo->value, oo->value, np->value, rnd);
      return c8obj_ref((struct c8obj*)oo);
    }
    case C8_OP_MULTIPLY_ASSIGN: {
      mpfr_mul(oo->value, oo->value, np->value, rnd);
      return c8obj_ref((struct c8obj*)oo);
    }
    case C8_OP_DIVIDE_ASSIGN: {
      mpfr_div(oo->value, oo->value, np->value, rnd);
      return c8obj_ref((struct c8obj*)oo);
    }
  }
  return 0;
}

static struct c8obj* c8mpfr_lookup(struct c8obj* o, const char* name)
{
  if (strcmp("abs", name)==0) return (struct c8obj*)c8func_create_method(c8mpfr_abs, o);
  if (strcmp("ceil", name)==0) return (struct c8obj*)c8func_create_method(c8mpfr_ceil, o);
  if (strcmp("floor", name)==0) return (struct c8obj*)c8func_create_method(c8mpfr_floor, o);
  if (strcmp("trunc", name)==0) return (struct c8obj*)c8func_create_method(c8mpfr_trunc, o);
  if (strcmp("log", name)==0) return (struct c8obj*)c8func_create_method(c8mpfr_log, o);
  if (strcmp("exp", name)==0) return (struct c8obj*)c8func_create_method(c8mpfr_exp, o);
  if (strcmp("sqrt", name)==0) return (struct c8obj*)c8func_create_method(c8mpfr_sqrt, o);
  if (strcmp("cos", name)==0) return (struct c8obj*)c8func_create_method(c8mpfr_cos, o);
  if (strcmp("sin", name)==0) return (struct c8obj*)c8func_create_method(c8mpfr_sin, o);
  if (strcmp("tan", name)==0) return (struct c8obj*)c8func_create_method(c8mpfr_tan, o);
  if (strcmp("acos", name)==0) return (struct c8obj*)c8func_create_method(c8mpfr_acos, o);
  if (strcmp("asin", name)==0) return (struct c8obj*)c8func_create_method(c8mpfr_asin, o);
  if (strcmp("atan", name)==0) return (struct c8obj*)c8func_create_method(c8mpfr_atan, o);
  if (strcmp("atan2", name)==0) return (struct c8obj*)c8func_create_method(c8mpfr_atan2, o);
  if (strcmp("cosh", name)==0) return (struct c8obj*)c8func_create_method(c8mpfr_cosh, o);
  if (strcmp("sinh", name)==0) return (struct c8obj*)c8func_create_method(c8mpfr_sinh, o);
  if (strcmp("tanh", name)==0) return (struct c8obj*)c8func_create_method(c8mpfr_tanh, o);
  if (strcmp("mean", name)==0) return (struct c8obj*)c8func_create_method(c8mpfr_mean, o);
  return 0;
}

static struct c8obj* c8mpfr_op(struct c8obj* o, int op, struct c8obj* p)
{
  struct c8mpfr* oo = to_c8mpfr(o);
  assert(oo);

  switch (op) {
    case C8_OP_LOOKUP: {
      struct c8buf nb; c8buf_init(&nb);
      c8obj_str(p, &nb, 0); 
      struct c8obj* ret = c8mpfr_lookup(o, c8buf_str(&nb));
      c8buf_clear(&nb);
      return ret;
    }
    case C8_OP_POSITIVE: {
      struct c8mpfr* nr = c8mpfr_create();
      mpfr_set(nr->value, oo->value, rnd);
      return (struct c8obj*)nr;
    }
    case C8_OP_NEGATIVE: {
      struct c8mpfr* nr = c8mpfr_create();
      mpfr_neg(nr->value, oo->value, rnd);
      return (struct c8obj*)nr;
    }
    case C8_OP_PRE_INC: {
      mpfr_add_ui(oo->value, oo->value, 1, rnd);
      c8obj_ref(o);
      return o;
    }
    case C8_OP_PRE_DEC: {
      mpfr_sub_ui(oo->value, oo->value, 1, rnd);
      c8obj_ref(o);
      return o;
    }
    case C8_OP_FACTORIAL: {
      if (!mpfr_integer_p(oo->value)) {
        return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
      }
      struct c8mpfr* nr = c8mpfr_create();
      mpfr_fac_ui(nr->value, mpfr_get_ui(oo->value, rnd), rnd);
      return (struct c8obj*)nr;
    }
    case C8_OP_POST_INC: {
      struct c8mpfr* nr = c8mpfr_create_mpfr(oo->value);
      mpfr_add_ui(oo->value, oo->value, 1, rnd);
      return (struct c8obj*)nr;
    }
    case C8_OP_POST_DEC: {
      struct c8mpfr* nr = c8mpfr_create_mpfr(oo->value);
      mpfr_sub_ui(oo->value, oo->value, 1, rnd);
      return (struct c8obj*)nr;
    }
  }

  struct c8mpfr* np = to_c8mpfr(p);
  if (np) return c8mpfr_binary_op(oo, op, np);

  // Convert p to mpfr and perform the op
  if (p) {
    struct c8buf buf; c8buf_init(&buf);
    c8obj_str(p, &buf, 0);
    struct c8mpfr* fo = c8mpfr_create_str(c8buf_str(&buf));
    c8buf_clear(&buf);
    struct c8obj* r = c8mpfr_binary_op(oo, op, fo);
    c8obj_unref((struct c8obj*)fo);
    return r;
  }

  return 0;
}

static const struct c8obj_imp c8mpfr_imp = {
  c8mpfr_destroy,
  c8mpfr_copy,
  c8mpfr_int,
  c8mpfr_str,
  c8mpfr_op
};

const struct c8mpfr* to_const_c8mpfr(const struct c8obj* o)
{
  const struct c8num* on = to_const_c8num(o);
  return (on && on->imp && on->imp == &c8mpfr_imp) ?
    (const struct c8mpfr*)o : 0;
}

struct c8mpfr* to_c8mpfr(struct c8obj* o)
{
  return (struct c8mpfr*)to_const_c8mpfr(o);
}

struct c8mpfr* c8mpfr_create()
{
  struct c8mpfr* oo = (struct c8mpfr*)malloc(sizeof(struct c8mpfr));
  assert(oo);
  c8num_init(&oo->base, &c8mpfr_imp);
  mpfr_init(oo->value);
  return oo;
}

struct c8mpfr* c8mpfr_create_int(int value)
{
  struct c8mpfr* oo = c8mpfr_create();
  mpfr_set_si(oo->value, value, rnd);
  return oo;
}

struct c8mpfr* c8mpfr_create_double(long double value)
{
  struct c8mpfr* oo = c8mpfr_create();
  mpfr_set_ld(oo->value, value, rnd);
  return oo;
}

struct c8mpfr* c8mpfr_create_str(const char* str)
{
  struct c8mpfr* oo = c8mpfr_create();
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
  mpfr_set_str(oo->value, str, base, rnd);
  return oo;
}

struct c8mpfr* c8mpfr_create_c8obj(const struct c8obj* obj)
{
  assert(obj);
  struct c8buf buf; c8buf_init(&buf);
  c8obj_str(obj, &buf, 0);
  struct c8mpfr* oo = c8mpfr_create_str(c8buf_str(&buf));
  c8buf_clear(&buf);
  return oo;
}

struct c8mpfr* c8mpfr_create_mpfr(const mpfr_t value)
{
  struct c8mpfr* oo = c8mpfr_create();
  mpfr_set(oo->value, value, rnd);
  return oo;
}

static struct c8num* c8mpfr_real_create(const char* str)
{
  return (struct c8num*)c8mpfr_create_str(str);
}

void c8mpfr_init_ctx(struct c8ctx* ctx)
{
  //  mpfr_set_default_prec(256);
  c8num_register_real_create(c8mpfr_real_create);

  struct c8mpfr* c = c8mpfr_create();
  mpfr_const_pi(c->value, rnd);
  c8ctx_add(ctx, "PI", (struct c8obj*)c);
  
  c = c8mpfr_create_int(1);
  mpfr_exp(c->value, c->value, rnd);
  c8ctx_add(ctx, "e", (struct c8obj*)c);
}

static struct c8obj* c8mpfr_single_arg(struct c8list* args)
{
  if (c8list_size(args) != 1)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  struct c8obj* a = c8list_at(args, 0);
  if (!a)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  struct c8mpfr* na = to_c8mpfr(a);
  if (na) return a;
  na = c8mpfr_create_c8obj(a);
  c8obj_unref(a);
  return (struct c8obj*)na;
}

#define C8MPFR_SINGLE_ARG_FN(name, code)            \
  struct c8obj* c8mpfr_##name (struct c8list* args) \
  {                                                 \
    struct c8obj* a = c8mpfr_single_arg(args);      \
    struct c8mpfr* na = to_c8mpfr(a);               \
    if (!na) return a;                              \
    struct c8mpfr* nr = c8mpfr_create();            \
    code;                                           \
    c8obj_unref(a);                                 \
    return (struct c8obj*)nr;                       \
  }

C8MPFR_SINGLE_ARG_FN(abs, mpfr_abs(nr->value, na->value, rnd))
C8MPFR_SINGLE_ARG_FN(ceil, mpfr_ceil(nr->value, na->value))
C8MPFR_SINGLE_ARG_FN(floor, mpfr_floor(nr->value, na->value))
C8MPFR_SINGLE_ARG_FN(trunc, mpfr_trunc(nr->value, na->value))
C8MPFR_SINGLE_ARG_FN(log, mpfr_log(nr->value, na->value, rnd))
C8MPFR_SINGLE_ARG_FN(exp, mpfr_exp(nr->value, na->value, rnd))
C8MPFR_SINGLE_ARG_FN(sqrt, mpfr_sqrt(nr->value, na->value, rnd))
C8MPFR_SINGLE_ARG_FN(cos, mpfr_cos(nr->value, na->value, rnd))
C8MPFR_SINGLE_ARG_FN(sin, mpfr_sin(nr->value, na->value, rnd))
C8MPFR_SINGLE_ARG_FN(tan, mpfr_tan(nr->value, na->value, rnd))
C8MPFR_SINGLE_ARG_FN(acos, mpfr_acos(nr->value, na->value, rnd))
C8MPFR_SINGLE_ARG_FN(asin, mpfr_asin(nr->value, na->value, rnd))
C8MPFR_SINGLE_ARG_FN(atan, mpfr_atan(nr->value, na->value, rnd))
C8MPFR_SINGLE_ARG_FN(cosh, mpfr_cosh(nr->value, na->value, rnd))
C8MPFR_SINGLE_ARG_FN(sinh, mpfr_sinh(nr->value, na->value, rnd))
C8MPFR_SINGLE_ARG_FN(tanh, mpfr_tanh(nr->value, na->value, rnd))

struct c8obj* c8mpfr_atan2(struct c8list* args)
{
  if (c8list_size(args) != 2)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  struct c8obj* y = c8list_at(args, 0);
  struct c8obj* x = c8list_at(args, 1);
  if (!y || !x) {
    c8obj_unref(y);
    c8obj_unref(x);
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  }
  struct c8mpfr* ny = to_c8mpfr(y);
  if (!ny) {
    ny = c8mpfr_create_c8obj(y);
    c8obj_unref(y);
  }
  struct c8mpfr* nx = to_c8mpfr(x);
  if (!nx) {
    nx = c8mpfr_create_c8obj(x);
    c8obj_unref(x);
  }
  struct c8mpfr* nr = c8mpfr_create();
  mpfr_atan2(nr->value, ny->value, nx->value, rnd);
  c8obj_unref((struct c8obj*)ny);
  c8obj_unref((struct c8obj*)nx);
  return (struct c8obj*)nr;
}

struct c8obj* c8mpfr_mean(struct c8list* args)
{
  int n = c8list_size(args);
  if (n == 0)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  struct c8mpfr* nr = c8mpfr_create_int(0);

  for (int i=0; i<n; ++i) {
    struct c8obj* a = c8list_at(args, i);
    if (!a) continue;
    struct c8mpfr* na = to_c8mpfr(a);
    if (!na) {
      na = c8mpfr_create_c8obj(a);
      c8obj_unref(a);
      a = (struct c8obj*)na;
    }
    mpfr_add(nr->value, nr->value, na->value, rnd);
    c8obj_unref(a);
  }
  mpfr_div_ui(nr->value, nr->value, n, rnd);
  return (struct c8obj*)nr;
}
