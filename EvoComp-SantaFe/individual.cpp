/*
 * individual.cpp
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

#include "individual.h"
#include <deque>
#include <iostream> /* Logging/error reporting only */
#include <string> /* std::to_string() if this is even necessary anymore */

Individual::Individual() {
	root_ = new Node;
	original_max_depth_ = 0;
	terminal_count_ = 0;
	nonterminal_count_ = 0;
	fitness_ = 0;
}
Individual::Individual(size_t depth_max, bool full_tree) : Individual() {
	original_max_depth_ = depth_max;
	GenerateTree(depth_max, full_tree);
}
Individual::Individual(const Individual &to_copy) {
	root_ = new Node;
	root_->Copy(to_copy.root_);
	original_max_depth_ = to_copy.original_max_depth_;
	terminal_count_ = to_copy.terminal_count_;
	nonterminal_count_ = to_copy.nonterminal_count_;
	fitness_ = to_copy.fitness_;
	CorrectTree();
}
void Individual::Erase() {
	root_->Erase();
}
std::string Individual::ToString(bool latex) {
	return root_->ToString(latex);
}
void Individual::GenerateTree(size_t depth_max, bool full_tree) {
	root_->GenerateTree(0, depth_max, nullptr, full_tree);
	CalculateTreeSize();
}
void Individual::Mutate(double mutation_rate) {
	root_->Mutate(mutation_rate, original_max_depth_);
}
std::pair<Node*, size_t> Individual::GetRandomNode(bool nonterminal) {
	size_t upper_bound;
	size_t countdown;

	/* Even if nonterminal is true, return root node if it's the only node. */
	if (nonterminal_count_ == 0) {
		return std::pair<Node*, size_t>(root_, 0); /** @todo	-1 instead? */
	}

	if (nonterminal) {
		upper_bound = nonterminal_count_ - 1;
	} else {
		upper_bound = terminal_count_ - 1;
	}
	std::uniform_int_distribution<size_t> d{ 0,upper_bound };
	countdown = d(Individual::GetEngine());

	return root_->SelectNode(countdown, nonterminal);
}
void Individual::CalculateTreeSize() {
	terminal_count_ = 0;
	nonterminal_count_ = 0;
	root_->CountNodes(terminal_count_, nonterminal_count_);
}
void Individual::CalculateScores(std::vector<TrailMap> maps) {
	RunSimulation(maps);

	scores_.clear();
	for (auto map : maps) {
		scores_.emplace_back(std::make_pair(map.GetConsumedFoodCount(),
											map.GetTotalFoodCount()));
	}
}
void Individual::CalculateFitness() {
	fitness_ = 0;
	for (auto score : scores_) {
		if (score.first && score.second) {
			fitness_ += static_cast<double>(score.first / score.second);
		}
	}
	fitness_ *= 100;
	fitness_ /= scores_.size();
}
void Individual::CorrectTree() {
	root_->CorrectNodes(nullptr, 0);
	CalculateTreeSize();
}
double Individual::GetFitness() {
	return fitness_;
}
size_t Individual::GetTreeSize() {
	return GetTerminalCount() + GetNonterminalCount();
}
size_t Individual::GetTerminalCount() {
	return terminal_count_;
}
size_t Individual::GetNonterminalCount() {
	return nonterminal_count_;
}
Node* Individual::GetRootNode() {
	return root_;
}
void Individual::SetRootNode(Node *root) {
	root_ = root;
}
std::vector<std::string> Individual::PrintSolvedMap(
	std::vector<TrailMap> maps, bool latex) {
	std::vector<std::string> printed_maps;
	for (TrailMap &map : maps) {
		RunSimulation(map);
		printed_maps.emplace_back(map.ToString(latex));
	}
	return printed_maps;
}
std::mt19937 &Individual::GetEngine() {
	static std::random_device rd;
	static std::mt19937 mt(rd());
	return mt;
}
void Individual::RunSimulation(std::vector<TrailMap> maps) {
	for (TrailMap &map : maps) {
		RunSimulation(map);
	}
}
void Individual::RunSimulation(TrailMap &map) {
	map.Reset();
	while (map.HasActionsRemaining()) {
		root_->Evaluate(map);
	}
}