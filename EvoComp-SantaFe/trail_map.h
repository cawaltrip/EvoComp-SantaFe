/*
* trail_map.h
* Copyright (C) 2015 Chris Waltrip <walt2178@vandals.uidaho.edu>
*
* This file is part of EvoComp-SantaFe
*
* EvoComp-SantaFe is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* EvoComp-SantaFe is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with EvoComp-SantaFe.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
* Representation of the map that an Ant will attempt to traverse.
*
* @file
* @date 25 November 2015
*/
#pragma once

#include <vector>
/**
 * @class	TrailMap
 * A structure that represents a map that an Ant will traverse while
 * collecting food and the total amount of food on the map.
 * @todo	"Objectify"/"Classify" this and remove direct access to variables.
 */
struct TrailMap {
	/** The map itself, referencable directly. */
	std::vector<std::vector<char>> map_;
	/** The number of total food on the map. */
	size_t food_count_;
};