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
#include "c8debug.h"

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
  struct c8cond* oo = to_c8cond(o);
  assert(oo);
  c8buf_clear(&oo->condition);
  c8stmt_destroy(oo->true_stmt);
  c8stmt_destroy(oo->false_stmt);
  free(oo);
}

static int c8cond_parse(struct c8stmt* o, struct c8script* script,
                        const char* token)
{
  c8debug(C8_DEBUG_INFO, "c8cond_parse: %s", token);
  struct c8cond* oo = to_c8cond(o);
  assert(oo);

  switch (++oo->seq) {
  case 1:
    c8buf_clear(&oo->condition);
    c8buf_init_str(&oo->condition, token);
    break;

  case 2:
    if (strcmp(token, "else") == 0) {
      // Empty true statement
      ++oo->seq;
      break;
    }
    oo->true_stmt = c8script_parse_token(script, token);
    if (oo->true_stmt) c8stmt_set_parent(oo->true_stmt, o);
    break;

  case 3:
    if (strcmp(token, "else") != 0) {
      return C8_PARSE_POP;
    }
    break;

  case 4:
    oo->false_stmt = c8script_parse_token(script, token);
    if (oo->false_stmt) c8stmt_set_parent(oo->true_stmt, o);
    break;

  default:
    return C8_PARSE_POP;
  }

  return C8_PARSE_CONTINUE;
}

static int c8cond_parse_mode(struct c8stmt* o)
{
  struct c8cond* oo = to_c8cond(o);
  assert(oo);
  return (oo->seq == 0 ? C8_PARSEMODE_BRACKETED : C8_PARSEMODE_STATEMENT);
}

static int c8cond_run(struct c8stmt* o, struct c8script* script)
{
  struct c8cond* oo = to_c8cond(o);
  assert(oo);
  int ret = C8_RUN_NORMAL;
  struct c8eval* eval = c8script_eval(script);
  int cr = c8eval_cond(eval, c8buf_str(&oo->condition));
  if (cr < 0) return C8_RUN_ERROR;
  if (cr) {
    if (oo->true_stmt) ret = c8stmt_run(oo->true_stmt, script);
  } else {
    if (oo->false_stmt) ret = c8stmt_run(oo->false_stmt, script);
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
  struct c8cond* oo = malloc(sizeof(struct c8cond));
  oo->base.imp = &c8cond_imp;
  oo->base.parent = 0;
  oo->seq = 0;
  c8buf_init(&oo->condition);
  oo->true_stmt = 0;
  oo->false_stmt = 0;
  return oo;
}
