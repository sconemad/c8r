/** c8mpz - numeric object using GNU mpz
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

struct c8mpz;
struct c8obj;
struct cbbuf;
struct c8ctx;
struct c8list;

/** Safe casts from c8obj
 */
const struct c8mpz* to_const_c8mpz(const struct c8obj* o);
struct c8mpz* to_c8mpz(struct c8obj* o);

/** Create a c8mpz object
 */
struct c8mpz* c8mpz_create();
struct c8mpz* c8mpz_create_int(int value);
struct c8mpz* c8mpz_create_double(double value);
struct c8mpz* c8mpz_create_str(const char* str);

/** Add mpz functions to context
 */
void c8mpz_init_ctx(struct c8ctx* ctx);

/** Functions
 */
struct c8obj* c8mpz_abs(struct c8list* args);
struct c8obj* c8mpz_gcd(struct c8list* args);
struct c8obj* c8mpz_lcm(struct c8list* args);
struct c8obj* c8mpz_fib(struct c8list* args);
