/** c8string - string object
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

struct c8string;
struct c8obj;
struct c8buf;

/** Safe casts from c8obj
 */
const struct c8string* to_const_c8string(const struct c8obj* o);
struct c8string* to_c8string(struct c8obj* o);

/** Create a c8string object
 */
struct c8string* c8string_create();
struct c8string* c8string_create_str(const char* str);
struct c8string* c8string_create_buf(const struct c8buf* buf);

