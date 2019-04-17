/** c8group - group statement
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

#include "c8group.h"
#include "c8stmtimp.h"
#include "c8stmt.h"
#include "c8script.h"
#include "c8error.h"
#include "c8eval.h"
#include "c8vec.h"
#include "c8obj.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

struct c8group {
  struct c8stmt base;
  struct c8vec vec;
};

static void c8group_destroy(struct c8stmt* o)
{
  struct c8group* go = to_c8group(o);
  assert(go);
  int size = c8vec_size(&go->vec);
  for (int i=0; i<size; ++i) {
    struct c8stmt* item = (struct c8stmt*)c8vec_at(&go->vec, i);
    c8stmt_destroy(item);
  }
  c8vec_clear(&go->vec);
  free(go);
}

static int c8group_parse(struct c8stmt* o, struct c8script* script,
			 const char* token)
{
  printf("c8group_parse: %s\n", token);
  struct c8group* go = to_c8group(o);
  assert(go);

  if (strcmp(token, "}") == 0) {
    return C8_PARSERESULT_END;
  }

  struct c8stmt* sub = c8script_parse_token(script, token);
  if (!sub) return C8_PARSERESULT_ERROR;
  c8stmt_set_parent(sub, o);
  c8vec_push_back(&go->vec, sub);
  return C8_PARSERESULT_CONTINUE;
}

static int c8group_parse_mode(struct c8stmt* o)
{
  struct c8group* go = to_c8group(o);
  assert(go);
  return C8_PARSEMODE_STATEMENT;
}

static struct c8obj* c8group_run(struct c8stmt* o,
				 struct c8script* script, int* flow)
{
  struct c8group* go = to_c8group(o);
  assert(go);
  struct c8obj* ret = 0;
  int size = c8vec_size(&go->vec);
  for (int i=0; i<size; ++i) {
    struct c8stmt* item = (struct c8stmt*)c8vec_at(&go->vec, i);
    c8obj_unref(ret);
    ret = c8stmt_run(item, script, flow);
    if (ret && to_c8error(ret)) break;
    if (*flow != C8_FLOW_NORMAL) break;
  }
  return ret;
}

static const struct c8stmt_imp c8group_imp = {
  c8group_destroy,
  c8group_parse,
  c8group_parse_mode,
  c8group_run,
};

struct c8group* to_c8group(struct c8stmt* o)
{
  return (o && o->imp && o->imp == &c8group_imp) ?
    (struct c8group*)o : 0;
}

struct c8group* c8group_create(const char* expr)
{
  struct c8group* go = malloc(sizeof(struct c8group));
  go->base.imp = &c8group_imp;
  go->base.parent = 0;
  c8vec_init(&go->vec);
  return go;
}
