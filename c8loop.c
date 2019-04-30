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
#include "c8debug.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

struct c8loop {
  struct c8stmt base;
  int seq;
  int type;
  struct c8buf initialiser;
  struct c8buf condition;
  struct c8buf increment;
  struct c8stmt* body;
};

static void c8loop_destroy(struct c8stmt* o)
{
  struct c8loop* co = to_c8loop(o);
  assert(co);
  c8buf_clear(&co->initialiser);
  c8buf_clear(&co->condition);
  c8buf_clear(&co->increment);
  c8stmt_destroy(co->body);
  free(co);
}

static int parse_loop(struct c8loop* lo, const char* token)
{
  c8buf_clear(&lo->initialiser);
  c8buf_clear(&lo->condition);
  c8buf_clear(&lo->increment);

  if (lo->type == C8_LOOP_WHILE) {
    c8buf_init_str(&lo->condition, token);
    
  } else if (lo->type == C8_LOOP_FOR) {
    char* orig = strdup(token);
    char* tok = orig;
    const char* a = strsep(&tok, ";");
    const char* b = strsep(&tok, ";");
    const char* c = strsep(&tok, ";");
    
    if (a) c8buf_init_str(&lo->initialiser, a);
    if (b) c8buf_init_str(&lo->condition, b);
    if (c) c8buf_init_str(&lo->increment, c);
    
    free((void*)orig);
  }

  return C8_PARSE_CONTINUE;
}

static int c8loop_parse(struct c8stmt* o, struct c8script* script,
                        const char* token)
{
  c8debug(C8_DEBUG_INFO, "c8loop_parse: %s", token);
  struct c8loop* lo = to_c8loop(o);
  assert(lo);

  switch (++lo->seq) {
  case 1:
    return parse_loop(lo, token);

  case 2:
    lo->body = c8script_parse_token(script, token);
    if (lo->body) c8stmt_set_parent(lo->body, o);
    break;

  default:
    return C8_PARSE_POP;
  }

  return C8_PARSE_CONTINUE;
}

static int c8loop_parse_mode(struct c8stmt* o)
{
  struct c8loop* lo = to_c8loop(o);
  assert(lo);
  return (lo->seq == 0 ? C8_PARSEMODE_BRACKETED : C8_PARSEMODE_STATEMENT);
}

static int c8loop_run(struct c8stmt* o, struct c8script* script)
{
  struct c8loop* lo = to_c8loop(o);
  assert(lo);
  int ret = C8_RUN_NORMAL; 
  struct c8eval* eval = c8script_eval(script);

  // Run initialiser
  if (c8buf_str(&lo->initialiser)) {
    struct c8obj* r = c8eval_expr(eval, c8buf_str(&lo->initialiser));
    c8obj_unref(r);
  }

  while (1) {
    // Evaluate condition
    int cr = c8eval_cond(eval, c8buf_str(&lo->condition));
    if (cr < 0) return C8_RUN_ERROR;
    if (cr == 0) break;

    // Run body
    if (lo->body) {
      ret = c8stmt_run(lo->body, script);
      if (ret == C8_RUN_ERROR ||
          ret == C8_RUN_RETURN) {
        break;
      }
      if (ret == C8_RUN_LAST) {
        ret = C8_RUN_NORMAL;
        break;
      }
      if (ret == C8_RUN_NEXT) {
        ret = C8_RUN_NORMAL;
      }
    }

    // Run increment
    if (c8buf_str(&lo->increment)) {
      struct c8obj* r = c8eval_expr(eval, c8buf_str(&lo->increment));
      c8obj_unref(r);
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

struct c8loop* c8loop_create(int type)
{
  struct c8loop* lo = malloc(sizeof(struct c8loop));
  lo->base.imp = &c8loop_imp;
  lo->base.parent = 0;
  lo->seq = 0;
  lo->type = type;
  c8buf_init(&lo->initialiser);
  c8buf_init(&lo->condition);
  c8buf_init(&lo->increment);
  lo->body = 0;
  return lo;
}
