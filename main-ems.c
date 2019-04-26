/** main - entry point (for Emscripten)
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

#include "c8r.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char* CR = "\n";
const char* null = "null\n";

void evaluate(const char* line)
{
  struct c8ctx* ctx = c8ctx_create();
  c8mpfr_init_ctx(ctx);
  c8mpz_init_ctx(ctx);
  struct c8val* eval = c8eval_create(ctx);

  struct c8obj* r = c8eval_expr(eval, line);
  if (r) {
    struct c8buf rs; c8buf_init(&rs);
    c8obj_str(r, &rs, C8_FMT_DEC);
    write(fileno(stdout), c8buf_str(&rs), c8buf_len(&rs));
    write(fileno(stdout), CR, strlen(CR));
    c8buf_clear(&rs);
    c8obj_unref(r);
  } else {
    write(fileno(stdout), null, strlen(null));
  }

  c8eval_destroy(eval);
  c8ctx_destroy(ctx);
}

int main(int argc, char* argv[])
{
  return 0;
}
