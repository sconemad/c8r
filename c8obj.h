/** c8obj - base object
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

struct c8obj;
struct c8buf;

/** Increment refererence count of a c8obj
 */
struct c8obj* c8obj_ref(struct c8obj* o);

/** Decrement reference count of a c8obj
 * When the reference count reaches zero, the c8obj will be destroyed.
 */
void c8obj_unref(struct c8obj* o);

/** Return a deep copy of a c8obj
 */
struct c8obj* c8obj_copy(const struct c8obj* o);

/* Get an integer representation of this c8obj
 */
int c8obj_int(const struct c8obj* o);

/** Get a string representation of this c8obj
 * Appends to supplied buf.
 * Uses format flags f.
 */
void c8obj_str(const struct c8obj* o, struct c8buf* buf, int f);

#define C8_FMT_MASK_BASE 0x07
#define C8_FMT_DEC 0x0
#define C8_FMT_BIN 0x1
#define C8_FMT_OCT 0x2
#define C8_FMT_HEX 0x3
#define C8_FMT_SCI 0x4
#define C8_FMT_FIX 0x5

#define C8_FMT_JSON 0x10

/** Perform an operation on this object
 * If op is a binary operation, p gives the right hand value.
 */
struct c8obj* c8obj_op(struct c8obj* o, int op, struct c8obj* p);
