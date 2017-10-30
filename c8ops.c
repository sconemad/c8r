/** c8ops - c8obj operators
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

#include "c8ops.h"

#include <string.h>

#define RET_IF_OP(pattern,op) if (strcmp(str,pattern)==0) return op;

int c8ops_binary(const char* str)
{
  RET_IF_OP(",",  C8_OP_SEQUENTIAL);
  RET_IF_OP(":",  C8_OP_RESOLVE);
  RET_IF_OP("=",  C8_OP_ASSIGN);
  RET_IF_OP("+=", C8_OP_ADD_ASSIGN);
  RET_IF_OP("-=", C8_OP_SUBTRACT_ASSIGN);
  RET_IF_OP("*=", C8_OP_MULTIPLY_ASSIGN);
  RET_IF_OP("/=", C8_OP_DIVIDE_ASSIGN);
  RET_IF_OP("||", C8_OP_LOGIC_OR);
  RET_IF_OP("&&", C8_OP_LOGIC_AND);
  RET_IF_OP("|",  C8_OP_BIT_OR);
  RET_IF_OP("&",  C8_OP_BIT_AND);
  RET_IF_OP("xor", C8_OP_BIT_XOR);
  RET_IF_OP("<<", C8_OP_SHIFT_LEFT);
  RET_IF_OP(">>", C8_OP_SHIFT_RIGHT);
  RET_IF_OP("==", C8_OP_EQUALITY);
  RET_IF_OP("!=", C8_OP_INEQUALITY);
  RET_IF_OP(">",  C8_OP_GREATER);
  RET_IF_OP("<",  C8_OP_LESS);
  RET_IF_OP(">=", C8_OP_GREATER_OR_EQUAL);
  RET_IF_OP("<=", C8_OP_LESS_OR_EQUAL);
  RET_IF_OP("+",  C8_OP_ADD);
  RET_IF_OP("-",  C8_OP_SUBTRACT);
  RET_IF_OP("*",  C8_OP_MULTIPLY);
  RET_IF_OP("/",  C8_OP_DIVIDE);
  RET_IF_OP("%",  C8_OP_MODULUS);
  RET_IF_OP("^",  C8_OP_POWER);
  RET_IF_OP("[",  C8_OP_SUBSCRIPT);
  RET_IF_OP("]",  C8_OP_SUBSCRIPT_END);
  RET_IF_OP("(",  C8_OP_LIST);
  RET_IF_OP(")",  C8_OP_LIST_END);
  RET_IF_OP("{",  C8_OP_MAP);
  RET_IF_OP("}",  C8_OP_MAP_END);
  RET_IF_OP(".",  C8_OP_LOOKUP);
  return C8_OP_UNKNOWN;
}

int c8ops_prefix(const char* str)
{
  RET_IF_OP("+",  C8_OP_POSITIVE);
  RET_IF_OP("-",  C8_OP_NEGATIVE);
  RET_IF_OP("!",  C8_OP_LOGIC_NOT);
  RET_IF_OP("~",  C8_OP_BIT_NOT);
  RET_IF_OP("++", C8_OP_PRE_INC);
  RET_IF_OP("--", C8_OP_PRE_DEC);
  return C8_OP_UNKNOWN;
}

int c8ops_postfix(const char* str)
{
  RET_IF_OP("!",  C8_OP_FACTORIAL);
  RET_IF_OP("++", C8_OP_POST_INC);
  RET_IF_OP("--", C8_OP_POST_DEC);
  return C8_OP_UNKNOWN;
}

int c8ops_prec(int op)
{
  switch (op) {
    case C8_OP_SEQUENTIAL: return 1;
    case C8_OP_RESOLVE: return 1;

    case C8_OP_ASSIGN: return 2;
    case C8_OP_ADD_ASSIGN: return 2;
    case C8_OP_SUBTRACT_ASSIGN: return 2;
    case C8_OP_MULTIPLY_ASSIGN: return 2;
    case C8_OP_DIVIDE_ASSIGN: return 2;
      
    case C8_OP_LOGIC_OR: return 3;
    case C8_OP_LOGIC_AND: return 4;
    case C8_OP_BIT_OR: return 5;
    case C8_OP_BIT_XOR: return 6;
    case C8_OP_BIT_AND: return 7;

    case C8_OP_EQUALITY: return 8;
    case C8_OP_INEQUALITY: return 8;

    case C8_OP_SHIFT_LEFT: return 9;
    case C8_OP_SHIFT_RIGHT: return 9;
      
    case C8_OP_GREATER: return 10;
    case C8_OP_LESS: return 10;
    case C8_OP_GREATER_OR_EQUAL: return 10;
    case C8_OP_LESS_OR_EQUAL: return 10;

    case C8_OP_ADD: return 11;
    case C8_OP_SUBTRACT: return 11;
      
    case C8_OP_MULTIPLY: return 12;
    case C8_OP_DIVIDE: return 12;
    case C8_OP_MODULUS: return 12;
      
    case C8_OP_POWER: return 13;

    case C8_OP_FACTORIAL: return 14;
    case C8_OP_POST_INC: return 14;
    case C8_OP_POST_DEC: return 14;
      
    case C8_OP_POSITIVE: return 15;
    case C8_OP_NEGATIVE: return 15;
    case C8_OP_LOGIC_NOT: return 15;
    case C8_OP_BIT_NOT: return 15;
    case C8_OP_PRE_INC: return 15;
    case C8_OP_PRE_DEC: return 15;
      
    case C8_OP_SUBSCRIPT: return 16;
    case C8_OP_LIST: return 17;
    case C8_OP_MAP: return 18;
    case C8_OP_LOOKUP: return 19;
  }
  return -1;
}
