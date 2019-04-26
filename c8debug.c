/** c8debug - debugging
 *
 * Copyright (c) 2019 Andrew Wedgbury <wedge@sconemad.com>
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

#include "c8debug.h"

#include <stdio.h>
#include <stdarg.h>

static int debug_level = 0;

void c8debug_level(int level)
{
  debug_level = level;
}

void c8debug(int level, const char* msg, ...)
{
  if (level > debug_level) return;
  int c = 34;
  if (level == C8_DEBUG_ERROR) c = 31;
  if (level == C8_DEBUG_WARNING) c = 33;
  if (level == C8_DEBUG_INFO) c = 32;
  va_list ap;
  va_start(ap, msg);
  printf("\x1b[1;%dm", c);
  vprintf(msg, ap);
  printf("\x1b[m\n");
  va_end(ap);
}
