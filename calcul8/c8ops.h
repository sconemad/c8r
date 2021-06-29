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

#pragma once

/** Operator codes for use with c8obj op calls */

/** Unknown operator */
#define C8_OP_UNKNOWN 0

/* Binary operators: */
#define C8_OP_SEQUENTIAL 1
#define C8_OP_RESOLVE 2

#define C8_OP_ASSIGN 3
#define C8_OP_ADD_ASSIGN 4
#define C8_OP_SUBTRACT_ASSIGN 5
#define C8_OP_MULTIPLY_ASSIGN 6
#define C8_OP_DIVIDE_ASSIGN 7
      
#define C8_OP_LOGIC_OR 8
#define C8_OP_LOGIC_AND 9
#define C8_OP_BIT_OR 10
#define C8_OP_BIT_XOR 11
#define C8_OP_BIT_AND 12

#define C8_OP_EQUALITY 13
#define C8_OP_INEQUALITY 14

#define C8_OP_SHIFT_LEFT 15
#define C8_OP_SHIFT_RIGHT 16
      
#define C8_OP_GREATER 17
#define C8_OP_LESS 18
#define C8_OP_GREATER_OR_EQUAL 19
#define C8_OP_LESS_OR_EQUAL 20

#define C8_OP_ADD 21
#define C8_OP_SUBTRACT 22
      
#define C8_OP_MULTIPLY 23
#define C8_OP_DIVIDE 24
#define C8_OP_MODULUS 25
      
#define C8_OP_POWER 26

#define C8_OP_SUBSCRIPT 27
#define C8_OP_SUBSCRIPT_END 28
#define C8_OP_LIST 29
#define C8_OP_LIST_END 30
#define C8_OP_MAP 31
#define C8_OP_MAP_END 32
#define C8_OP_LOOKUP 33

/* Unary postfix operators: */
#define C8_OP_FACTORIAL 100
#define C8_OP_POST_INC 101
#define C8_OP_POST_DEC 102

/* Unary prefix operators: */
#define C8_OP_POSITIVE 200
#define C8_OP_NEGATIVE 201
#define C8_OP_LOGIC_NOT 202
#define C8_OP_BIT_NOT 203
#define C8_OP_PRE_INC 204
#define C8_OP_PRE_DEC 205

/** Get operator code from string for binary/prefix/postfix operators
 * Returns C8_OP_UNKNOWN if not recognised
 */
int c8ops_binary(const char* str);
int c8ops_prefix(const char* str);
int c8ops_postfix(const char* str);

/** Get precedence value for an operator
 */
int c8ops_prec(int op);
