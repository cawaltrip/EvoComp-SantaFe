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
	weighted_fitness_ = 0;
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
	weighted_fitness_ = to_copy.weighted_fitness_;

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
void Individual::CalculateFitness(std::vector<TrailMap> maps) {
	fitness_ = 0.0;
	for (TrailMap &map : maps) {
		map.Reset();
		while (map.HasActionsRemaining()) {
			root_->Evaluate(map);
		}
		fitness_ += static_cast<double>(map.GetConsumedFoodCount() /
										map.GetTotalFoodCount());
	}
	fitness_ = fitness_ / maps.size();
}
void Individual::CalculateWeightedFitness(double parsimony_coefficient) {
	/** @todo	Once figuring out regular fitness this will be easy. */
}
void Individual::CorrectTree() {
	root_->CorrectParents(nullptr);
	CalculateTreeSize();
}
double Individual::GetFitness() {
	return fitness_;
}
double Individual::GetWeightedFitness() {
	return weighted_fitness_;
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
std::mt19937 &Individual::GetEngine() {
	static std::random_device rd;
	static std::mt19937 mt(rd());
	return mt;
}
bool Individual::operator==(const Individual &rhs) {
	return fitness_ == rhs.fitness_;
}
bool Individual::operator!=(const Individual &rhs) {
	return !operator==(rhs);
}
bool Individual::operator<(const Individual &rhs) {
	return fitness_ < rhs.fitness_;
}
bool Individual::operator<=(const Individual &rhs) {
	return operator<(rhs) || operator==(rhs);
}
bool Individual::operator>(const Individual &rhs) {
	return !operator<=(rhs);
}
bool Individual::operator>=(const Individual &rhs) {
	return !operator<(rhs);
}