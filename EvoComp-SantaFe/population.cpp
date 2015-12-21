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
					   size_t tournament_size, 
					   double proportional_tournament_rate, size_t depth_min, 
					   size_t depth_max, std::vector<TrailMap*> maps) {
	maps_ = maps;
	mutation_rate_ = mutation_rate;
	nonterminal_crossover_rate_ = nonterminal_crossover_rate;
	tournament_size_ = tournament_size;
	proportional_tournament_rate_ = proportional_tournament_rate;

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
	RampedHalfAndHalf(population_size, depth_min, depth_max);
	CalculateFitness();
}
Population::Population(Options opts, std::vector<TrailMap*> maps) : 
	Population(opts.population_size_, opts.mutation_rate_, 
			   opts.nonterminal_crossover_rate_, opts.tournament_size_, 
			   opts.proportional_tournament_rate_, opts.tree_depth_min_, 
			   opts.tree_depth_max_, maps) {}
Population::Population(const Population &copy, 
					   std::vector<TrailMap*> new_maps) {
	maps_ = new_maps;
	mutation_rate_ = copy.mutation_rate_;
	nonterminal_crossover_rate_ = copy.nonterminal_crossover_rate_;
	tournament_size_ = copy.tournament_size_;
	proportional_tournament_rate_ = copy.proportional_tournament_rate_;

	best_index_ = copy.best_index_;

	best_fitness_ = copy.best_fitness_;
	worst_fitness_ = copy.worst_fitness_;
	avg_fitness_ = copy.avg_fitness_;

	largest_tree_ = copy.largest_tree_;
	smallest_tree_ = copy.smallest_tree_;
	avg_tree_ = copy.avg_tree_;

	pop_.resize(copy.pop_.size());
	for (size_t i = 0; i < copy.pop_.size(); ++i) {
		Individual new_individual(copy.pop_[i]);
		pop_[i] = new_individual;
	}
}
void Population::Evolve() {
	std::vector<Individual> evolved_pop(pop_.size());
	
	evolved_pop[0] = pop_[best_index_];

	/* Non-elite individual selection. */
	for (size_t i = 1; i < evolved_pop.size(); ++i) {
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
void Population::CalculateFitness() {
	double cur_fitness = 0;
	avg_fitness_ = 0;
	best_fitness_ = DBL_MIN;
	worst_fitness_ = DBL_MAX;

	for (size_t i = 0; i < pop_.size(); ++i) {
		pop_[i].CalculateScores(maps_);
		pop_[i].CalculateFitness();
		cur_fitness = pop_[i].GetFitness();
		avg_fitness_ += cur_fitness;
		if (cur_fitness > best_fitness_) {
			best_fitness_ = cur_fitness;
		} else if (cur_fitness < worst_fitness_) {
			worst_fitness_ = cur_fitness;
		}
	}
	avg_fitness_ = avg_fitness_ / pop_.size();
	SetElite();
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
void Population::SetMaps(std::vector<TrailMap*> maps) {
	maps_ = maps;
}
std::string Population::ToString(bool include_fitness, bool latex) {
	std::stringstream ss;
	for (size_t i = 0; i < pop_.size(); ++i) {
		if (include_fitness) {
			ss << pop_[i].GetFitness() << " ==> ";
		}
		ss << pop_[i].ToString(latex) << "\n";
	}
	return ss.str();
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
size_t Population::GetBestTreeSize() {
	return pop_[best_index_].GetTreeSize();
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
std::vector<std::string> Population::GetBestSolutionMap(bool latex) {
	return pop_[best_index_].PrintSolvedMap(maps_, latex);
}
std::string Population::GetBestSolutionGraphViz(std::string graph_name) {
	return pop_[best_index_].CallGraphViz(graph_name);
}
void Population::RampedHalfAndHalf(size_t population_size, size_t depth_min, 
								   size_t depth_max) {
	unsigned gradations = static_cast<unsigned>(depth_max - depth_min + 1);
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
		if (parent->GetChild(c1.second)) { /* Make sure not a terminal */
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
	bool fitness_based;
	
	/* Determine whether tournment is fitness or parsimony based */
	std::uniform_real_distribution<double> t{ 0,1 };
	fitness_based = (t(GetEngine()) < proportional_tournament_rate_);

	/* Run the tournament */
	std::uniform_int_distribution<size_t> d{ 0,pop_.size() - 1 };
	winner = d(GetEngine());

	for (size_t i = 0; i < tournament_size_; ++i) {
		do {
			challenger = d(GetEngine());
		} while (winner == challenger);
		if (fitness_based) {
			if (pop_[challenger].GetFitness() > pop_[winner].GetFitness()) {
				winner = challenger; 
			}
		} else {
			if (pop_[challenger].GetTreeSize() < pop_[winner].GetTreeSize()) {
				winner = challenger;
			}
		}
	}
	return winner;
}
void Population::SetElite() {
	best_index_ = 0;

	for (size_t i = 0; i < pop_.size(); ++i) {
		if (pop_[i].GetFitness() > pop_[best_index_].GetFitness()) {
			best_index_ = i;
		}
	}
}
std::mt19937 &Population::GetEngine() {
	static std::random_device rd;
	static std::mt19937 mt(rd());
	return mt;
}