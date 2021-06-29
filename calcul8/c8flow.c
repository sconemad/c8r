/** c8flow - flow statement
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

#include "c8flow.h"
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

struct c8flow {
  struct c8stmt base;
  int seq;
  int flow;
  struct c8buf expr;
};

static void c8flow_destroy(struct c8stmt* o)
{
  struct c8flow* oo = to_c8flow(o);
  assert(oo);
  c8buf_clear(&oo->expr);
  free(oo);
}

static int c8flow_parse(struct c8stmt* o, struct c8script* script,
                        const char* token)
{
  c8debug(C8_DEBUG_INFO, "c8flow_parse: %s", token);
  struct c8flow* oo = to_c8flow(o);
  assert(oo);

  switch (++oo->seq) {
  case 1:
    c8buf_clear(&oo->expr);
    c8buf_init_str(&oo->expr, token);
    break;

  default:
    return C8_PARSE_POP;
  }

  return C8_PARSE_CONTINUE;
}

static int c8flow_parse_mode(struct c8stmt* o)
{
  struct c8flow* oo = to_c8flow(o);
  assert(oo);
  return C8_PARSEMODE_STATEMENT;
}

static int c8flow_run(struct c8stmt* o, struct c8script* script)
{
  struct c8flow* oo = to_c8flow(o);
  assert(oo);

  if (oo->flow == C8_RUN_RETURN) {
    struct c8obj* retval = 0;
    if (c8buf_len(&oo->expr)) {
      struct c8eval* eval = c8script_eval(script);
      retval = c8eval_expr(eval, c8buf_str(&oo->expr));
    }
    c8script_give_ret(script, retval);
  }

  return oo->flow;
}

static const struct c8stmt_imp c8flow_imp = {
  c8flow_destroy,
  c8flow_parse,
  c8flow_parse_mode,
  c8flow_run,
};

struct c8flow* to_c8flow(struct c8stmt* o)
{
  return (o && o->imp && o->imp == &c8flow_imp) ?
    (struct c8flow*)o : 0;
}

struct c8flow* c8flow_create(int flow)
{
  struct c8flow* oo = malloc(sizeof(struct c8flow));
  oo->base.imp = &c8flow_imp;
  oo->base.parent = 0;
  oo->seq = 0;
  oo->flow = flow;
  c8buf_init(&oo->expr);
  return oo;
}
