/** c8buf - generic data buffer/string
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

/** Returns a new copy of a c string
 */
char* copy_str(const char* cstr);

/** Buffer/string
 */
struct c8buf {
  char* data;
  int len;
  int max;
};

void c8buf_init(struct c8buf* o);
void c8buf_init_str(struct c8buf* o, const char* str);
void c8buf_init_copy(struct c8buf* o, const struct c8buf* buf);

void c8buf_clear(struct c8buf* o);

int c8buf_len(const struct c8buf* o);
const char* c8buf_str(const struct c8buf* o);

void c8buf_append_buf(struct c8buf* o, const struct c8buf* buf);
void c8buf_append_strn(struct c8buf* o, const char* str, int n);
void c8buf_append_str(struct c8buf* o, const char* str);
void c8buf_append_fmt(struct c8buf* o, const char* fmt, ...);
