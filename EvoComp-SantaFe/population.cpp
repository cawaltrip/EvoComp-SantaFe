/*
 * population.cpp
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

#include "population.h"
#include <iostream> /* Logging/error reporting only */
#include <sstream>
#include <utility> /* std::swap */

Population::Population(size_t population_size, double mutation_rate,
					   double nonterminal_crossover_rate, 
					   size_t tournament_size, size_t depth_min, 
					   size_t depth_max, std::vector<TrailMap> maps) {
	/** @todo	Triple check everything here is correct! */
	maps_ = maps;
	mutation_rate_ = mutation_rate;
	nonterminal_crossover_rate_ = nonterminal_crossover_rate;
	tournament_size_ = tournament_size;

	/** @todo	Default fitness values may need to be swapped. */
	best_fitness_ = DBL_MAX;
	worst_fitness_ = DBL_MIN;
	avg_fitness_ = 0;
	
	largest_tree_ = 0;
	smallest_tree_ = SIZE_MAX;
	avg_tree_ = 0;

	/* Generate the population */
	if (depth_min > depth_max) {
		std::swap(depth_min, depth_max);
	}

	/** @todo	Is forcing population to be even still necessary? */
	if (population_size % 2 != 0) {
		++population_size;
	}
	RampedHalfAndHalf(population_size, depth_min, depth_max);
	CalculateFitness();
}
void Population::Evolve(size_t elitism_count) {

}
std::string Population::ToString(bool include_fitness, bool latex) {
	std::string population_string;
	
	return population_string;
}