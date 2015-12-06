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

TrailMap::TrailMap(std::vector<std::string> map_file_contents) {
	row_count_ = map_file_contents.size();
	column_count_ = 0;
	for (std::string s : map_file_contents) {
		if (s.length() > column_count_) {
			column_count_ = s.length();
		}
	}

	/* Parse the data */
	map_.resize(row_count_);
	for (size_t i = 0; i < row_count_; ++i) {
		map_[i].assign(column_count_, TrailData::kUnvisitedEmpty);
		for (size_t j = 0; j < column_count_; ++j) {
			map_[i][j] = ConvertCharToTrailData(map_file_contents[i][j]);
		}
	}
}
TrailData TrailMap::ConvertCharToTrailData(char c) {
	switch (c) {
	case '_':
	case '-':
		return TrailData::kUnvisitedEmpty;
		break;
	case 'X':
	case 'x':
		return TrailData::kUnvisitedFood;
		break;
	case 'A':
	case 'a':
		return TrailData::kAntPosition;
		break;
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
	case TrailData::kAntPosition: 
		return 'A';
	}
	return '?';
}
void TrailMap::SetTotalFoodCount() {
	total_food_ = 0;
	for (std::vector<TrailData> row : map_) {
		for (TrailData d : row) {
			switch (d) {
			case TrailData::kUnvisitedFood:
			case TrailData::kVisitedFood:
				++total_food_;
				break;
			}
		}
	}
}
size_t TrailMap::GetTotalFoodCount() {
	return total_food_;
}
void TrailMap::SetCell(size_t row, size_t column, TrailData data) {
	if (row < row_count_ && column < column_count_) {
		map_[row][column] = data;
	}
}
TrailData TrailMap::GetCell(size_t row, size_t column) {
	return map_[row][column];
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