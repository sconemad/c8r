/** c8error - object representing an error
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

/** c8error codes
 */
#define C8_ERROR_UNKNOWN 0
#define C8_ERROR_UNDEFINED_NAME 1
#define C8_ERROR_ARGUMENT 2
#define C8_ERROR_LIST_INIT 3
#define C8_ERROR_MAP_INIT 4
#define C8_ERROR_PARENTHESIS 5


struct c8error;
struct c8obj;

/** Safe casts from c8obj
 */
const struct c8error* to_const_c8error(const struct c8obj* o);
struct c8error* to_c8error(struct c8obj* o);

/** Create a c8error object
 */
struct c8error* c8error_create(int code);
struct c8error* c8error_create_arg(int code, const char* arg);

/** Set/get the error code
 */
void c8error_set(struct c8error* oo, int code);
int c8error_code(const struct c8error* oo);

const char* c8error_arg(const struct c8error* oo);
