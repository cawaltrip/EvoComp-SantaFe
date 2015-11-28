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
 * @enum	TrailData
 * Represents the possible values in a cell.
 */
enum TrailData {
	kUnvisitedEmpty, /**< An empty cell that hasn't been visited. */
	kUnvisitedFood, /**< A cell with uneaten food. */
	kVisitedEmpty, /**< An empty cell that's been visited. */
	kVisitedFood /**< A cell with food that's been eaten. */
};
/**
 * @class	TrailMap
 * A structure that represents a map that an Ant will traverse while
 * collecting food and the total amount of food on the map.
 */
class TrailMap {
public:
	/** 
	 * Constructor requires a 2D vector of characters.  This vector gets
	 * converted into `TrailData` items in the map.
	 */
	TrailMap(std::vector<std::vector<char>> map);
	/** Explicitly set a cell's contents. */
	void SetCell(size_t x, size_t y, TrailData contents);
	/** Retrieve the value at a given point.  Doesn't check bounds. */
	TrailData GetCell(size_t x, size_t y);
	/** Return the total number eaten and uneaten food on the map. */
	size_t GetTotalFoodCount();
private:
	std::vector<std::vector<TrailData>> map_;
	size_t total_food_;
};