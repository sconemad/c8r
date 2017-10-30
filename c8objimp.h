/** c8objimp - object implementation
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

struct c8buf;
struct c8obj;

typedef void (*c8obj_destroy_func)
(struct c8obj* o);

typedef struct c8obj* (*c8obj_copy_func)
(const struct c8obj* o);

typedef int (*c8obj_int_func)
(const struct c8obj* o);

typedef void (*c8obj_str_func)
(const struct c8obj* o, struct c8buf* buf, int f);

typedef struct c8obj* (*c8obj_op_func)
(struct c8obj* o, int op, struct c8obj* p);

struct c8obj_imp {
  c8obj_destroy_func destroy;
  c8obj_copy_func copy;
  c8obj_int_func to_int;
  c8obj_str_func to_str;
  c8obj_op_func op;
};

struct c8obj {
  int refs;
  const struct c8obj_imp* imp;
};
