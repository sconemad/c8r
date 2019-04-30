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
  struct c8flow* fo = to_c8flow(o);
  assert(fo);
  c8buf_clear(&fo->expr);
  free(fo);
}

static int c8flow_parse(struct c8stmt* o, struct c8script* script,
                        const char* token)
{
  c8debug(C8_DEBUG_INFO, "c8flow_parse: %s", token);
  struct c8flow* fo = to_c8flow(o);
  assert(fo);

  switch (++fo->seq) {
  case 1:
    c8buf_clear(&fo->expr);
    c8buf_init_str(&fo->expr, token);
    break;

  default:
    return C8_PARSE_POP;
  }

  return C8_PARSE_CONTINUE;
}

static int c8flow_parse_mode(struct c8stmt* o)
{
  struct c8flow* fo = to_c8flow(o);
  assert(fo);
  return C8_PARSEMODE_STATEMENT;
}

static int c8flow_run(struct c8stmt* o, struct c8script* script)
{
  struct c8flow* fo = to_c8flow(o);
  assert(fo);

  if (fo->flow == C8_RUN_RETURN) {
    struct c8obj* retval = 0;
    if (c8buf_len(&fo->expr)) {
      struct c8eval* eval = c8script_eval(script);
      retval = c8eval_expr(eval, c8buf_str(&fo->expr));
    }
    c8script_give_ret(script, retval);
  }

  return fo->flow;
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
  struct c8flow* fo = malloc(sizeof(struct c8flow));
  fo->base.imp = &c8flow_imp;
  fo->base.parent = 0;
  fo->seq = 0;
  fo->flow = flow;
  c8buf_init(&fo->expr);
  return fo;
}
