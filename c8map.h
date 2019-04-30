/** c8map - map object
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

struct c8map;
struct c8list;
struct c8obj;
struct c8buf;

/** Safe casts from c8obj
 */
const struct c8map* to_const_c8map(const struct c8obj* o);
struct c8map* to_c8map(struct c8obj* o);

/** Create a c8map object
 */
struct c8map* c8map_create();

/** Allow iteration through map contents
 */
int c8map_size(const struct c8map* oo);
struct c8obj* c8map_at(struct c8map* oo, int i, struct c8buf* key);

/** Get list of map keys
 */
struct c8list* c8map_keys(const struct c8map* oo);

/** Lookup by key
 */
struct c8obj* c8map_lookup(struct c8map* oo, const char* key);

/** Set by key
 */
void c8map_set(struct c8map* oo, const char* key, struct c8obj* value);
