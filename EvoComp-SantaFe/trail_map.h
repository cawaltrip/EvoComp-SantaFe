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
 * @enum	Direction
 * Represents the cardinal directions that the ant can be facing.
 */
enum class Direction {
	kNorth,
	kEast,
	kSouth,
	kWest
};
 /** 
 * @struct	Ant
 * Simple structure to represent location of the Ant at a given time.
 */
struct Ant {
	Ant();	/**< Constructor sets ant's start position and direction. */
	size_t x;	/**< The column number the ant is on. */
	size_t y;	/**< The row number the ant is on. */
	Direction direction; /**< The direction the ant is facing. */
};
/** 
 * @enum	TrailData
 * Represents the possible values in a `TrailMap` cell.
 */
enum class TrailData {
	kUnvisitedEmpty,	/**< An empty cell that hasn't been visited. */
	kUnvisitedFood,		/**< A cell with uneaten food. */
	kVisitedEmpty,		/**< An empty cell that's been visited. */
	kVisitedFood		/**< A cell with food that's been eaten. */
};
/**
 * @class	TrailMap
 * A class that represents a map that an Ant will traverse while collecting
 * food and the total amount of food on the map.  Also responsible for
 * traversing the map and updating the ant's position.
 */
class TrailMap {
public:
	/** 
	 * Constructor requires a vector of strings.  This data gets parsed
	 * and converted into `TrailData` items in the map.  Any rows that are
	 * uneven have blank unvisited cells appended to the end of them to make
	 * the map width uniform.
	 *
	 * @param[in]	map_file_contents	A vector where each index is a string
	 *									representing the line that was in the
	 *									map data file.
	 */
	TrailMap(std::vector<std::string> map_file, size_t step_limit);
	/** Explicitly set a cell's contents. Checks bounds. */
	void SetCell(size_t row, size_t column, TrailData data);
	/** Retrieve the value at a given point.  Doesn't check bounds. */
	TrailData GetCell(size_t row, size_t column);
	/** Return the total number of food on the map. */
	size_t GetTotalFoodCount();
	/** Return the map as a string.
	 *
	 * @param[in]	latex	Add formatting for placing in a LaTeX file.
	 *
	 * @return	`std::string` with one row per line representing the map.
	 *
	 * @todo	Add LaTeX wrappings.
	 */
	std::string ToString(bool latex);
	/** Moves the ant forward on the map */
	void MoveForward();
	/** Turns the ant left (anticlockwise) */
	void TurnLeft();
	/** Turns the ant right (clockwise) */
	void TurnRight();
	/** Returns the status of uneaten food being ahead of the ant */
	bool IsFoodAhead();
	/** Returns whether the ant has any steps left to take */
	bool HasStepsRemaining();
private:
	/** 
	 * Looks up what `TrailData` is represented by a certain character.  If 
	 * an invalid character is passed, the default is to return
	 * `TrailData::kUnvisitedEmpty`.
	 *
	 * @param[in]	c	Character to match.
	 *
	 * @return `TrailData` represented by the input.
	 */
	TrailData ConvertCharToTrailData(char c);
	/**
	 * Looks up what character represents a given `TrailData` item.  If an
	 * invalid `TrailData` parameter is passed, the default is to return '?'.
	 *
	 * @param[in]	d	`TrailData` to match.
	 *
	 * @return `char` representing the input.
	 */
	char ConvertTrailDataToChar(TrailData d);
	/**
	* Determine the total number of food in a given map.  This will only need
	* to be called once at the creation of the `TrailMap` object because the
	* food count is fixed.
	*/
	void SetTotalFoodCount();
	std::vector<std::vector<TrailData>> map_;
	size_t current_steps_;
	size_t step_limit_;
	size_t consumed_food_;
	size_t uneaten_food_;
	size_t row_count_;
	size_t column_count_;
	Ant ant_;

};