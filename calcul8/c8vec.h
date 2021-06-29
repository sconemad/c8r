/** c8vec - generic vector/stack
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

/** Generic vector struct
 */
struct c8vec {
  void** items;
  int size;
  int max;
};

void c8vec_init(struct c8vec* o);
void c8vec_clear(struct c8vec* o);

int c8vec_size(const struct c8vec* o);
void* c8vec_at(struct c8vec* o, int i);
const void* c8vec_const_at(const struct c8vec* o, int i);
  
void c8vec_push_back(struct c8vec* o, void* item);
void* c8vec_pop_back(struct c8vec* o);

void c8vec_push_front(struct c8vec* o, void* item);
void* c8vec_pop_front(struct c8vec* o);
