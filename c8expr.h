/** c8expr - expression evaluator
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

struct c8expr;
struct c8ctx;
struct c8obj;

/** Create an expression evaluator
 */
struct c8expr* c8expr_create(struct c8ctx* global);

/** Destroy an expression evaluator
 */
void c8expr_destroy(struct c8expr* o);

/** Evaluate an expression
 */
struct c8obj* c8expr_eval(struct c8expr* o, const char* expr);

/** Set local context
 */
void c8expr_set_local(struct c8expr* o, struct c8ctx* local);
