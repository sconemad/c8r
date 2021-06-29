/** c8sub - subroutine statement and object
 *
 * Copyright (c) 2019 Andrew Wedgbury <wedge@sconemad.com>
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

#include "c8sub.h"
#include "c8stmtimp.h"
#include "c8stmt.h"
#include "c8objimp.h"
#include "c8script.h"
#include "c8error.h"
#include "c8bool.h"
#include "c8eval.h"
#include "c8vec.h"
#include "c8obj.h"
#include "c8buf.h"
#include "c8debug.h"
#include "c8group.h"
#include "c8ctx.h"
#include "c8ops.h"
#include "c8list.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

// c8subdef

struct c8subdef {
  struct c8stmt base;
  int seq;
  struct c8buf name;
  struct c8vec args;
  struct c8stmt* body;
};

static void c8subdef_destroy(struct c8stmt* o)
{
  struct c8subdef* oo = to_c8subdef(o);
  assert(oo);
  c8buf_clear(&oo->name); 
  const int na = c8vec_size(&oo->args);
  for (int i=0; i<na; ++i) {
    const char* s = (const char*)c8vec_at(&oo->args, i);
    free((void*)s);
  }
  c8vec_clear(&oo->args);
  c8stmt_destroy(oo->body);
  free(oo);
}

static int parse_arglist(struct c8subdef* oo, const char* token)
{
  char* orig = strdup(token);
  char* tok = orig;
  const char* arg;
  while ((arg = strsep(&tok, " ,")) != 0) {
    if (arg[0]) c8vec_push_back(&oo->args, strdup(arg));
  }
  free(orig);
  return C8_PARSE_CONTINUE;
}

static int c8subdef_parse(struct c8stmt* o, struct c8script* script,
                          const char* token)
{
  c8debug(C8_DEBUG_INFO, "c8subdef_parse: %s", token);
  struct c8subdef* oo = to_c8subdef(o);
  assert(oo);

  switch (++oo->seq) {
  case 1:
    c8buf_clear(&oo->name);
    c8buf_init_str(&oo->name, token);
    break;

  case 2:
    return parse_arglist(oo, token);

  case 3:
    oo->body = c8script_parse_token(script, token);
    if (oo->body) c8stmt_set_parent(oo->body, o);
    break;

  default:
    return C8_PARSE_POP;
  }

  return C8_PARSE_CONTINUE;
}

static int c8subdef_parse_mode(struct c8stmt* o)
{
  struct c8subdef* oo = to_c8subdef(o);
  assert(oo);
  switch (oo->seq) {
  case 0: return C8_PARSEMODE_NAME;
  case 1: return C8_PARSEMODE_BRACKETED;
  }
  return C8_PARSEMODE_STATEMENT;
}

static int c8subdef_run(struct c8stmt* o, struct c8script* script)
{
  struct c8subdef* oo = to_c8subdef(o);
  assert(oo);
  struct c8eval* eval = c8script_eval(script);
  struct c8ctx* ctx = c8eval_global(eval);
  struct c8sub* sub = c8sub_create(oo, script);
  c8ctx_add(ctx, c8buf_str(&oo->name), (struct c8obj*)sub);

  return C8_RUN_NORMAL;
}

static const struct c8stmt_imp c8subdef_imp = {
  c8subdef_destroy,
  c8subdef_parse,
  c8subdef_parse_mode,
  c8subdef_run,
};

struct c8subdef* to_c8subdef(struct c8stmt* o)
{
  return (o && o->imp && o->imp == &c8subdef_imp) ?
    (struct c8subdef*)o : 0;
}

struct c8subdef* c8subdef_create(int type)
{
  struct c8subdef* oo = malloc(sizeof(struct c8subdef));
  oo->base.imp = &c8subdef_imp;
  oo->base.parent = 0;
  oo->seq = 0;
  c8buf_init(&oo->name);
  c8vec_init(&oo->args);
  oo->body = 0;
  return oo;
}

// c8sub

struct c8sub {
  struct c8obj base;
  struct c8subdef* def;
  struct c8script* script;
};

static void c8sub_destroy(struct c8obj* o)
{
  struct c8sub* oo = to_c8sub(o);
  assert(oo);
  free(oo);
}

static struct c8obj* c8sub_copy(const struct c8obj* o)
{
  const struct c8sub* oo = to_const_c8sub(o);
  assert(oo);
  return (struct c8obj*)c8sub_create(oo->def, oo->script);
}

static int c8sub_int(const struct c8obj* o)
{
  const struct c8sub* oo = to_const_c8sub(o);
  assert(oo);
  return (oo->def != 0);
}

static void c8sub_str(const struct c8obj* o, struct c8buf* buf, int f)
{
  const struct c8sub* oo = to_const_c8sub(o);
  assert(oo);
  c8buf_append_str(buf, "subroutine:");
  c8buf_append_buf(buf, &oo->def->name);
}

static struct c8obj* c8sub_op(struct c8obj* o, int op, struct c8obj* p)
{
  struct c8sub* oo = to_c8sub(o);
  assert(oo);
  if (C8_OP_LIST == op) {
    struct c8list* args = to_c8list(p);
    if (args) return c8sub_call(oo, args);
  }
  return 0;
}

static const struct c8obj_imp c8sub_imp = {
  c8sub_destroy,
  c8sub_copy,
  c8sub_int,
  c8sub_str,
  c8sub_op
};

const struct c8sub* to_const_c8sub(const struct c8obj* o)
{
  return (o && o->imp && o->imp == &c8sub_imp) ?
    (const struct c8sub*)o : 0;
}

struct c8sub* to_c8sub(struct c8obj* o)
{
  return (struct c8sub*)to_const_c8sub(o);
}

struct c8sub* c8sub_create(struct c8subdef* def, struct c8script* script)
{
  assert(def);
  struct c8sub* oo = malloc(sizeof(struct c8sub));
  assert(oo);
  oo->base.refs = 1;
  oo->base.imp = &c8sub_imp;
  oo->def = def;
  oo->script = script;
  return oo;
}

struct c8obj* c8sub_call(struct c8sub* oo, struct c8list* args)
{
  assert(oo);
  struct c8group* group = to_c8group(oo->def->body);
  if (group) {
    struct c8ctx* ctx = c8group_ctx(group);
    const int na = c8vec_size(&oo->def->args);
    if (na != c8list_size(args)) 
      return (struct c8obj*)c8error_create(C8_ERROR_ARGUMENT);
    for (int i=0; i<na; ++i) {
      const char* name = (const char*)c8vec_at(&oo->def->args, i);
      struct c8obj* value = c8list_at(args, i);
      c8ctx_add(ctx, name, value);
    }
  }

  struct c8ctx* gctx = c8eval_global(c8script_eval(oo->script));
  struct c8script* subscr = c8script_create(gctx);
  c8stmt_run(oo->def->body, subscr);
  struct c8obj* r = c8script_take_ret(subscr);
  c8script_destroy(subscr);
  return r;
}
