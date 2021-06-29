/** c8map - map object
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

#include "c8map.h"
#include "c8obj.h"
#include "c8objimp.h"
#include "c8vec.h"
#include "c8list.h"
#include "c8string.h"
#include "c8buf.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct entry {
  struct c8buf key;
  struct c8obj* value;
};

struct c8map {
  struct c8obj base;
  struct c8vec vec;
};

static void c8map_destroy(struct c8obj* o)
{
  struct c8map* oo = to_c8map(o);
  assert(oo);
  int size = c8vec_size(&oo->vec);
  for (int i=0; i<size; ++i) {
    struct entry* e = (struct entry*)c8vec_at(&oo->vec, i);
    c8buf_clear(&e->key);
    c8obj_unref(e->value);
    free(e);
  }
  c8vec_clear(&oo->vec);
  free(oo);
}

static struct c8obj* c8map_copy(const struct c8obj* o)
{
  const struct c8map* oo = to_const_c8map(o);
  assert(oo);
  struct c8map* mc = c8map_create();
  int size = c8vec_size(&oo->vec);
  for (int i=0; i<size; ++i) {
    const struct entry* e = (const struct entry*)c8vec_const_at(&oo->vec, i);
    struct c8obj* vc = e->value ? c8obj_copy(e->value) : 0;
    c8map_set(mc, c8buf_str(&e->key), vc);
    c8obj_unref(vc);
  }
  return (struct c8obj*)mc;
}

static int c8map_int(const struct c8obj* o)
{
  const struct c8map* oo = to_const_c8map(o);
  assert(oo);
  return c8vec_size(&oo->vec);
}

static void c8map_str(const struct c8obj* o, struct c8buf* buf, int f)
{
  const struct c8map* oo = to_const_c8map(o);
  assert(oo);
  c8buf_append_str(buf, "{");
  int size = c8vec_size(&oo->vec);
  for (int i=0; i<size; ++i) {
    if (i!=0) c8buf_append_str(buf, ",");
    const struct entry* e = (const struct entry*)c8vec_const_at(&oo->vec, i);
    c8buf_append_buf(buf, &e->key);
    c8buf_append_str(buf, ":");
    if (e->value) c8obj_str(e->value, buf, f);
  }
  c8buf_append_str(buf, "}");
}

static struct c8obj* c8map_op(struct c8obj* o, int op, struct c8obj* p)
{
  struct c8map* oo = to_c8map(o);
  assert(oo);
  //assert(p);
  return 0;
}

static const struct c8obj_imp c8map_imp = {
  c8map_destroy,
  c8map_copy,
  c8map_int,
  c8map_str,
  c8map_op
};

const struct c8map* to_const_c8map(const struct c8obj* o)
{
  return (o && o->imp && o->imp == &c8map_imp) ?
    (const struct c8map*)o : 0;
}

struct c8map* to_c8map(struct c8obj* o)
{
  return (struct c8map*)to_const_c8map(o);
}

struct c8map* c8map_create()
{
  struct c8map* oo = malloc(sizeof(struct c8map));
  assert(oo);
  oo->base.refs = 1;
  oo->base.imp = &c8map_imp;
  c8vec_init(&oo->vec);
  return oo;
}

int c8map_size(const struct c8map* oo)
{
  assert(oo);
  return c8vec_size(&oo->vec);
}


struct c8obj* c8map_at(struct c8map* oo, int i, struct c8buf* key)
{
  assert(oo);
  struct entry* e = (struct entry*)c8vec_at(&oo->vec, i);
  if (e) {
    if (key) c8buf_append_buf(key, &e->key);
    if (e->value) return c8obj_ref(e->value);
  }
  return 0;
}

struct c8list* c8map_keys(const struct c8map* oo)
{
  assert(oo);
  struct c8list* kl = c8list_create();
  int size = c8vec_size(&oo->vec);
  for (int i=0; i<size; ++i) {
    const struct entry* e = (const struct entry*)c8vec_const_at(&oo->vec, i);
    struct c8obj* k = (struct c8obj*)c8string_create_buf(&e->key);
    c8list_push_back(kl, k);
    c8obj_unref(k);
  }  
  return kl;
}

struct c8obj* c8map_lookup(struct c8map* oo, const char* key)
{
  assert(oo);
  int size = c8vec_size(&oo->vec);
  for (int i=0; i<size; ++i) {
    struct entry* e = (struct entry*)c8vec_at(&oo->vec, i);
    if (strcmp(c8buf_str(&e->key), key) == 0) {
      if (e->value) return c8obj_ref(e->value);
      return 0;
    }
  }
  return 0;
}

void c8map_set(struct c8map* oo, const char* key, struct c8obj* value)
{
  assert(oo);
  int size = c8vec_size(&oo->vec);
  for (int i=0; i<size; ++i) {
    struct entry* e = (struct entry*)c8vec_at(&oo->vec, i);
    if (strcmp(c8buf_str(&e->key), key) == 0) {
      c8obj_unref(e->value);
      e->value = value ? c8obj_ref(value) : 0;
      return;
    }
  }
  // Add a new entry
  struct entry* e = (struct entry*)malloc(sizeof (struct entry));
  c8buf_init_str(&e->key, key);
  e->value = value ? c8obj_ref(value) : 0;
  c8vec_push_back(&oo->vec, e);
}
