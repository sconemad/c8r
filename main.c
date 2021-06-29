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

#include "calcul8/calcul8.h"
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <getopt.h>
#include <sys/stat.h>

static struct c8ctx* ctx;
static struct c8script* script;
static struct c8eval* eval;

int print_usage(const char* pgm)
{
  printf("Usage: %s [OPTS]... [script]\n"
         "Options:\n"
         "  -v      print version info\n"
         "  -dN     use debug level N (0...4)\n"
         "\n", pgm);
  return 0;
}

int print_version()
{
  printf("calcul8r %d.%d.%d / %s\n%s\n\n",
         C8_VERSION_MAJOR, C8_VERSION_MINOR, C8_VERSION_PATCH,
         C8_WEBSITE, C8_COPYRIGHT);
  return 0;
}

int run_interactive()
{
  struct c8buf hf;
  c8buf_init_str(&hf, getenv("HOME"));
  c8buf_append_str(&hf, "/.config/calcul8r");
  mkdir(c8buf_str(&hf), 0700);
  c8buf_append_str(&hf, "/history");
  read_history(c8buf_str(&hf));

  print_version();

  int fmt = C8_FMT_DEC;

  while (1) {
    //    char* line = readline("\x1b[1;1mc8>\x1b[m ");
    char* line = readline("c8> ");
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
    struct c8obj* r = c8eval_expr(eval, line);
    if (r) {
      struct c8buf rs; c8buf_init(&rs);
      c8obj_str(r, &rs, fmt);
      printf("%s\n", c8buf_str(&rs));
      c8buf_clear(&rs);
      c8obj_unref(r);
    }
    free(line);
  }

  printf("\n");
  write_history(c8buf_str(&hf));
  c8buf_clear(&hf);
  return 0;
}

int run_script(const char* file)
{
  FILE* f = fopen(file, "r");
  if (!f) return 1;
  fseek(f, 0L, SEEK_END);
  long size = ftell(f);
  char* data = malloc(size+1);
  fseek(f, 0L, SEEK_SET);
  fread(data, 1, size ,f); 
  fclose(f);
  data[size] = 0;

  // parse
  c8debug(C8_DEBUG_INFO, "Parsing script (%d bytes)...", size);
  int ret = c8script_parse(script, data);
  if (ret != 0) {
    c8debug(C8_DEBUG_ERROR, "Parse error: %d", ret);
    return ret;
  }
  
  // run
  c8debug(C8_DEBUG_INFO, "Parsed ok, running script...");
  ret = c8script_run(script);
  free((void*)data);
  if (ret != 0) {
    c8debug(C8_DEBUG_ERROR, "Runtime error: %d", ret);
  }
  return ret;
}

struct c8obj* print(struct c8list* args)
{
  if (c8list_size(args) != 1) 
    return (struct c8obj*) c8error_create(C8_ERROR_ARGUMENT);
  struct c8obj* a = c8list_at(args, 0);
  struct c8buf m; c8buf_init(&m);
  c8obj_str(a, &m, C8_FMT_DEC);
  printf("%s\n", c8buf_str(&m));
  c8buf_clear(&m);
  c8obj_unref(a);
  return 0;
}

struct c8obj* run(struct c8list* args)
{
  if (c8list_size(args) != 1)
    return (struct c8obj*) c8error_create(C8_ERROR_ARGUMENT);
  struct c8obj* a = c8list_at(args, 0);
  struct c8buf m; c8buf_init(&m);
  c8obj_str(a, &m, C8_FMT_DEC);
  run_script(c8buf_str(&m));
  c8buf_clear(&m);
  c8obj_unref(a);
  return 0;
}

struct c8obj* debug(struct c8list* args)
{
  if (c8list_size(args) != 1) 
    return (struct c8obj*) c8error_create(C8_ERROR_ARGUMENT);
  struct c8obj* a = c8list_at(args, 0);
  c8debug_level(c8obj_int(a));
  c8obj_unref(a);
  return 0;
}

struct c8obj* test(struct c8list* args)
{
  if (c8list_size(args) < 1 || c8list_size(args) > 2)
    return (struct c8obj*) c8error_create(C8_ERROR_ARGUMENT);
  struct c8obj* cond = c8list_at(args, 0);
  if (c8obj_int(cond) == 0) {

    struct c8buf msg; c8buf_init(&msg);
    if (c8list_size(args) == 2) {
      struct c8obj* omsg = c8list_at(args, 1);
      c8obj_str(omsg, &msg, C8_FMT_DEC);
      c8obj_unref(omsg);
    }

    c8debug(C8_DEBUG_ERROR, "script:%d: Test failed: %s",
            c8script_current_line(script), c8buf_str(&msg));
    c8buf_clear(&msg);
    exit(1);
  }
  c8obj_unref(cond);
  return 0;
}

int main(int argc, char* argv[])
{
  int c;
  extern char* optarg;
  while ((c = getopt(argc, argv,"?vd:")) >= 0) {
    switch (c) {
    case '?': return print_usage(argv[0]);
    case 'v': return print_version();
    case 'd': c8debug_level(atoi(optarg)); break;
    }
  }

  ctx = c8ctx_create();
  c8string_init_ctx(ctx);
  c8mpfr_init_ctx(ctx);
  c8mpz_init_ctx(ctx);
  c8ctx_add(ctx, "print", (struct c8obj*)c8func_create(print));
  c8ctx_add(ctx, "run", (struct c8obj*)c8func_create(run));
  c8ctx_add(ctx, "debug", (struct c8obj*)c8func_create(debug));
  c8ctx_add(ctx, "test", (struct c8obj*)c8func_create(test));

  script = c8script_create(ctx);
  eval = c8script_eval(script);

  int ret = 0;
  if (optind < argc) {
    ret = run_script(argv[optind]);
  } else {
    ret = run_interactive();
  }

  c8script_destroy(script);
  c8ctx_destroy(ctx);
  return ret;
}
