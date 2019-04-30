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
#include "c8objimp.h"
#include "c8ctx.h"
#include "c8buf.h"
#include "c8debug.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

struct c8group {
  struct c8stmt base;
  struct c8vec vec;
  struct c8ctx* ctx;
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
  c8ctx_destroy(go->ctx);
  free(go);
}

static int c8group_parse(struct c8stmt* o, struct c8script* script,
			 const char* token)
{
  c8debug(C8_DEBUG_INFO, "c8group_parse: %s", token);
  struct c8group* go = to_c8group(o);
  assert(go);

  if (strcmp(token, "}") == 0) {
    return C8_PARSE_END;
  }

  struct c8stmt* sub = c8script_parse_token(script, token);
  if (!sub) return C8_PARSE_ERROR;
  c8stmt_set_parent(sub, o);
  c8vec_push_back(&go->vec, sub);
  return C8_PARSE_CONTINUE;
}

static int c8group_parse_mode(struct c8stmt* o)
{
  struct c8group* go = to_c8group(o);
  assert(go);
  return C8_PARSEMODE_STATEMENT;
}

static struct c8obj* c8group_resolve(const char* name, void* data)
{
  struct c8stmt* o = (struct c8stmt*)data;
  if (!o) return 0;
  struct c8group* go = to_c8group(o);
  if (!go) return c8group_resolve(name, o->parent);
  struct c8obj* ret = c8ctx_resolve(go->ctx, name);
  if (ret) return ret;
  return c8group_resolve(name, o->parent);
}

static int c8group_run(struct c8stmt* o, struct c8script* script)
{
  struct c8group* go = to_c8group(o);
  assert(go);
  int ret = C8_RUN_NORMAL;
  c8eval_resolver_func save_resolver;
  void* save_resolver_data;

  struct c8eval* ev = c8script_eval(script);
  save_resolver = c8eval_get_resolver(ev, &save_resolver_data);

  int size = c8vec_size(&go->vec);
  for (int i=0; i<size; ++i) {
    struct c8stmt* item = (struct c8stmt*)c8vec_at(&go->vec, i);

    c8eval_set_resolver(ev, c8group_resolve, o);
    ret = c8stmt_run(item, script);

    if (ret != C8_RUN_NORMAL) break;
  }
  c8eval_set_resolver(ev, save_resolver, save_resolver_data);
  return ret;
}

static const struct c8stmt_imp c8group_imp = {
  c8group_destroy,
  c8group_parse,
  c8group_parse_mode,
  c8group_run,
};

struct c8group* find_parent_group(struct c8stmt* o)
{
  struct c8stmt* p = o->parent;
  while (p) {
    struct c8group* g = to_c8group(p);
    if (g) return g;
    p = p->parent;
  }
  return 0;
}

struct c8group* to_c8group(struct c8stmt* o)
{
  return (o && o->imp && o->imp == &c8group_imp) ?
    (struct c8group*)o : 0;
}

struct c8group* c8group_create()
{
  struct c8group* go = malloc(sizeof(struct c8group));
  go->base.imp = &c8group_imp;
  go->base.parent = 0;
  go->base.line = 0;
  c8vec_init(&go->vec);
  go->ctx = c8ctx_create();
  return go;
}

struct c8ctx* c8group_ctx(struct c8group* o)
{
  assert(o);
  return o->ctx;
}
