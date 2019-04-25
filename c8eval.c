/** c8eval - expression evaluator
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

#include "c8eval.h"
#include "c8buf.h"
#include "c8obj.h"
#include "c8ops.h"
#include "c8error.h"
#include "c8bool.h"
#include "c8string.h"
#include "c8list.h"
#include "c8map.h"
#include "c8mpz.h"
#include "c8mpfr.h"
#include "c8ctx.h"

#define _GNU_SOURCE
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define C8_TOKEN_NULL 0
#define C8_TOKEN_OP 1
#define C8_TOKEN_NAME 2
#define C8_TOKEN_VALUE 3

struct c8eval {
  struct c8ctx* global;
  c8eval_resolver_func resolver;
  void* resolver_data;
  const char* pos;
  int type;
  int binary_op;
  int prefix_op;
  int postfix_op;
  struct c8buf name;
  struct c8obj* value;
  struct c8list* list;
};

static struct c8obj* expression(struct c8eval* o, int p, int f, int ex);
static struct c8obj* primary(struct c8eval* o, int f, int ex);
static struct c8obj* list(struct c8eval* o, int lop, int ex);
static struct c8obj* resolve(struct c8eval* o, const char* name);

static void next(struct c8eval* o);

static struct c8obj* expression(struct c8eval* o, int p, int f, int ex)
{
  struct c8obj* left = primary(o, f, ex);

  while (1) {
    if (C8_TOKEN_NULL == o->type) {
      break;

    } else if (C8_TOKEN_OP == o->type) {
      int op = o->binary_op;
      f = (C8_OP_LOOKUP != op);
      int p2 = c8ops_prec(op);
      if (p2 > 0) {
        // Binary op
        if (p2 < p) break;
        
        if (C8_OP_SUBSCRIPT == op) {
          // Subscript op
          struct c8obj* right = expression(o, 0, 1, ex);
          if (0 == right) {
            o->type = C8_TOKEN_NULL;
            c8obj_unref(left);
            return right;
          }
          struct c8obj* new_left = c8obj_op(left, op, right);
          c8obj_unref(left);
          c8obj_unref(right);
          left = new_left;

          // Check for end
          if (C8_TOKEN_OP != o->type ||
              C8_OP_SUBSCRIPT_END != o->binary_op) {
            o->type = C8_TOKEN_NULL;
            c8obj_unref(left);
            return 0;
          }
          next(o);

        } else if (C8_OP_LIST == op) { // argument list          
          struct c8obj* r = list(o, op, ex);
          struct c8list* lr = to_c8list(r);
          if (!lr) {
            c8obj_unref(left);
            return r;
          }
          next(o);
          
          // Call function
          struct c8obj* new_left = c8obj_op(left, op, r);
          c8obj_unref(left);
          c8obj_unref(r);
          left = new_left;

        } else {
          struct c8obj* right = 0;

          if (C8_OP_LOGIC_OR == op && c8obj_int(left)) {
            // Short circuit || - don't exec rhs if lhs is true
            right = expression(o, p2+1, f, 0);
            c8obj_unref(right);
            return left;

          } else if (C8_OP_LOGIC_AND == op && !c8obj_int(left)) {
            // Short circuit && - don't exec rhs if lhs is false
            right = expression(o, p2+1, f, 0);
            c8obj_unref(right);
            return left;
            
          } else {
            right = expression(o, p2+1, f, ex);
          }

          if (0 == right) {
            o->type = C8_TOKEN_NULL;
            c8obj_unref(left);
            return 0;
          }

          if (C8_OP_SEQUENTIAL == op || C8_OP_RESOLVE == op) {
            if (o->list) c8list_push_back(o->list, left);
            c8obj_unref(left);
            left = right;
          } else { // normal binary op
            struct c8obj* new_left = c8obj_op(left, op, right);
            c8obj_unref(left);
            c8obj_unref(right);
            left = new_left;
          }
        }
        if (0 == left) {
          o->type = C8_TOKEN_NULL;
          return 0;
        }
      
      } else if (c8ops_prec(o->postfix_op) >= 0) { // Unary postfix
        struct c8obj* new_left = c8obj_op(left, o->postfix_op, 0);
        c8obj_unref(left);
        left = new_left;
        next(o);
      
      } else {
        break;
      }

    } else {
      o->type = C8_TOKEN_NULL;
      return left;
    }
  }

  return left;
}

static struct c8obj* primary(struct c8eval* o, int f, int ex)
{
  next(o);

  switch (o->type) {
    case C8_TOKEN_OP: {
      int op = C8_OP_UNKNOWN;

      // Check for unary prefix op
      op = o->prefix_op;
      int p2 = c8ops_prec(op);
      if (p2 >= 0) {
        struct c8obj* right = expression(o, p2, f, ex);
        if (right) {
          struct c8obj* result = 0;
          if (ex) result = c8obj_op(right, op, 0);
          c8obj_unref(right);
          return result;

        } else {
          // Special case: !NULL should return true
          if (C8_OP_LOGIC_NOT == op)
            return (struct c8obj*)c8bool_create(1);
          return 0;
        }
      }

      // Binary operation
      op = o->binary_op;

      // Sub-expression
      if (C8_OP_LIST == op) {
        struct c8obj* r = expression(o, 0, 1, ex);

        // Check for end
        if (o->binary_op != C8_OP_LIST_END) {
          o->type = C8_TOKEN_NULL;
          c8obj_unref(r);
          return (struct c8obj*)c8error_create(C8_ERROR_PARENTHESIS);
        }
        next(o);
        return r;
      }

      // List initializer
      if (C8_OP_SUBSCRIPT == op) {
        struct c8obj* r = list(o, op, ex);
        struct c8list* lr = to_c8list(r);
        if (!lr) return r;
        next(o);
        return (struct c8obj*)lr;
      }

      if (C8_OP_MAP == op) {
        struct c8obj* r = list(o, op, ex);
        struct c8list* lr = to_c8list(r);
        if (!lr) return r;
        
        // Construct map from list
        int s = c8list_size(lr); 
        if (s%2 != 0) {
          // Even number of values required
          o->type = C8_TOKEN_NULL;
          c8obj_unref((struct c8obj*)lr);
          return (struct c8obj*)c8error_create(C8_ERROR_MAP_INIT);
        }
        struct c8map* map = c8map_create();
        for (int i=0; i<s; i+=2) {
          struct c8buf key; c8buf_init(&key);
          struct c8obj* ko = c8list_at(lr, i);
          if (ko) {
            c8obj_str(ko, &key, 0);
            struct c8obj* value = c8list_at(lr, i+1);
            c8map_set(map, c8buf_str(&key), value);
            c8obj_unref(ko);
            c8obj_unref(value);
          }
          c8buf_clear(&key);
        }
        c8obj_unref((struct c8obj*)lr);
        
        next(o);
        return (struct c8obj*)map;
      }
    } break;

    case C8_TOKEN_VALUE: {
      struct c8obj* r = o->value;
      o->value = 0;
      next(o);
      return r;
    }

    case C8_TOKEN_NAME: {
      struct c8obj* r = f ?
        resolve(o, c8buf_str(&o->name)) :
        (struct c8obj*)c8string_create_buf(&o->name);
      if (r) {
        next(o);
        return r;
      }
      o->type = C8_TOKEN_NULL;
      return 0;
    }
  }
  
  o->type = C8_TOKEN_NULL;    
  return 0;
}

static struct c8obj* list(struct c8eval* o, int lop, int ex)
{
  struct c8list* plist = o->list;
  o->list = c8list_create();
  struct c8obj* q = expression(o, 0, 1, ex);
  c8list_push_back(o->list, q);
  c8obj_unref(q);
  struct c8list* ls = o->list;
  o->list = plist;

  if (c8list_size(ls) == 1 && q == 0 && o->binary_op == lop+1) {
    // Empty list
    c8obj_unref((struct c8obj*)ls);
    ls = c8list_create();
    
  } else if (C8_TOKEN_NULL == o->type) {
    // Error in argument list
    c8obj_unref((struct c8obj*)ls);
    return (struct c8obj*)c8error_create(C8_ERROR_LIST_INIT);
    
  } else if (o->type != C8_TOKEN_OP || o->binary_op != lop+1) {
    c8obj_unref((struct c8obj*)ls);
    return (struct c8obj*)c8error_create(C8_ERROR_LIST_INIT);
  }

  return (struct c8obj*)ls;
}

static struct c8obj* resolve(struct c8eval* o, const char* name)
{
  // Standard names which can't be overridden
  if (strcmp("null", name)==0) return 0;
  if (strcmp("true", name)==0) return (struct c8obj*)c8bool_create(1);
  if (strcmp("false", name)==0) return (struct c8obj*)c8bool_create(0);

  // Resolve using supplied resolver function
  struct c8obj* obj = 0;
  if (o->resolver) obj = (o->resolver)(name, o->resolver_data);
  if (obj) return obj;

  // Look in global context
  if (o->global) obj = c8ctx_resolve(o->global, name);
  if (obj) return obj;

  return (struct c8obj*)c8error_create(C8_ERROR_UNDEFINED_NAME);
}

// Maximum (non-alpha) operator length
#define MAXOP 2

static void next(struct c8eval* o)
{
  // Reset
  o->type = C8_TOKEN_NULL;
  o->binary_op = C8_OP_UNKNOWN;
  o->prefix_op = C8_OP_UNKNOWN;
  o->postfix_op = C8_OP_UNKNOWN;
  c8buf_clear(&o->name);
  o->value = 0;
  
  // Skip initial whitespace
  while (*o->pos&& isspace(*o->pos)) ++o->pos;
  if (!*o->pos) return;

  int len = strlen(o->pos);
  const char* c = o->pos;

  // Symbolic operators
  if (ispunct(*c)) {
    int maxop = MAXOP; 
    if (len < maxop) maxop=len;
    char opstr[MAXOP+1];
    memcpy(opstr, o->pos, maxop);
    for (int ol=maxop; ol>0; --ol) {
      opstr[ol] = 0;
      o->binary_op = c8ops_binary(opstr);
      o->prefix_op = c8ops_prefix(opstr);
      o->postfix_op = c8ops_postfix(opstr);
      if (o->binary_op != C8_OP_UNKNOWN ||
          o->prefix_op != C8_OP_UNKNOWN ||
          o->postfix_op != C8_OP_UNKNOWN) {
        c8buf_init_str(&o->name, opstr);
        o->pos += ol;
        o->type = C8_TOKEN_OP;
        return;
      }
    }
  }

  // Numeric
  if (isdigit(*c)) {
    int ends = 1;
    int ex = 0; // counts exponentiations
    int dd = 0; // counts decimal points
    int hex = 0;

    char pc = *c; // previous character

    for (; *c; ++c) {
      ends = !isalnum(*c);
      switch (*c) {
        case '.':
          ends = 0;
          ++dd;
          break;
        case 'x': case 'X':
          ends = (++hex>1);
          break;
        case 'a': case 'A':
        case 'b': case 'B':
        case 'c': case 'C':
        case 'd': case 'D':
        case 'f': case 'F':
          if (hex==1) ends = 0;
          break;
        case 'e': case 'E':
          if (hex==1) ends=0;
          else ends = (++ex>1);
          break;
        case 'p': case 'P':
          if (hex==0) ends=1;
          else ends = (++ex>1);
          break;
        case 'o': case 'O':
          ends = 0;
          break;
        case '+': case '-':
          ends = (pc != 'e' && pc != 'E');
          break;
      }
      if (ends) break;
      pc = *c;
    }

    c8buf_append_strn(&o->name, o->pos, c - o->pos);
    if (dd || ex) {
      o->value = (struct c8obj*)c8mpfr_create_str(c8buf_str(&o->name));
    } else {
      o->value = (struct c8obj*)c8mpz_create_str(c8buf_str(&o->name));
    }
    o->pos = c;
    o->type = C8_TOKEN_VALUE;
    return;
  }

  // Alpha
  if (isalpha(*c) || '_' == *c) {
    for (; *c; ++c) if (!(isalnum(*c) || '_' == *c)) break;
    c8buf_append_strn(&o->name, o->pos, c - o->pos);
    o->pos = c;
    // Check for alpha operator
    o->binary_op = c8ops_binary(c8buf_str(&o->name));
    o->prefix_op = c8ops_prefix(c8buf_str(&o->name));
    o->postfix_op = c8ops_postfix(c8buf_str(&o->name));
    if (o->binary_op != C8_OP_UNKNOWN ||
        o->prefix_op != C8_OP_UNKNOWN ||
        o->postfix_op != C8_OP_UNKNOWN) {
      o->type = C8_TOKEN_OP;
    } else {
      o->type = C8_TOKEN_NAME;
    }
    return;
  }

  // Quoted string
  if ('"' == *c || '\'' == *c) {
    char delim = *c;
    ++c;
    for (; *c; ++c) {
      if (delim == *c) break;
      //TODO: escapes
      c8buf_append_strn(&o->name, c, 1);
    }
    if (*c) o->pos = ++c;
    o->value = (struct c8obj*)c8string_create_buf(&o->name);
    o->type = C8_TOKEN_VALUE;
    return;
  }
}

struct c8eval* c8eval_create(struct c8ctx* global)
{
  struct c8eval* o = malloc(sizeof(struct c8eval));
  assert(o);
  o->global = global;
  o->resolver = 0;
  o->resolver_data = 0;
  return o;
}

void c8eval_destroy(struct c8eval* o)
{
  assert(o);
  free(o);
}

struct c8obj* c8eval_expr(struct c8eval* o, const char* expr)
{
  o->pos = expr;
  o->type = C8_TOKEN_NULL;
  c8buf_init(&o->name);
  o->value = 0;
  o->list = 0;
  struct c8obj* ro = expression(o, 0, 1, 1);
  c8buf_clear(&o->name);
  c8obj_unref((struct c8obj*)o->value);
  c8obj_unref((struct c8obj*)o->list);
  return ro;
}

void c8eval_set_resolver(struct c8eval* o, c8eval_resolver_func resolver, 
                         void* data)
{
  assert(o);
  o->resolver = resolver;
  o->resolver_data = data;
}

c8eval_resolver_func c8eval_get_resolver(struct c8eval* o, void** data)
{
  assert(o);
  *data = o->resolver_data;
  return o->resolver;
}
