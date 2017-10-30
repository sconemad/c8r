/** main - entry point
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
#include <readline/readline.h>
#include <readline/history.h>

int main(int argc, char* argv[])
{
  struct c8buf hf;
  c8buf_init_str(&hf, getenv("HOME"));
  c8buf_append_str(&hf, "/.c8r_history");
  read_history(c8buf_str(&hf));
  rl_bind_key ('\t', rl_abort);

  struct c8ctx* ctx = c8ctx_create();
  c8mpfr_init_ctx(ctx);
  c8mpz_init_ctx(ctx);
  
  printf("c8r %d.%d.%d / %s\n%s\n\n",
         C8_VERSION_MAJOR, C8_VERSION_MINOR, C8_VERSION_PATCH,
         C8_WEBSITE, C8_COPYRIGHT);

  struct c8expr* expr = c8expr_create(ctx);
  int fmt = C8_FMT_DEC;

  while (1) {
    char* line = readline("c8r> ");
    if (line==0 || strcmp(line, "exit") == 0) break;
    if (line[0] == 0) continue;

    int f = -1;
    if (strcmp(line, "dec") == 0) f = C8_FMT_DEC;
    if (strcmp(line, "bin") == 0) f = C8_FMT_BIN;
    if (strcmp(line, "oct") == 0) f = C8_FMT_OCT;
    if (strcmp(line, "hex") == 0) f = C8_FMT_HEX;
    if (strcmp(line, "sci") == 0) f = C8_FMT_SCI;
    if (strcmp(line, "fix") == 0) f = C8_FMT_FIX;
    if (f >= 0) { fmt = f; continue; }
    
    add_history(line);
    struct c8obj* r = c8expr_eval(expr, line);
    if (r) {
      struct c8buf rs; c8buf_init(&rs);
      c8obj_str(r, &rs, fmt);
      printf("%s\n", c8buf_str(&rs));
      c8buf_clear(&rs);
      c8obj_unref(r);
    } else {
      printf("null\n");
    }
    free(line);
  }

  printf("\n");
  write_history(c8buf_str(&hf));
  c8buf_clear(&hf);
  c8expr_destroy(expr);
  c8ctx_destroy(ctx);
  return 0;
}
