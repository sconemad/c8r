/** c8decl - declaration statement
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

#include "c8decl.h"
#include "c8stmtimp.h"
#include "c8stmt.h"
#include "c8script.h"
#include "c8error.h"
#include "c8bool.h"
#include "c8eval.h"
#include "c8vec.h"
#include "c8obj.h"
#include "c8buf.h"
#include "c8group.h"
#include "c8ctx.h"
#include "c8debug.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

struct c8decl {
  struct c8stmt base;
  int seq;
  struct c8buf name;
  struct c8buf initialiser;
};

static void c8decl_destroy(struct c8stmt* o)
{
  struct c8decl* oo = to_c8decl(o);
  assert(oo);
  c8buf_clear(&oo->name);
  c8buf_clear(&oo->initialiser);
  free(oo);
}

static int c8decl_parse(struct c8stmt* o, struct c8script* script,
                        const char* token)
{
  c8debug(C8_DEBUG_INFO, "c8decl_parse: %s", token);
  struct c8decl* oo = to_c8decl(o);
  assert(oo);

  switch (++oo->seq) {
  case 1:
    c8buf_clear(&oo->name);
    c8buf_init_str(&oo->name, token);
    break;

  case 2:
    c8buf_clear(&oo->initialiser);
    c8buf_init_str(&oo->initialiser, &token[1]);
    break;

  default:
    return C8_PARSE_POP;
  }

  return C8_PARSE_CONTINUE;
}

static int c8decl_parse_mode(struct c8stmt* o)
{
  struct c8decl* oo = to_c8decl(o);
  assert(oo);
  return (oo->seq == 0 ? C8_PARSEMODE_NAME : C8_PARSEMODE_STATEMENT);
}

static int c8decl_run(struct c8stmt* o, struct c8script* script)
{
  struct c8decl* oo = to_c8decl(o);
  assert(oo);
  struct c8group* group = find_parent_group(o);
  if (!group) return C8_RUN_ERROR;
  
  struct c8eval* eval = c8script_eval(script);
  struct c8obj* init = c8eval_expr(eval, c8buf_str(&oo->initialiser));
  struct c8ctx* ctx = c8group_ctx(group);
  c8ctx_add(ctx, c8buf_str(&oo->name), init);

  return C8_RUN_NORMAL;
}

static const struct c8stmt_imp c8decl_imp = {
  c8decl_destroy,
  c8decl_parse,
  c8decl_parse_mode,
  c8decl_run,
};

struct c8decl* to_c8decl(struct c8stmt* o)
{
  return (o && o->imp && o->imp == &c8decl_imp) ?
    (struct c8decl*)o : 0;
}

struct c8decl* c8decl_create()
{
  struct c8decl* oo = malloc(sizeof(struct c8decl));
  oo->base.imp = &c8decl_imp;
  oo->base.parent = 0;
  oo->seq = 0;
  c8buf_init(&oo->name);
  c8buf_init(&oo->initialiser);
  return oo;
}
