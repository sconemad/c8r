/** c8func - object representing a function or method call
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

struct c8func;
struct c8obj;
struct c8list;

typedef struct c8obj* (*c8func_func)
(struct c8list* args);

/** Safe casts from c8obj
 */
const struct c8func* to_const_c8func(const struct c8obj* o);
struct c8func* to_c8func(struct c8obj* o);

/** Create a c8func object
 */
struct c8func* c8func_create(c8func_func f);
struct c8func* c8func_create_method(c8func_func f, struct c8obj* obj);

/** Call the function
 */
struct c8obj* c8func_call(struct c8func* oo, struct c8list* args);
