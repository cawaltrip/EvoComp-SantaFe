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
#include <algorithm> /* std::sort */
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

	best_fitness_ = DBL_MIN;
	worst_fitness_ = DBL_MAX;
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
	std::vector<Individual> evolved_pop(pop_.size());
	/* Elite individual selection uses raw fitness score. */
	std::sort(pop_.begin(), pop_.end());
	for (size_t i = 0; i < elitism_count; ++i) {
		evolved_pop[i] = pop_[i];
	}

	/* Non-elite individual selection uses weighted fitness score. */
	for (size_t i = elitism_count; i < evolved_pop.size(); ++i) {
		size_t p1 = SelectIndividual();
		size_t p2;
		do {
			p2 = SelectIndividual();
		} while (p2 == p1);

		Individual parent1(pop_[p1]);
		Individual parent2(pop_[p2]);
		Crossover(&parent1, &parent2);

		evolved_pop[i] = parent1;
		evolved_pop[i].Mutate(mutation_rate_);
	}
	this->pop_ = evolved_pop;
	CalculateFitness();
}
std::string Population::ToString(bool include_fitness, bool latex) {
	std::string population_string;
	
	return population_string;
}
std::string Population::BestSolutionToString(bool include_fitness,
											 bool latex) {
	std::stringstream ss;
	if (include_fitness) {
		ss << pop_[best_index_].GetFitness() << " ==> ";
	}
	ss << pop_[best_index_].ToString(latex);
	return ss.str();
}
std::string Population::BestWeightedToString(bool include_fitness,
											 bool latex) {
	std::stringstream ss;
	if (include_fitness) {
		ss << pop_[best_weighted_index_].GetFitness() << " ==> ";
	}
	ss << pop_[best_weighted_index_].ToString(latex);
	return ss.str();
}
size_t Population::GetLargestTreeSize() {
	return largest_tree_;
}
size_t Population::GetSmallestTreeSize() {
	return smallest_tree_;
}
size_t Population::GetAverageTreeSize() {
	return avg_tree_;
}
size_t Population::GetTotalNodeCount() {
	return total_nodes_;
}
double Population::GetBestFitness() {
	return best_fitness_;
}
double Population::GetWorstFitness() {
	return worst_fitness_;
}
double Population::GetAverageFitness() {
	return avg_fitness_;
}
double Population::GetBestWeightedFitness() {
	return best_weighted_fitness_;
}
double Population::GetWorstWeightedFitness() {
	return worst_weighted_fitness_;
}
double Population::GetAverageWeightedFitness() {
	return avg_weighted_fitness_;
}
void Population::RampedHalfAndHalf(size_t population_size, size_t depth_min, 
								   size_t depth_max) {
	unsigned gradations = static_cast<unsigned>(depth_max - depth_min + 1);
	pop_.reserve(population_size); /** @todo	Is reserve() needed? */
	bool full_tree;
	for (size_t i = 0; i < population_size; ++i) {
		if (i % 2) {
			full_tree = true;
		} else {
			full_tree = false;
		}
		pop_.emplace_back(Individual((depth_min + i % gradations), full_tree));
	}
}
void Population::Crossover(Individual *parent1, Individual *parent2) {
	std::uniform_real_distribution<double> d{ 0,1 };
	bool p1_nonterminal = ((d(GetEngine())) < nonterminal_crossover_rate_);
	bool p2_nonterminal = ((d(GetEngine())) < nonterminal_crossover_rate_);

	std::pair<Node*, size_t> c1 = parent1->GetRandomNode(p1_nonterminal);
	std::pair<Node*, size_t> c2 = parent2->GetRandomNode(p2_nonterminal);

	/* c1.first could be a nullptr in which case new individual is c2 */
	if (!c1.first->GetParent()) {
		c2.first->SetParent(nullptr);
		parent1->Erase();
		parent1->SetRootNode(c2.first);
	} else {
		Node *parent = c1.first->GetParent();
		c2.first->SetParent(parent);
		if (c1.first->GetChild(c1.second)) {
			parent->GetChild(c1.second)->Erase();
			parent->SetChild(c1.second, c2.first);
		} else {
			std::cerr << "Invalid child selected (" << c1.second << ")";
			std::cerr << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	parent1->CorrectTree();
}
size_t Population::SelectIndividual() {
	size_t winner;
	size_t challenger;
	
	std::uniform_int_distribution<size_t> d{ 0,pop_.size() - 1 };
	winner = d(GetEngine());
	
	for (size_t i = 0; i < tournament_size_; ++i) {
		do {
			challenger = d(GetEngine());
		} while (winner != challenger);
		if (pop_[challenger].GetWeightedFitness() < 
			pop_[winner].GetWeightedFitness()) {
			winner = challenger;
		}
	}
	return winner;
}
void Population::CalculateFitness() {
	CalculateWeightedFitness();
}
void Population::CalculateRawFitness() {
	double cur_fitness = 0;
	avg_fitness_ = 0;
	best_fitness_ = DBL_MIN;
	worst_fitness_ = DBL_MAX;

	for (size_t i = 0; i < pop_.size(); ++i) {
		pop_[i].CalculateFitness(maps_);
		cur_fitness = pop_[i].GetFitness();
		avg_fitness_ += cur_fitness;
		if (cur_fitness > best_fitness_) {
			best_fitness_ = cur_fitness;
			best_index_ = i;
		} else if (cur_fitness < worst_fitness_) {
			worst_fitness_ = cur_fitness;
		}
	}
	avg_fitness_ = avg_fitness_ / pop_.size();
}
void Population::CalculateWeightedFitness() {
	double parsimony_coefficient = CalculateParsimonyCoefficient();
	double cur_weighted_fitness = 0;
	avg_weighted_fitness_ = 0;
	best_weighted_fitness_ = DBL_MIN;
	worst_weighted_fitness_ = DBL_MAX;

	for (size_t i = 0; i < pop_.size(); ++i) {
		/* Fake the parsimony pressure for now */
		if (pop_[i].GetTreeSize() > 100) {
			parsimony_coefficient = 2;
		} else {
			parsimony_coefficient = 0;
		}
		pop_[i].CalculateWeightedFitness(parsimony_coefficient);
		cur_weighted_fitness = pop_[i].GetWeightedFitness();
		avg_weighted_fitness_ += cur_weighted_fitness;
		if (cur_weighted_fitness > best_weighted_fitness_) {
			best_weighted_fitness_ = cur_weighted_fitness;
			best_weighted_index_ = i;
		} else if (cur_weighted_fitness < worst_weighted_fitness_) {
			worst_weighted_fitness_ = cur_weighted_fitness;
		}
	}
	avg_weighted_fitness_ = avg_weighted_fitness_ / pop_.size();
}
double Population::CalculateParsimonyCoefficient() {
	double covariance = 0;
	double variance = 0;

	CalculateRawFitness();
	CalculateTreeSize();

	/** @todo	Calculate parsimony pressure correctly. */
	return 1.0f;
}
void Population::CalculateTreeSize() {
	size_t cur_tree = 0;
	avg_tree_ = 0;
	largest_tree_ = -1;
	smallest_tree_ = SIZE_MAX;

	for (auto p : pop_) {
		cur_tree = p.GetTreeSize();
		avg_tree_ += cur_tree;
		if (cur_tree > largest_tree_) {
			largest_tree_ = cur_tree;
		} else if (cur_tree < smallest_tree_) {
			smallest_tree_ = cur_tree;
		}
	}
	total_nodes_ = avg_tree_;
	avg_tree_ = avg_tree_ / pop_.size();
}
std::mt19937 &Population::GetEngine() {
	static std::random_device rd;
	static std::mt19937 mt(rd());
	return mt;
}