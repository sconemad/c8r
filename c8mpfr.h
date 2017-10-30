/** c8mpfr - numeric object using GNU mpfr
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

struct c8mpfr;
struct c8obj;
struct c8ctx;
struct c8list;

/** Safe casts from c8obj
 */
const struct c8mpfr* to_const_c8mpfr(const struct c8obj* o);
struct c8mpfr* to_c8mpfr(struct c8obj* o);

/** Create a c8mpfr object
 */
struct c8mpfr* c8mpfr_create();
struct c8mpfr* c8mpfr_create_int(int value);
struct c8mpfr* c8mpfr_create_double(long double value);
struct c8mpfr* c8mpfr_create_str(const char* str);
struct c8mpfr* c8mpfr_create_c8obj(const struct c8obj* obj);

/** Add mpfr functions to context
 */
void c8mpfr_init_ctx(struct c8ctx* ctx);

/** Functions
 */
struct c8obj* c8mpfr_abs(struct c8list* args);
struct c8obj* c8mpfr_ceil(struct c8list* args);
struct c8obj* c8mpfr_floor(struct c8list* args);
struct c8obj* c8mpfr_trunc(struct c8list* args);
struct c8obj* c8mpfr_log(struct c8list* args);
struct c8obj* c8mpfr_exp(struct c8list* args);
struct c8obj* c8mpfr_pow(struct c8list* args);
struct c8obj* c8mpfr_sqrt(struct c8list* args);
struct c8obj* c8mpfr_cos(struct c8list* args);
struct c8obj* c8mpfr_sin(struct c8list* args);
struct c8obj* c8mpfr_tan(struct c8list* args);
struct c8obj* c8mpfr_acos(struct c8list* args);
struct c8obj* c8mpfr_asin(struct c8list* args);
struct c8obj* c8mpfr_atan(struct c8list* args);
struct c8obj* c8mpfr_atan2(struct c8list* args);
struct c8obj* c8mpfr_cosh(struct c8list* args);
struct c8obj* c8mpfr_sinh(struct c8list* args);
struct c8obj* c8mpfr_tanh(struct c8list* args);
struct c8obj* c8mpfr_mean(struct c8list* args);
