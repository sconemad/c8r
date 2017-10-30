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

#pragma once

struct c8ctx;
struct c8obj;

/** Create a context
 */
struct c8ctx* c8ctx_create();

/** Destroy a context
 */
void c8ctx_destroy(struct c8ctx* o);

/** Add a named object to the context
 */
void c8ctx_add(struct c8ctx* o, const char* name, struct c8obj* obj);

/** Resolve a named object in the context
 */
struct c8obj* c8ctx_resolve(struct c8ctx* o, const char* name);
