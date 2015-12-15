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
Ant::Ant() : x(0), y(0), direction(Direction::kEast) {}

TrailMap::TrailMap(std::vector<std::string> map_file, size_t step_limit) : 
	current_steps_(0), step_limit_(step_limit), row_count_(map_file.size()), 
	column_count_(0), ant_(Ant()) {

	for (std::string s : map_file) {
		if (s.length() > column_count_) {
			column_count_ = s.length();
		}
	}

	/* Parse the data */
	map_.resize(row_count_);
	for (size_t i = 0; i < row_count_; ++i) {
		map_[i].assign(column_count_, TrailData::kUnvisitedEmpty);
		for (size_t j = 0; j < column_count_; ++j) {
			map_[i][j] = ConvertCharToTrailData(map_file[i][j]);
		}
	}
	/* Count the number of food on the map */
	SetTotalFoodCount();
}
size_t TrailMap::GetTotalFoodCount() {
	return uneaten_food_ + consumed_food_;
}
void TrailMap::SetCell(size_t row, size_t column, TrailData data) {
	if (row < row_count_ && column < column_count_) {
		map_[row][column] = data;
	}
}
TrailData TrailMap::GetCell(size_t row, size_t column) {
	return map_[row][column];
}
void TrailMap::MoveForward() {
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
	++current_steps_;
	if (GetCell(ant_.x, ant_.y) == TrailData::kUnvisitedFood) {
		SetCell(ant_.x, ant_.y, TrailData::kVisitedFood);
		++consumed_food_;
		--uneaten_food_;
	} else {
		SetCell(ant_.x, ant_.y, TrailData::kVisitedEmpty);
	}
}
void TrailMap::TurnLeft() {
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
	++current_steps_;
}
void TrailMap::TurnRight() {
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
	++current_steps_;
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
bool TrailMap::HasStepsRemaining() {
	return current_steps_ < step_limit_;
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
	uneaten_food_ = 0;
	consumed_food_ = 0;
	for (std::vector<TrailData> row : map_) {
		for (TrailData d : row) {
			switch (d) {
			case TrailData::kUnvisitedFood:
				++uneaten_food_;
				break;
			case TrailData::kVisitedFood:
				++consumed_food_;
				break;
			}
		}
	}
}
std::string TrailMap::ToString(bool latex) {
	std::string printed_map;
	for (std::vector<TrailData> row : map_) {
		for (TrailData d : row) {
			printed_map += ConvertTrailDataToChar(d);
		}
		printed_map += '\n';
	}
	printed_map.pop_back();
	return printed_map;
}