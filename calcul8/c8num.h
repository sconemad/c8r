/** c8num - generic numeric object
 *
 * Copyright (c) 2021 Andrew Wedgbury <wedge@sconemad.com>
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

struct c8num;
struct c8obj;
struct c8ctx;
struct c8list;

/** Safe casts from c8obj
 */
const struct c8num* to_const_c8num(const struct c8obj* o);
struct c8num* to_c8num(struct c8obj* o);

/** Create a c8num object
 */
struct c8num* c8num_create_str(const char* str);
struct c8num* c8num_parse(const char** c);

/** Add numeric functions to context
 */
void c8num_init_ctx(struct c8ctx* ctx);

/** Register implementations 
 */
typedef struct c8num* (*c8num_type_create_func)
(const char* str);
void c8num_register_int_create(c8num_type_create_func f);
void c8num_register_real_create(c8num_type_create_func f);
void c8num_register_cplx_create(c8num_type_create_func f);
