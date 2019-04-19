/** c8loop - conditional statement
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

#include "c8loop.h"
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

struct c8loop {
  struct c8stmt base;
  int seq;
  struct c8buf condition;
  struct c8stmt* body;
};

static void c8loop_destroy(struct c8stmt* o)
{
  struct c8loop* co = to_c8loop(o);
  assert(co);
  c8buf_clear(&co->condition);
  c8stmt_destroy(co->body);
  free(co);
}

static int c8loop_parse(struct c8stmt* o, struct c8script* script,
                        const char* token)
{
  printf("c8loop_parse: %s\n", token);
  struct c8loop* lo = to_c8loop(o);
  assert(lo);

  switch (++lo->seq) {
  case 1:
    c8buf_clear(&lo->condition);
    c8buf_init_str(&lo->condition, token);
    break;

  case 2:
    lo->body = c8script_parse_token(script, token);
    if (lo->body) c8stmt_set_parent(lo->body, o);
    break;

  default:
    return C8_PARSERESULT_POP;
  }

  return C8_PARSERESULT_CONTINUE;
}

static int c8loop_parse_mode(struct c8stmt* o)
{
  struct c8loop* lo = to_c8loop(o);
  assert(lo);
  return (lo->seq == 0 ? C8_PARSEMODE_BRACKETED : C8_PARSEMODE_STATEMENT);
}

static struct c8obj* c8loop_run(struct c8stmt* o,
                                struct c8script* script, int* flow)
{
  struct c8loop* lo = to_c8loop(o);
  assert(lo);
  struct c8obj* ret = 0;
  struct c8eval* eval = c8script_eval(script);
  struct c8bool* br = 0;

  while (1) {
    c8obj_unref(ret);
    ret = c8eval_expr(eval, c8buf_str(&lo->condition));
    if (!ret) return (struct c8obj*) c8error_create(C8_ERROR_ARGUMENT);
    if (to_c8error(ret)) return ret;
    br = to_c8bool(ret);
    if (!br) return (struct c8obj*) c8error_create(C8_ERROR_ARGUMENT);
  
    if (!c8bool_value(br)) {
      // Condition is false, break out of the loop
      break;
    }
    
    if (lo->body) {
      c8obj_unref(ret);
      ret = c8stmt_run(lo->body, script, flow);
      if (ret && to_c8error(ret)) return ret;
      if (*flow == C8_FLOW_RETURN) break;
      if (*flow == C8_FLOW_LAST) {
        *flow = C8_FLOW_NORMAL;
        break;
      }
      if (*flow == C8_FLOW_NEXT) {
        *flow = C8_FLOW_NORMAL;
      }
    }
  }
  return ret;
}

static const struct c8stmt_imp c8loop_imp = {
  c8loop_destroy,
  c8loop_parse,
  c8loop_parse_mode,
  c8loop_run,
};

struct c8loop* to_c8loop(struct c8stmt* o)
{
  return (o && o->imp && o->imp == &c8loop_imp) ?
    (struct c8loop*)o : 0;
}

struct c8loop* c8loop_create()
{
  struct c8loop* lo = malloc(sizeof(struct c8loop));
  lo->base.imp = &c8loop_imp;
  lo->base.parent = 0;
  lo->seq = 0;
  c8buf_init(&lo->condition);
  lo->body = 0;
  return lo;
}
