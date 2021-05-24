/** c8script - script parser
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

struct c8script;
struct c8stmt;
struct c8obj;
struct c8eval;
struct c8ctx;

struct c8script* c8script_create(struct c8ctx* global);

void c8script_destroy(struct c8script* o);

int c8script_parse(struct c8script* o, const char* script);

int c8script_run(struct c8script* o);

int c8script_current_line(struct c8script* o);

/** Get the eval object
 */
struct c8eval* c8script_eval(struct c8script* o);

/** Give/take current subroutine return value
 */
void c8script_give_ret(struct c8script* o, struct c8obj* ret);
struct c8obj* c8script_take_ret(struct c8script* o);

// internal

struct c8stmt* c8script_parse_token(struct c8script* o, const char* token);

void c8script_set_running(struct c8script* o, struct c8stmt* s);
