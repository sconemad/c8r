/** c8stmtimp - statement implementation
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

struct c8stmt;
struct c8obj;
struct c8script;

typedef void (*c8stmt_destroy_func)
(struct c8stmt* o);

typedef int (*c8stmt_parse_func)
(struct c8stmt* o, struct c8script* script, const char* token);

typedef int (*c8stmt_parse_mode_func)
(struct c8stmt* o);

typedef struct c8obj* (*c8stmt_run_func)
(struct c8stmt* o, struct c8script* script, int* flow);

struct c8stmt_imp {
  c8stmt_destroy_func destroy;
  c8stmt_parse_func parse;
  c8stmt_parse_mode_func parse_mode;
  c8stmt_run_func run;
};

struct c8stmt {
  const struct c8stmt_imp* imp;
  struct c8stmt* parent;
  int line;
};

