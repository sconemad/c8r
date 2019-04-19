/** c8script - script parser
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

#include "c8script.h"
#include "c8stmt.h"
#include "c8expr.h"
#include "c8group.h"
#include "c8cond.h"
#include "c8loop.h"
#include "c8obj.h"
#include "c8eval.h"
#include "c8buf.h"
#include "c8vec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

struct c8script {
  struct c8buf script;
  const char* pos;
  struct c8buf token;
  int tokenid;
  struct c8vec stack;
  struct c8eval* eval;
};

static int next(struct c8script* o);

struct c8script* c8script_create()
{
  struct c8script* o = malloc(sizeof(struct c8script));
  assert(o);
  c8buf_init(&o->script);
  c8buf_init(&o->token);
  c8vec_init(&o->stack);
  o->eval = c8eval_create(0);
  return o;
}

void c8script_destroy(struct c8script* o)
{
  assert(o);
  c8buf_clear(&o->script);
  c8buf_clear(&o->token);
  for (int i=0; i<c8vec_size(&o->stack); ++i) {
    struct c8stmt* s = (struct c8stmt*)c8vec_at(&o->stack, i);
    c8stmt_destroy(s);
  }
  c8vec_clear(&o->stack);
  c8eval_destroy(o->eval);
  free(o);
}

int c8script_parse(struct c8script* o, const char* script)
{
  assert(o);
  c8buf_init_str(&o->script, script);
  o->pos = c8buf_str(&o->script);

  struct c8stmt* root = (struct c8stmt*)c8group_create();
  c8vec_push_back(&o->stack, root);

  while (1) {
    if (!next(o)) break;
    //printf("TOK: %s\n", c8buf_str(&o->token));
    while (1) {
      struct c8stmt* cur = (struct c8stmt*)c8vec_at(&o->stack, -1);
      int pr = c8stmt_parse(cur, o, c8buf_str(&o->token));
      if (pr == C8_PARSERESULT_POP || pr == C8_PARSERESULT_END) {
	if (c8vec_size(&o->stack) == 0) {
	  printf("c8script: parse stack underflow\n");
	  return 2;
	}
	c8vec_pop_back(&o->stack);
      }
      if (pr == C8_PARSERESULT_ERROR) {
	printf("c8script: syntax error\n");
	return 1;
      }
      if (pr != C8_PARSERESULT_POP) {
	break;
      }
    }
  }
  return 0;
}

struct c8obj* c8script_run(struct c8script* o)
{
  assert(o);
  assert(c8vec_size(&o->stack) > 0);
  struct c8stmt* root = (struct c8stmt*)c8vec_at(&o->stack, 0);
  int flow = C8_FLOW_NORMAL;
  return c8stmt_run(root, o, &flow);
}

#define C8_PARSETOKEN_UNKNOWN 0
#define C8_PARSETOKEN_IF 1
#define C8_PARSETOKEN_ELSE 2
#define C8_PARSETOKEN_WHILE 3
#define C8_PARSETOKEN_FOR 4
#define C8_PARSETOKEN_RETURN 5
#define C8_PARSETOKEN_BREAK 6
#define C8_PARSETOKEN_CONTINUE 7
#define C8_PARSETOKEN_VAR 8
#define C8_PARSETOKEN_SUB 9
#define C8_PARSETOKEN_OPEN_BRACE 13
#define C8_PARSETOKEN_CLOSE_BRACE 14

#define RET_IF_EQUAL(str,val,ret) if (strcmp(str,val)==0) return ret;

static int get_tokenid(const char* token)
{
  RET_IF_EQUAL(token, "if", C8_PARSETOKEN_IF);
  RET_IF_EQUAL(token, "else", C8_PARSETOKEN_ELSE);
  RET_IF_EQUAL(token, "while", C8_PARSETOKEN_WHILE);
  RET_IF_EQUAL(token, "for", C8_PARSETOKEN_FOR);
  RET_IF_EQUAL(token, "return", C8_PARSETOKEN_RETURN);
  RET_IF_EQUAL(token, "break", C8_PARSETOKEN_BREAK);
  RET_IF_EQUAL(token, "continue", C8_PARSETOKEN_CONTINUE);
  RET_IF_EQUAL(token, "var", C8_PARSETOKEN_VAR);
  RET_IF_EQUAL(token, "sub", C8_PARSETOKEN_SUB);
  RET_IF_EQUAL(token, "{", C8_PARSETOKEN_OPEN_BRACE);
  RET_IF_EQUAL(token, "}", C8_PARSETOKEN_CLOSE_BRACE);
  return C8_PARSETOKEN_UNKNOWN;
}

struct c8stmt* c8script_parse_token(struct c8script* o, const char* token)
{
  assert(o);
  int tid = get_tokenid(token); 
  struct c8stmt* s = 0;
  switch (tid) {
  case C8_PARSETOKEN_IF: 
    s = (struct c8stmt*)c8cond_create(); break;
  case C8_PARSETOKEN_WHILE:
    s = (struct c8stmt*)c8loop_create(); break;
  case C8_PARSETOKEN_FOR: 
    s = (struct c8stmt*)c8loop_create(); break;
  case C8_PARSETOKEN_RETURN: 
    s = (struct c8stmt*)0; /*C8_FLOW_RETURN*/ break;
  case C8_PARSETOKEN_BREAK:
    s = (struct c8stmt*)0; /*C8_FLOW_LAST*/ break;
  case C8_PARSETOKEN_CONTINUE:
    s = (struct c8stmt*)0; /*C8_FLOW_NEXT*/ break;
  case C8_PARSETOKEN_VAR:
    s = (struct c8stmt*)0; break;
  case C8_PARSETOKEN_SUB:
    s = (struct c8stmt*)0; break;
  case C8_PARSETOKEN_OPEN_BRACE: 
    s = (struct c8stmt*)c8group_create(); break;
  case C8_PARSETOKEN_UNKNOWN: 
    s = (struct c8stmt*)c8expr_create(token); break;
  }
  if (s && tid != C8_PARSETOKEN_UNKNOWN) c8vec_push_back(&o->stack, s);
  return s;
}

static int next(struct c8script* o)
{
  //printf("next: ");
  // Reset
  c8buf_clear(&o->token);
  o->tokenid = C8_PARSETOKEN_UNKNOWN;

  // Determine parse mode of current statement
  struct c8stmt* cs = (struct c8stmt*)c8vec_at(&o->stack,-1);
  int pm = c8stmt_parse_mode(cs);
  
  // Skip initial whitespace
  while (*o->pos && isspace(*o->pos)) ++o->pos;
  if (!*o->pos) {
    //printf("END\n");
    return 0;
  }

  int len = 0;
  int skip = 0;
  char in_quote = 0; // Holds the current quote char (" or ') when in quote
  int escape = 0; // Was the last char an escape (\)
  int in_comment = 0;
  int in_bracket = 0;
  //  int first = 0;

  for (const char* c=o->pos; *c; ++c) {
    if (pm == C8_PARSEMODE_STATEMENT) {

      if ((*c == ';') || (*c == '(') || isspace(*c)) {
	//XXX strncmp?
	c8buf_clear(&o->token);
	c8buf_append_strn(&o->token, o->pos, len);
	o->tokenid = get_tokenid(c8buf_str(&o->token));
	if (o->tokenid != C8_PARSETOKEN_UNKNOWN) break;
      }

      if (in_quote) {
	// Ignore everything until (unescaped) end quote
	if (*c == in_quote && !escape) in_quote = 0;
	// Check for escape
	escape = (*c == '\\');
	
      } else if (in_comment) {
	// Ignore everything until end of line
	while (*c == '\r' || *c == '\n') {
	  in_comment = 1;
	  ++skip;
	  ++c;
	  if (!*c) break;
	}
	if (!in_comment) break;

      } else if (*c == '"' || *c == '\'') {
	in_quote = *c;
      } else if (*c == '#') {
	in_comment = 1;
      } else if (*c == ';') {
	++skip;
	break;
      }

    } else if (pm == C8_PARSEMODE_BRACKETED) {

      if (in_quote) {
	// Ignore everything until (unescaped) end quote
	if (*c == in_quote && !escape) in_quote = 0;
	// Check for escape
	escape = (*c == '\\');

      } else if (*c == '"' || *c == '\'') {
        in_quote = *c;
      } else if (*c == '(') {
        if (in_bracket == 0) {
          ++o->pos;
          --len;
        }
        ++in_bracket;
      } else if (*c == ')') {
        --in_bracket;
        if (in_bracket == 0) {
          ++skip;
          break;
        }
      }
      
    } else if (pm == C8_PARSEMODE_NAME) {

    }

    ++len;
  }

  c8buf_clear(&o->token);
  c8buf_append_strn(&o->token, o->pos, len);
  o->pos += len + skip;
  
  //printf("%s\n", c8buf_str(&o->token));
  return 1;
}

struct c8eval* c8script_eval(struct c8script* o)
{
  assert(o);
  return o->eval;
}
