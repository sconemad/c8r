/** c8list - list object
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

struct c8list;
struct c8obj;

/** Safe casts from c8obj
 */
const struct c8list* to_const_c8list(const struct c8obj* o);
struct c8list* to_c8list(struct c8obj* o);

/** Create a c8list object
 */
struct c8list* c8list_create();

int c8list_size(const struct c8list* lo);
struct c8obj* c8list_at(struct c8list* lo, int i);

/** Stack-like access
 */
void c8list_push_back(struct c8list* lo, struct c8obj* p);
void c8list_pop_back(struct c8list* lo);

void c8list_push_front(struct c8list* lo, struct c8obj* p);
void c8list_pop_front(struct c8list* lo);
