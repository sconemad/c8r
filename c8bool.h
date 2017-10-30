/** c8bool - boolean type
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

struct c8bool;
struct c8obj;
struct c8buf;

#define C8_BOOL_FALSE 0
#define C8_BOOL_TRUE 1

/** Safe casts from c8obj
 */
const struct c8bool* to_const_c8bool(const struct c8obj* o);
struct c8bool* to_c8bool(struct c8obj* o);

/** Create a c8bool object
 */
struct c8bool* c8bool_create(int value);

/** Set/get the value
 */
void c8bool_set(struct c8bool* bo, int code);
int c8bool_value(const struct c8bool* bo);
