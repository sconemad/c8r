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

#include "c8buf.h"

#define _GNU_SOURCE
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

char* copy_str(const char* str)
{
  assert(str);
  int len = strlen(str)+1;
  char* cstr = (char*)malloc(len);
  memcpy(cstr, str, len);
  return cstr;
}

void c8buf_init(struct c8buf* o)
{
  o->len = 0;
  o->max = 0;
  o->data = 0;
}

void c8buf_init_str(struct c8buf* o, const char* str)
{
  o->len = strlen(str);
  o->max = o->len + 1;
  o->data = (char*)malloc(o->max);
  memcpy(o->data, str, o->max);
}

void c8buf_init_copy(struct c8buf* o, const struct c8buf* buf)
{
  o->len = buf->len;
  o->max = buf->len + 1;
  o->data = (char*)malloc(o->max);
  memcpy(o->data, buf->data, o->max);
}

void c8buf_clear(struct c8buf* o)
{
  if (o->data) free(o->data);
  c8buf_init(o);
}

int c8buf_len(const struct c8buf* o)
{
  assert(o);
  return o->len;
}

const char* c8buf_str(const struct c8buf* o)
{
  assert(o);
  return (const char*)o->data;
}

static void c8buf_realloc(struct c8buf* o)
{
  assert(o);
  // Increase the buffer size in powers of 2
  for (o->max=1; o->max<=o->len+1; o->max*=2) ;
  o->data = realloc(o->data, o->max);
}

void c8buf_append_buf(struct c8buf* o, const struct c8buf* buf)
{
  assert(o);
  assert(buf);
  int pl = o->len;
  o->len += buf->len;
  if (o->len+1 > o->max) c8buf_realloc(o);
  memcpy(o->data+pl, buf->data, buf->len+1);
}

void c8buf_append_strn(struct c8buf* o, const char* str, int n)
{
  assert(o);
  assert(str);
  int sl = strlen(str);
  if (n < sl) sl = n;
  int pl = o->len;
  o->len += sl;
  if (o->len+1 > o->max) c8buf_realloc(o);
  memcpy(o->data+pl, str, sl);
  *(char*)(o->data + o->len) = 0;
}

void c8buf_append_str(struct c8buf* o, const char* str)
{
  c8buf_append_strn(o, str, strlen(str));
}

void c8buf_append_fmt(struct c8buf* o, const char* fmt, ...)
{
  assert(o);
  assert(fmt);
  char* str = 0;
  va_list ap;
  va_start(ap, fmt);
  int sl = vasprintf(&str, fmt, ap);
  va_end(ap);
  int pl = o->len;
  o->len += sl;
  if (o->len+1 > o->max) c8buf_realloc(o);
  memcpy(o->data+pl, str, sl+1);
  free(str);
}
