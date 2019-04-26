/** c8expr - expression statement
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

#include "c8expr.h"
#include "c8stmtimp.h"
#include "c8stmt.h"
#include "c8eval.h"
#include "c8script.h"
#include "c8buf.h"
#include "c8obj.h"
#include "c8debug.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

struct c8expr {
  struct c8stmt base;
  struct c8buf expr;
};

static void c8expr_destroy(struct c8stmt* o)
{
  struct c8expr* eo = to_c8expr(o);
  assert(eo);
  c8buf_clear(&eo->expr);
  free(eo);
}

static int c8expr_parse(struct c8stmt* o, struct c8script* script,
			     const char* token)
{
  struct c8expr* eo = to_c8expr(o);
  assert(eo);
  return C8_PARSERESULT_POP;
}

static int c8expr_parse_mode(struct c8stmt* o)
{
  struct c8expr* eo = to_c8expr(o);
  assert(eo);
  return C8_PARSEMODE_STATEMENT;
}

static struct c8obj* c8expr_run(struct c8stmt* o,
				struct c8script* script, int* flow)
{
  struct c8expr* eo = to_c8expr(o);
  assert(eo);
  c8debug(C8_DEBUG_INFO, "c8expr_run: %s", c8buf_str(&eo->expr));
  struct c8eval* eval = c8script_eval(script);
  return c8eval_expr(eval, c8buf_str(&eo->expr));
}

static const struct c8stmt_imp c8expr_imp = {
  c8expr_destroy,
  c8expr_parse,
  c8expr_parse_mode,
  c8expr_run,
};

struct c8expr* to_c8expr(struct c8stmt* o)
{
  return (o && o->imp && o->imp == &c8expr_imp) ?
    (struct c8expr*)o : 0;
}

struct c8expr* c8expr_create(const char* expr)
{
  struct c8expr* eo = malloc(sizeof(struct c8expr));
  eo->base.imp = &c8expr_imp;
  eo->base.parent = 0;
  c8buf_init(&eo->expr);
  c8buf_append_str(&eo->expr, expr);
  return eo;
}
