/*  Coypright (C) 2011 Christopher Allen Ogden
 *
 *     This file is part of SydMine.
 *
 *  SydMine is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SydMine is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SydMine.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <iostream>

#define LOG_DEBUG std::cout
#define LOG_INFO std::cout
#define LOG_NOTICE std::cout
#define LOG_WARNING std::cout
#define LOG_ERROR std::cerr
#define LOG_CRITICAL std::cerr
#define LOG_ALERT std::cerr
#define LOG_EMERGENCY std::cerr
