/** c8obj - base object
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

#include "c8obj.h"
#include "c8objimp.h"

#include <assert.h>

struct c8obj* c8obj_ref(struct c8obj* o)
{
  assert(o);
  ++o->refs;
  return o;
}

void c8obj_unref(struct c8obj* o)
{
  if (!o) return;
  assert(o->imp);
  assert(o->refs > 0);
  if (--o->refs == 0) {
    (o->imp->destroy)(o);
  }
}

struct c8obj* c8obj_copy(const struct c8obj* o)
{
  assert(o);
  return (o->imp->copy)(o);
}

int c8obj_int(const struct c8obj* o)
{
  assert(o);
  return (o->imp->to_int)(o);
}

void c8obj_str(const struct c8obj* o, struct c8buf* buf, int f)
{
  assert(o);
  (o->imp->to_str)(o, buf, f);
}

struct c8obj* c8obj_op(struct c8obj* o, int op, struct c8obj* p)
{
  assert(o);
  return (o->imp->op)(o, op, p);
}
