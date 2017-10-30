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

#include "c8vec.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void c8vec_init(struct c8vec* o)
{
  o->size = 0;
  o->max = 0;
  o->items = 0;
}

void c8vec_clear(struct c8vec* o)
{
  if (o->items) free(o->items);
  c8vec_init(o);
}

int c8vec_size(const struct c8vec* o)
{
  assert(o);
  return o->size;
}

void* c8vec_at(struct c8vec* o, int i)
{
  assert(o);
  if (i<0) i = o->size + i;
  assert(i>=0 && i<o->size);
  return o->items[i];
}

const void* c8vec_const_at(const struct c8vec* o, int i)
{
  assert(o);
  assert(i>=0 && i<o->size);
  return o->items[i];
}

static void c8vec_realloc(struct c8vec* o)
{
  assert(o);
  // Increase in powers of 2
  for (o->max=1; o->max<=o->size; o->max*=2) ;
  o->items = realloc(o->items, o->max * sizeof(void*));
}

void c8vec_push_back(struct c8vec* o, void* item)
{
  assert(o);
  ++o->size;
  if (o->size > o->max) c8vec_realloc(o);
  o->items[o->size-1] = item;
}

void* c8vec_pop_back(struct c8vec* o)
{
  assert(o);
  assert(o->size > 0);
  --o->size; //xxx need to realloc if reduced significantly?
  return o->items[o->size];
}

void c8vec_push_front(struct c8vec* o, void* item)
{
  assert(o);
  int psize = o->size;
  ++o->size;
  if (o->size > o->max) c8vec_realloc(o);
  memmove(&o->items[1], o->items, psize * sizeof(void*));
  o->items[0] = item;
}

void* c8vec_pop_front(struct c8vec* o)
{
  assert(o);
  assert(o->size > 0);
  void* r = o->items[0];
  --o->size; //xxx need to realloc if reduced significantly?
  memmove(o->items, &o->items[1], o->size * sizeof(void*));
  return r;
}
