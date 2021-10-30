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
struct c8num* c8num_create();
struct c8num* c8num_create_int(int value);
struct c8num* c8num_create_double(long double value);
struct c8num* c8num_create_cplx(long double real, long double imag);
struct c8num* c8num_create_str(const char* str);
struct c8num* c8num_parse(const char** c);
struct c8num* c8num_create_c8obj(const struct c8obj* obj);

const struct c8obj* c8num_const_value(const struct c8num* oo);
struct c8obj* c8num_value(struct c8num* oo);

/** Add numeric functions to context
 */
void c8num_init_ctx(struct c8ctx* ctx);

/** Functions
 */
struct c8obj* c8num_abs(struct c8list* args);
struct c8obj* c8num_ceil(struct c8list* args);
struct c8obj* c8num_floor(struct c8list* args);
struct c8obj* c8num_trunc(struct c8list* args);
struct c8obj* c8num_log(struct c8list* args);
struct c8obj* c8num_exp(struct c8list* args);
struct c8obj* c8num_pow(struct c8list* args);
struct c8obj* c8num_sqrt(struct c8list* args);
struct c8obj* c8num_cos(struct c8list* args);
struct c8obj* c8num_sin(struct c8list* args);
struct c8obj* c8num_tan(struct c8list* args);
struct c8obj* c8num_acos(struct c8list* args);
struct c8obj* c8num_asin(struct c8list* args);
struct c8obj* c8num_atan(struct c8list* args);
struct c8obj* c8num_atan2(struct c8list* args);
struct c8obj* c8num_cosh(struct c8list* args);
struct c8obj* c8num_sinh(struct c8list* args);
struct c8obj* c8num_tanh(struct c8list* args);
struct c8obj* c8num_mean(struct c8list* args);
