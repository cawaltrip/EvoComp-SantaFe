/*
 * trail_map.cpp
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

#include "trail_map.h"
#include <iostream> /* Logging/error reporting only */
/* Ant struct */
/**
 * @todo	Allow for Ant placement to be defined in map file.
 * @todo	Rename the TrailMap class and files to Ant or Solution.  All of 
 * the functions that other users will be calling are from the perspective 
 * of the Ant anyways.
 */
Ant::Ant() : x(0), y(0), direction(Direction::kEast) {}
void Ant::Reset() {
	x = 0; 
	y = 0;
	direction = Direction::kEast;
}
Cell::Cell(TrailData data) : data_(data), original_(data) {}
void Cell::Reset() {
	data_ = original_;
}

TrailMap::TrailMap(std::vector<std::string> map_file, size_t step_limit) : 
	current_action_count_(0), action_count_limit_(step_limit), 
	row_count_(map_file.size()), column_count_(0), ant_(Ant()) {

	for (std::string s : map_file) {
		if (s.length() > column_count_) {
			column_count_ = s.length();
		}
	}

	/* Parse the data */
	map_.resize(row_count_);
	for (size_t i = 0; i < row_count_; ++i) {
		map_[i].assign(column_count_, Cell(TrailData::kUnvisitedEmpty));
		for (size_t j = 0; j < column_count_; ++j) {
			map_[i][j] = Cell(ConvertCharToTrailData(map_file[i][j]));
		}
	}

	/* Count the number of food on the map */
	SetTotalFoodCount();
}
void TrailMap::Reset() {
	ant_.Reset();

	/* Reset the Map */
	for (size_t i = 0; i < row_count_; ++i) {
		for (size_t j = 0; j < column_count_; ++j) {
			map_[i][j].Reset();
		}
	}
	
	/* Reset food count totals */
	SetTotalFoodCount();
}
size_t TrailMap::GetTotalFoodCount() {
	return total_food_;
}
size_t TrailMap::GetConsumedFoodCount() {
	return consumed_food_;
}
void TrailMap::SetCell(size_t row, size_t column, TrailData data) {
	if (row < row_count_ && column < column_count_) {
		map_[row][column].data_ = data;
	}
}
TrailData TrailMap::GetCell(size_t row, size_t column) {
	/* Simplistic bounds enforcing/checking */
	return map_[row % row_count_][column % column_count_].data_;
}
void TrailMap::MoveForward() {
	/* Short-circuit execution if the ant is over the action limit. */
	if (!HasActionsRemaining()) {
		return;
	}
	/* Use modulus to wrap around the map. Easy bounds checking! */
	switch (ant_.direction) {
	case Direction::kNorth:
		ant_.y = (ant_.y - 1) % row_count_;
		break;
	case Direction::kEast:
		ant_.x = (ant_.x + 1) % column_count_;
		break;
	case Direction::kSouth:
		ant_.y = (ant_.y + 1) % row_count_;
		break;
	case Direction::kWest:
		ant_.x = (ant_.x - 1) % column_count_;
		break;
	default:
		std::cerr << "Ant cannot move in third dimension!" << std::endl;
		exit(EXIT_FAILURE);
	}
	++current_action_count_;
	if (GetCell(ant_.x, ant_.y) == TrailData::kUnvisitedFood) {
		SetCell(ant_.x, ant_.y, TrailData::kVisitedFood);
		++consumed_food_;
	} else {
		SetCell(ant_.x, ant_.y, TrailData::kVisitedEmpty);
	}
}
void TrailMap::TurnLeft() {
	/* Short-circuit execution if the ant is over the action limit. */
	if (!HasActionsRemaining()) {
		return;
	}
	switch (ant_.direction) {
	case Direction::kNorth:
		ant_.direction = Direction::kWest;
		break;
	case Direction::kEast:
		ant_.direction = Direction::kNorth;
		break;
	case Direction::kSouth:
		ant_.direction = Direction::kEast;
		break;
	case Direction::kWest:
		ant_.direction = Direction::kSouth;
		break;
	}
	++current_action_count_;
}
void TrailMap::TurnRight() {
	/* Short-circuit execution if the ant is over the action limit. */
	if (!HasActionsRemaining()) {
		return;
	}
	switch (ant_.direction) {
	case Direction::kNorth:
		ant_.direction = Direction::kEast;
		break;
	case Direction::kEast:
		ant_.direction = Direction::kSouth;
		break;
	case Direction::kSouth:
		ant_.direction = Direction::kWest;
		break;
	case Direction::kWest:
		ant_.direction = Direction::kNorth;
		break;
	}
	++current_action_count_;
}
bool TrailMap::IsFoodAhead() {
	size_t row = ant_.y;
	size_t column = ant_.x;
	switch (ant_.direction) {
	case Direction::kNorth:
		row = (row - 1) % row_count_;
		break;
	case Direction::kEast:
		column = (column + 1) % column_count_;
		break;
	case Direction::kSouth:
		row = (row + 1) % row_count_;
		break;
	case Direction::kWest:
		column = (column - 1) % column_count_;
		break;
	}
	if (GetCell(row, column) == TrailData::kUnvisitedFood) {
		return true;
	}
	return false;
}
bool TrailMap::HasActionsRemaining() {
	return current_action_count_ < action_count_limit_;
}
TrailData TrailMap::ConvertCharToTrailData(char c) {
	switch (c) {
	case '_':
	case '-':
		return TrailData::kUnvisitedEmpty;
	case 'X':
	case 'x':
		return TrailData::kUnvisitedFood;
	case '+':
		return TrailData::kVisitedEmpty;
	case '*':
		return TrailData::kVisitedFood;
	}
	return TrailData::kUnvisitedEmpty;
}
char TrailMap::ConvertTrailDataToChar(TrailData d) {
	switch (d) {
	case TrailData::kUnvisitedEmpty: 
		return '_';
	case TrailData::kUnvisitedFood: 
		return 'X';
	case TrailData::kVisitedEmpty: 
		return '+';
	case TrailData::kVisitedFood: 
		return '*';
	}
	return '?';
}
void TrailMap::SetTotalFoodCount() {
	total_food_ = 0;
	consumed_food_ = 0;
	for (std::vector<Cell> row : map_) {
		for (Cell d : row) {
			switch (d.data_) {
			case TrailData::kVisitedFood:
				++consumed_food_;
			case TrailData::kUnvisitedFood:
				++total_food_;
				break;
			}
		}
	}
}
std::string TrailMap::ToString(bool latex) {
	std::string printed_map;
	for (std::vector<Cell> row : map_) {
		for (Cell d : row) {
			printed_map += ConvertTrailDataToChar(d.data_);
		}
		printed_map += '\n';
	}
	printed_map.pop_back();
	return printed_map;
}