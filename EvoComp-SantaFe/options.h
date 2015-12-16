/*
* options.h
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
* @file
* @date 15 December 2015
*/
#pragma once
#include <vector>

/**
 * @struct	Options
 * Holds the options for the genetic program.  This is used to pass between 
 * the option parser and the main function for organization and cleanliness.
 */
struct Options {
	size_t evolution_count_;
	size_t elitism_count_;
	size_t population_size_;
	size_t action_count_limit_;
	double mutation_rate_;
	double nonterminal_crossover_rate_;
	double proportional_tournament_rate_;
	size_t tournament_size_;
	size_t tree_depth_min_;
	size_t tree_depth_max_;
	std::string primary_map_file_;
	std::vector<std::string> map_test_suite_files_;
	std::vector<std::string> verification_map_files_;

	bool primary_map_exists_;
	bool verification_maps_exist_;
};