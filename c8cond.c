/** c8cond - conditional statement
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

#include "c8cond.h"
#include "c8stmtimp.h"
#include "c8stmt.h"
#include "c8script.h"
#include "c8error.h"
#include "c8bool.h"
#include "c8eval.h"
#include "c8vec.h"
#include "c8obj.h"
#include "c8buf.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

struct c8cond {
  struct c8stmt base;
  int seq;
  struct c8buf condition;
  struct c8stmt* true_stmt;
  struct c8stmt* false_stmt;
};

static void c8cond_destroy(struct c8stmt* o)
{
  struct c8cond* co = to_c8cond(o);
  assert(co);
  c8buf_clear(&co->condition);
  c8stmt_destroy(co->true_stmt);
  c8stmt_destroy(co->false_stmt);
  free(co);
}

static int c8cond_parse(struct c8stmt* o, struct c8script* script,
                        const char* token)
{
  printf("c8cond_parse: %s\n", token);
  struct c8cond* co = to_c8cond(o);
  assert(co);

  switch (++co->seq) {
  case 1:
    c8buf_clear(&co->condition);
    c8buf_init_str(&co->condition, token);
    break;

  case 2:
    if (strcmp(token, "else") == 0) {
      // Empty true statement
      ++co->seq;
      break;
    }
    co->true_stmt = c8script_parse_token(script, token);
    if (co->true_stmt) c8stmt_set_parent(co->true_stmt, o);
    break;

  case 3:
    if (strcmp(token, "else") != 0) {
      return C8_PARSERESULT_POP;
    }
    break;

  case 4:
    co->false_stmt = c8script_parse_token(script, token);
    if (co->false_stmt) c8stmt_set_parent(co->true_stmt, o);
    break;

  default:
    return C8_PARSERESULT_POP;
  }

  return C8_PARSERESULT_CONTINUE;
}

static int c8cond_parse_mode(struct c8stmt* o)
{
  struct c8cond* co = to_c8cond(o);
  assert(co);
  return (co->seq == 0 ? C8_PARSEMODE_BRACKETED : C8_PARSEMODE_STATEMENT);
}

static struct c8obj* c8cond_run(struct c8stmt* o,
                                struct c8script* script, int* flow)
{
  struct c8cond* co = to_c8cond(o);
  assert(co);
  struct c8obj* ret = 0;
  struct c8eval* eval = c8script_eval(script);
  struct c8bool* br = 0;

  ret = c8eval_expr(eval, c8buf_str(&co->condition));
  if (!ret) return (struct c8obj*) c8error_create(C8_ERROR_ARGUMENT);
  if (to_c8error(ret)) return ret;
  br = to_c8bool(ret);
  if (!br) return (struct c8obj*) c8error_create(C8_ERROR_ARGUMENT);
  
  if (c8bool_value(br)) {
    c8obj_unref(ret); ret = 0;
    if (co->true_stmt) ret = c8stmt_run(co->true_stmt, script, flow);
  } else {
    c8obj_unref(ret); ret = 0;
    if (co->false_stmt) ret = c8stmt_run(co->false_stmt, script, flow);
  }
  return ret;
}

static const struct c8stmt_imp c8cond_imp = {
  c8cond_destroy,
  c8cond_parse,
  c8cond_parse_mode,
  c8cond_run,
};

struct c8cond* to_c8cond(struct c8stmt* o)
{
  return (o && o->imp && o->imp == &c8cond_imp) ?
    (struct c8cond*)o : 0;
}

struct c8cond* c8cond_create()
{
  struct c8cond* co = malloc(sizeof(struct c8cond));
  co->base.imp = &c8cond_imp;
  co->base.parent = 0;
  co->seq = 0;
  c8buf_init(&co->condition);
  co->true_stmt = 0;
  co->false_stmt = 0;
  return co;
}
