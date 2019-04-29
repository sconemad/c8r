/** c8sub - subroutine statement and object
 *
 * Copyright (c) 2019 Andrew Wedgbury <wedge@sconemad.com>
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

struct c8subdef;
struct c8sub;
struct c8obj;
struct c8stmt;
struct c8list;
struct c8script;

/** Safe cast from c8stmt
 */
struct c8subdef* to_c8subdef(struct c8stmt* o);

/** Create a c8subdef statement
 */
struct c8subdef* c8subdef_create();


/** Safe casts from c8obj
 */
const struct c8sub* to_const_c8sub(const struct c8obj* o);
struct c8sub* to_c8sub(struct c8obj* o);

/** Create a c8sub object
 */
struct c8sub* c8sub_create(struct c8subdef* def, struct c8script* script);

/** Call the subroutine
 */
struct c8obj* c8sub_call(struct c8sub* fo, struct c8list* args);
