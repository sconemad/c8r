/** c8stmt - script statements
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

struct c8stmt;
struct c8script;
struct c8exec;

void c8stmt_destroy(struct c8stmt* o);

#define C8_PARSE_CONTINUE 0
#define C8_PARSE_END      1
#define C8_PARSE_POP      2
#define C8_PARSE_ERROR    3
int c8stmt_parse(struct c8stmt* o, struct c8script* script, 
		 const char* token);

#define C8_PARSEMODE_STATEMENT 0
#define C8_PARSEMODE_BRACKETED 1
#define C8_PARSEMODE_NAME      2
int c8stmt_parse_mode(struct c8stmt* o);

void c8stmt_set_parent(struct c8stmt* o, struct c8stmt* p);

#define C8_RUN_NORMAL 0
#define C8_RUN_ERROR  1
#define C8_RUN_RETURN 2
#define C8_RUN_LAST   3
#define C8_RUN_NEXT   4
int c8stmt_run(struct c8stmt* o, struct c8script* script);
