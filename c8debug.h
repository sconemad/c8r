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

#pragma once

#define C8_DEBUG_OFF 0
#define C8_DEBUG_ERROR 1
#define C8_DEBUG_WARNING 2
#define C8_DEBUG_INFO 3
#define C8_DEBUG_DETAIL 4

/** Set the logging level
 */
void c8debug_level(int enable);

/** Debug log
 */
void c8debug(int level, const char* msg, ...);

