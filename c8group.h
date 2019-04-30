/** c8group - group statement
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

struct c8group;
struct c8stmt;
struct c8ctx;

/** Find parent group
 */
struct c8group* find_parent_group(struct c8stmt* o);

/** Safe cast from c8stmt
 */
struct c8group* to_c8group(struct c8stmt* o);

/** Create a c8group statement
 */
struct c8group* c8group_create();

/** Get the context for this group
 */
struct c8ctx* c8group_ctx(struct c8group* oo);
