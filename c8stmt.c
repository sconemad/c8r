/** c8stmt - script statements
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

#include "c8stmt.h"
#include "c8stmtimp.h"
#include "c8obj.h"
#include "c8buf.h"

#include <assert.h>
#include <stdlib.h>

/* c8stmt */

void c8stmt_destroy(struct c8stmt* o)
{
  if (!o) return;
  assert(o->imp);
  (o->imp->destroy)(o);
}

int c8stmt_parse(struct c8stmt* o, struct c8script* script,
		 const char* token)
{
  assert(o);
  return (o->imp->parse)(o, script, token);
}

int c8stmt_parse_mode(struct c8stmt* o)
{
  assert(o);
  return (o->imp->parse_mode)(o);
}

void c8stmt_set_parent(struct c8stmt* o, struct c8stmt* p)
{
  assert(o);
  o->parent = p;
}

int c8stmt_run(struct c8stmt* o, struct c8script* script)
{
  assert(o);
  return (o->imp->run)(o, script);
}
