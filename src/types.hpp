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

#include <cstdint>

typedef int8_t mcByte;
typedef uint8_t mcUByte;
typedef int16_t mcShort;
typedef uint16_t mcUShort;
typedef int32_t mcInt;
typedef uint32_t mcUInt;
typedef int64_t mcLong;
typedef uint64_t mcULong;
typedef float mcFloat; // No standard way to guarantee? Bah!
typedef double mcDouble; // No standard way to guarantee
typedef mcUByte mcCommandType;
typedef mcLong mcLargest;

typedef mcDouble Location;
typedef mcFloat Rot;
