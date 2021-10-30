/** c8mpc - complex numeric object using GNU mpc
 *
 * Copyright (c) 2021 Andrew Wedgbury <wedge@sconemad.com>
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

#include "c8mpc.h"
#include "c8mpfr.h"
#include "c8obj.h"
#include "c8objimp.h"
#include "c8ops.h"
#include "c8bool.h"
#include "c8error.h"
#include "c8func.h"
#include "c8ctx.h"
#include "c8list.h"
#include "c8buf.h"

#include <mpc.h>
#include <mpfr.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct c8mpc {
  struct c8obj base;
  mpc_t value;
};

static mpc_rnd_t rnd = MPC_RNDNN;
static mpfr_rnd_t mpfr_rnd = MPFR_RNDN;
static int dp = 0;

struct c8mpc* c8mpc_create_mpc(const mpc_t value);

static void c8mpc_destroy(struct c8obj* o)
{
  struct c8mpc* oo = to_c8mpc(o);
  assert(oo);
  mpc_clear(oo->value);
  free(oo);
}

static struct c8obj* c8mpc_copy(const struct c8obj* o)
{
  const struct c8mpc* oo = to_const_c8mpc(o);
  assert(oo);
  return (struct c8obj*)c8mpc_create_mpc(oo->value);
}

static int c8mpc_int(const struct c8obj* o)
{
  const struct c8mpc* oo = to_const_c8mpc(o);
  assert(oo);
  mpfr_t a; mpfr_init(a);
  mpc_abs(a, oo->value, mpfr_rnd);
  int ret = (int)mpfr_get_si(a, mpfr_rnd);
  mpfr_clear(a);
  return ret;
}

static void c8mpc_str(const struct c8obj* o, struct c8buf* buf, int f)
{
  const struct c8mpc* oo = to_const_c8mpc(o);
  assert(oo);

  if (dp == 0) {
    // Convert bit precision to decimal places
    dp = (unsigned int)(53 * 0.301); // log(2)
  }

  int base = 10;
  char* cs = 0;
  switch (f & C8_FMT_MASK_BASE) {
    case C8_FMT_BIN:
      base = 2;
      break;
    case C8_FMT_HEX:
      base = 16;
      break;
    case C8_FMT_SCI:
      break;
    case C8_FMT_FIX:
      break;
    default:
      break;
  }

  cs = mpc_get_str(base, dp, oo->value, rnd);
  c8buf_append_str(buf, cs);
  mpc_free_str(cs);
}

static struct c8obj* c8mpc_binary_op(struct c8mpc* oo, int op,
                                      struct c8mpc* np)
{
  switch (op) {
    case C8_OP_ADD: {
      struct c8mpc* nr = c8mpc_create();
      mpc_add(nr->value, oo->value, np->value, rnd);
      return (struct c8obj*)nr;
    }
    case C8_OP_SUBTRACT: {
      struct c8mpc* nr = c8mpc_create();
      mpc_sub(nr->value, oo->value, np->value, rnd);
      return (struct c8obj*)nr;
    }
    case C8_OP_MULTIPLY: {
      struct c8mpc* nr = c8mpc_create();
      mpc_mul(nr->value, oo->value, np->value, rnd);
      return (struct c8obj*)nr;
    }
    case C8_OP_DIVIDE: {
      struct c8mpc* nr = c8mpc_create();
      mpc_div(nr->value, oo->value, np->value, rnd);
      return (struct c8obj*)nr;
    }
    case C8_OP_POWER: {
      struct c8mpc* nr = c8mpc_create();
      mpc_pow(nr->value, oo->value, np->value, rnd);
      return (struct c8obj*)nr;
    }
    case C8_OP_EQUALITY:
      return (struct c8obj*)
        c8bool_create(mpc_cmp(oo->value, np->value) == 0);
    case C8_OP_INEQUALITY:
      return (struct c8obj*)
        c8bool_create(mpc_cmp(oo->value, np->value) != 0);
    case C8_OP_GREATER:
      return (struct c8obj*)
        c8bool_create(mpc_cmp_abs(oo->value, np->value) > 0);
    case C8_OP_LESS:
      return (struct c8obj*)
        c8bool_create(mpc_cmp_abs(oo->value, np->value) < 0);
    case C8_OP_GREATER_OR_EQUAL:
      return (struct c8obj*)
        c8bool_create(mpc_cmp_abs(oo->value, np->value) >= 0);
    case C8_OP_LESS_OR_EQUAL:
      return (struct c8obj*)
        c8bool_create(mpc_cmp_abs(oo->value, np->value) <= 0);
    case C8_OP_ASSIGN: {
      mpc_set(oo->value, np->value, rnd);
      return c8obj_ref((struct c8obj*)oo);
    }
    case C8_OP_ADD_ASSIGN: {
      mpc_add(oo->value, oo->value, np->value, rnd);
      return c8obj_ref((struct c8obj*)oo);
    }
    case C8_OP_SUBTRACT_ASSIGN: {
      mpc_sub(oo->value, oo->value, np->value, rnd);
      return c8obj_ref((struct c8obj*)oo);
    }
    case C8_OP_MULTIPLY_ASSIGN: {
      mpc_mul(oo->value, oo->value, np->value, rnd);
      return c8obj_ref((struct c8obj*)oo);
    }
    case C8_OP_DIVIDE_ASSIGN: {
      mpc_div(oo->value, oo->value, np->value, rnd);
      return c8obj_ref((struct c8obj*)oo);
    }
  }
  return 0;
}

static struct c8obj* c8mpc_lookup(struct c8obj* o, const char* name)
{
  if (strcmp("log", name)==0) return (struct c8obj*)c8func_create_method(c8mpc_log, o);
  if (strcmp("exp", name)==0) return (struct c8obj*)c8func_create_method(c8mpc_exp, o);
  if (strcmp("sqrt", name)==0) return (struct c8obj*)c8func_create_method(c8mpc_sqrt, o);
  if (strcmp("cos", name)==0) return (struct c8obj*)c8func_create_method(c8mpc_cos, o);
  if (strcmp("sin", name)==0) return (struct c8obj*)c8func_create_method(c8mpc_sin, o);
  if (strcmp("tan", name)==0) return (struct c8obj*)c8func_create_method(c8mpc_tan, o);
  if (strcmp("acos", name)==0) return (struct c8obj*)c8func_create_method(c8mpc_acos, o);
  if (strcmp("asin", name)==0) return (struct c8obj*)c8func_create_method(c8mpc_asin, o);
  if (strcmp("atan", name)==0) return (struct c8obj*)c8func_create_method(c8mpc_atan, o);
  if (strcmp("cosh", name)==0) return (struct c8obj*)c8func_create_method(c8mpc_cosh, o);
  if (strcmp("sinh", name)==0) return (struct c8obj*)c8func_create_method(c8mpc_sinh, o);
  if (strcmp("tanh", name)==0) return (struct c8obj*)c8func_create_method(c8mpc_tanh, o);
  return 0;
}

static struct c8obj* c8mpc_op(struct c8obj* o, int op, struct c8obj* p)
{
  struct c8mpc* oo = to_c8mpc(o);
  assert(oo);

  switch (op) {
    case C8_OP_LOOKUP: {
      struct c8buf nb; c8buf_init(&nb);
      c8obj_str(p, &nb, 0); 
      struct c8obj* ret = c8mpc_lookup(o, c8buf_str(&nb));
      c8buf_clear(&nb);
      return ret;
    }
    case C8_OP_POSITIVE: {
      struct c8mpc* nr = c8mpc_create();
      mpc_set(nr->value, oo->value, rnd);
      return (struct c8obj*)nr;
    }
    case C8_OP_NEGATIVE: {
      struct c8mpc* nr = c8mpc_create();
      mpc_neg(nr->value, oo->value, rnd);
      return (struct c8obj*)nr;
    }
    case C8_OP_PRE_INC: {
      mpc_add_ui(oo->value, oo->value, 1, rnd);
      c8obj_ref(o);
      return o;
    }
    case C8_OP_PRE_DEC: {
      mpc_sub_ui(oo->value, oo->value, 1, rnd);
      c8obj_ref(o);
      return o;
    }
    case C8_OP_POST_INC: {
      struct c8mpc* nr = c8mpc_create_mpc(oo->value);
      mpc_add_ui(oo->value, oo->value, 1, rnd);
      return (struct c8obj*)nr;
    }
    case C8_OP_POST_DEC: {
      struct c8mpc* nr = c8mpc_create_mpc(oo->value);
      mpc_sub_ui(oo->value, oo->value, 1, rnd);
      return (struct c8obj*)nr;
    }
  }

  struct c8mpc* np = to_c8mpc(p);
  if (np) return c8mpc_binary_op(oo, op, np);

  // Convert p to mpc and perform the op
  if (p) {
    struct c8buf buf; c8buf_init(&buf);
    c8obj_str(p, &buf, 0);
    struct c8mpc* fo = c8mpc_create_str(c8buf_str(&buf));
    c8buf_clear(&buf);
    struct c8obj* r = c8mpc_binary_op(oo, op, fo);
    c8obj_unref((struct c8obj*)fo);
    return r;
  }

  return 0;
}

static const struct c8obj_imp c8mpc_imp = {
  c8mpc_destroy,
  c8mpc_copy,
  c8mpc_int,
  c8mpc_str,
  c8mpc_op
};

const struct c8mpc* to_const_c8mpc(const struct c8obj* o)
{
  return (o && o->imp && o->imp == &c8mpc_imp) ?
    (const struct c8mpc*)o : 0;
}

struct c8mpc* to_c8mpc(struct c8obj* o)
{
  return (struct c8mpc*)to_const_c8mpc(o);
}

struct c8mpc* c8mpc_create()
{
  struct c8mpc* oo = (struct c8mpc*)malloc(sizeof(struct c8mpc));
  assert(oo);
  oo->base.refs = 1;
  oo->base.imp = &c8mpc_imp;
  mpc_init2(oo->value, 53); // XXX default prec?
  return oo;
}

struct c8mpc* c8mpc_create_int(int rvalue, int ivalue)
{
  struct c8mpc* oo = c8mpc_create();
  mpc_set_si_si(oo->value, rvalue, ivalue, rnd);
  return oo;
}

struct c8mpc* c8mpc_create_double(long double rvalue, long double ivalue)
{
  struct c8mpc* oo = c8mpc_create();
  mpc_set_ld_ld(oo->value, rvalue, ivalue, rnd);
  return oo;
}

struct c8mpc* c8mpc_create_str(const char* str)
{
  struct c8mpc* oo = c8mpc_create();
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
  mpc_set_str(oo->value, str, base, rnd);
  return oo;
}

struct c8mpc* c8mpc_create_c8obj(const struct c8obj* obj)
{
  assert(obj);
  struct c8buf buf; c8buf_init(&buf);
  c8obj_str(obj, &buf, 0);
  struct c8mpc* oo = c8mpc_create_str(c8buf_str(&buf));
  c8buf_clear(&buf);
  return oo;
}

struct c8mpc* c8mpc_create_mpc(const mpc_t value)
{
  struct c8mpc* oo = c8mpc_create();
  mpc_set(oo->value, value, rnd);
  return oo;
}

void c8mpc_init_ctx(struct c8ctx* ctx)
{
  //  mpc_set_default_prec(256);
  
  struct c8mpc* c = c8mpc_create_int(0, 1);
  c8ctx_add(ctx, "i", (struct c8obj*)c);
/*
  c8ctx_add(ctx, "log", (struct c8obj*)c8func_create(c8mpc_log));
  c8ctx_add(ctx, "exp", (struct c8obj*)c8func_create(c8mpc_exp));
  c8ctx_add(ctx, "sqrt", (struct c8obj*)c8func_create(c8mpc_sqrt));
  c8ctx_add(ctx, "cos", (struct c8obj*)c8func_create(c8mpc_cos));
  c8ctx_add(ctx, "sin", (struct c8obj*)c8func_create(c8mpc_sin));
  c8ctx_add(ctx, "tan", (struct c8obj*)c8func_create(c8mpc_tan));
  c8ctx_add(ctx, "acos", (struct c8obj*)c8func_create(c8mpc_acos));
  c8ctx_add(ctx, "asin", (struct c8obj*)c8func_create(c8mpc_asin));
  c8ctx_add(ctx, "atan", (struct c8obj*)c8func_create(c8mpc_atan));
  c8ctx_add(ctx, "cosh", (struct c8obj*)c8func_create(c8mpc_cosh));
  c8ctx_add(ctx, "sinh", (struct c8obj*)c8func_create(c8mpc_sinh));
  c8ctx_add(ctx, "tanh", (struct c8obj*)c8func_create(c8mpc_tanh));
*/
}

static struct c8obj* c8mpc_single_arg(struct c8list* args)
{
  if (c8list_size(args) != 1)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  struct c8obj* a = c8list_at(args, 0);
  if (!a)
    return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
  struct c8mpc* na = to_c8mpc(a);
  if (na) return a;
  na = c8mpc_create_c8obj(a);
  c8obj_unref(a);
  return (struct c8obj*)na;
}

#define C8MPC_SINGLE_ARG_FN(name, code)             \
  struct c8obj* c8mpc_##name (struct c8list* args)  \
  {                                                 \
    struct c8obj* a = c8mpc_single_arg(args);       \
    struct c8mpc* na = to_c8mpc(a);                 \
    if (!na) return a;                              \
    struct c8mpc* nr = c8mpc_create();              \
    code;                                           \
    c8obj_unref(a);                                 \
    return (struct c8obj*)nr;                       \
  }

C8MPC_SINGLE_ARG_FN(log, mpc_log(nr->value, na->value, rnd))
C8MPC_SINGLE_ARG_FN(exp, mpc_exp(nr->value, na->value, rnd))
C8MPC_SINGLE_ARG_FN(sqrt, mpc_sqrt(nr->value, na->value, rnd))
C8MPC_SINGLE_ARG_FN(cos, mpc_cos(nr->value, na->value, rnd))
C8MPC_SINGLE_ARG_FN(sin, mpc_sin(nr->value, na->value, rnd))
C8MPC_SINGLE_ARG_FN(tan, mpc_tan(nr->value, na->value, rnd))
C8MPC_SINGLE_ARG_FN(acos, mpc_acos(nr->value, na->value, rnd))
C8MPC_SINGLE_ARG_FN(asin, mpc_asin(nr->value, na->value, rnd))
C8MPC_SINGLE_ARG_FN(atan, mpc_atan(nr->value, na->value, rnd))
C8MPC_SINGLE_ARG_FN(cosh, mpc_cosh(nr->value, na->value, rnd))
C8MPC_SINGLE_ARG_FN(sinh, mpc_sinh(nr->value, na->value, rnd))
C8MPC_SINGLE_ARG_FN(tanh, mpc_tanh(nr->value, na->value, rnd))
