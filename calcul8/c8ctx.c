/** c8ctx - context
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

#include "c8ctx.h"
#include "c8func.h"
#include "c8map.h"
#include "c8obj.h"

#include <stdlib.h>
#include <assert.h>

struct c8ctx {
  struct c8map* lib;
};

struct c8ctx* c8ctx_create()
{
  struct c8ctx* o = malloc(sizeof(struct c8ctx));
  assert(o);
  o->lib = c8map_create();
  return o;
}

void c8ctx_destroy(struct c8ctx* o)
{
  assert(o);
  c8obj_unref((struct c8obj*)o->lib);
  free(o);
}

void c8ctx_add(struct c8ctx* o, const char* name, struct c8obj* obj)
{
  assert(o);
  c8map_set(o->lib, name, obj);
  c8obj_unref(obj);
}

struct c8obj* c8ctx_resolve(struct c8ctx* o, const char* name)
{
  assert(o);
  return c8map_lookup(o->lib, name);
}
