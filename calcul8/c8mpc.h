/** c8mpc - complex numeric object using GNU mpc
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

struct c8mpc;
struct c8obj;
struct c8ctx;
struct c8list;

/** Safe casts from c8obj
 */
const struct c8mpc* to_const_c8mpc(const struct c8obj* o);
struct c8mpc* to_c8mpc(struct c8obj* o);

/** Create a c8mpc object
 */
struct c8mpc* c8mpc_create();
struct c8mpc* c8mpc_create_int(int rvalue, int ivalue);
struct c8mpc* c8mpc_create_double(long double rvalue, long double ivalue);
struct c8mpc* c8mpc_create_str(const char* str);
struct c8mpc* c8mpc_create_c8obj(const struct c8obj* obj);

/** Add mpc functions to context
 */
void c8mpc_init_ctx(struct c8ctx* ctx);

/** Functions
 */
struct c8obj* c8mpc_log(struct c8list* args);
struct c8obj* c8mpc_exp(struct c8list* args);
struct c8obj* c8mpc_sqrt(struct c8list* args);
struct c8obj* c8mpc_cos(struct c8list* args);
struct c8obj* c8mpc_sin(struct c8list* args);
struct c8obj* c8mpc_tan(struct c8list* args);
struct c8obj* c8mpc_acos(struct c8list* args);
struct c8obj* c8mpc_asin(struct c8list* args);
struct c8obj* c8mpc_atan(struct c8list* args);
struct c8obj* c8mpc_cosh(struct c8list* args);
struct c8obj* c8mpc_sinh(struct c8list* args);
struct c8obj* c8mpc_tanh(struct c8list* args);
