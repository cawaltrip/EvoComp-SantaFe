/*
 * node.cpp
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

#include <deque>
#include <iostream> /* Logging/error reporting only */
#include <string> /* for std::to_string() which may not be needed. */
#include "node.h"

void Node::Copy(Node *to_copy) {
	parent_ = to_copy->parent_;
	op_ = to_copy->op_;

	switch (op_) {
	case OpType::kProg2:
		for (size_t i = 0; i < 2; ++i) {
			children_[i] = new Node;
			children_[i]->Copy(to_copy->children_[i]);
		}
		break;
	case OpType::kProg3:
		for (size_t i = 0; i < 3; ++i) {
			children_[i] = new Node;
			children_[i]->Copy(to_copy->children_[i]);
		}
		break;
	case OpType::kIfFoodAhead:
		children_[0] = new Node;
		children_[0]->Copy(to_copy->children_[0]);
		break;
	case OpType::kMoveForward:
	case OpType::kTurnLeft:
	case OpType::kTurnRight:
		children_.clear();
		break;
	}
}
void Node::Erase() {
	for (Node *child : children_) {
		if (child) {
			child->Erase();
		}
	}
	delete this;
}
std::string Node::ToString(bool latex) {
	/** @todo	Implement Node::ToString()! */
	return "Node::ToString is a stub currently";
}
void Node::GenerateTree(size_t cur_depth, size_t max_depth,
						Node *parent, bool full_tree) {
	OpType lower_bound, upper_bound;
	parent_ = parent;

	if (full_tree) {
		lower_bound = OpType::kProg2;
		upper_bound = OpType::kIfFoodAhead;
	} else {
		lower_bound = OpType::kProg2;
		upper_bound = OpType::kTurnRight;
	}
	if (cur_depth >= max_depth) {
		lower_bound = OpType::kMoveForward;
		upper_bound = OpType::kTurnRight;
	}

	std::uniform_int_distribution<int> d{ lower_bound, upper_bound };
	op_ = static_cast<OpType>(d(GetEngine()));
	size_t children_counter = 0;
	children_.clear(); /** @todo	Will this damage crossover? */
	switch (op_) {
	case OpType::kProg3:
		++children_counter;
	case OpType::kProg2:
		++children_counter;
	case OpType::kIfFoodAhead:
		++children_counter;
		children_.resize(children_counter);
		for (size_t i = 0; i < children_counter; ++i) {
			Node *child = new Node;
			children_[i] = child;
			child->GenerateTree(cur_depth + 1, max_depth, this, full_tree);
		}
		break;
	case OpType::kMoveForward:
	case OpType::kTurnLeft:
	case OpType::kTurnRight:
		break;
	default:
		/* Shouldn't ever get here */
		std::cerr << "Bad node type!" << std::endl;
		exit(EXIT_FAILURE);
		break;
	}
}
void Node::Mutate(double mutation_chance, size_t max_depth) {
	std::uniform_real_distribution<double> mut_dist{ 0,1 };
	const size_t kMinimumTreeIncrease = 3;

	if (mut_dist(GetEngine()) <= mutation_chance) {
		OpType lower_bound, upper_bound;
		if (IsTerminal()) {
			lower_bound = OpType::kMoveForward;
			upper_bound = OpType::kTurnRight;
		} else {
			lower_bound = OpType::kProg3;
			upper_bound = OpType::kIfFoodAhead;
		}
		std::uniform_int_distribution<int> d{ lower_bound, upper_bound };
		op_ = static_cast<OpType>(d(GetEngine()));
		size_t children_counter = 0;
		children_.clear();
		switch (op_) {
		case OpType::kProg3:
			++children_counter;
		case OpType::kProg2:
			++children_counter;
		case OpType::kIfFoodAhead:
			++children_counter;
			for (size_t i = 0; i < children_counter; ++i) {
				Node *child = new Node;
				children_[i] = child;
				/* Determine max tree size */
				size_t adjusted_depth = max_depth;
				if (adjusted_depth < depth_) {
					adjusted_depth = depth_ + kMinimumTreeIncrease;
				}
				child->GenerateTree(depth_ + 1, adjusted_depth, this, false);
			}
		}
	} else {
		for (Node *child : children_) {
			child->Mutate(mutation_chance, max_depth);
		}
	}
}
size_t Node::Evaluate(TrailMap map) {
	return 1; /** @todo	Implement evaluation function. */
}
std::pair<Node*, size_t> Node::SelectNode(size_t countdown, bool nonterminal) {
	std::deque<std::pair<Node*, size_t>> stack;
	std::pair<Node*, size_t> curr;
	bool done = false;

	stack.emplace_front(std::make_pair(this, 0));

	while (!done) {
		if (stack.empty()) {
			std::cerr << "Empty stack!" << std::endl;
			exit(EXIT_FAILURE);
		}
		if (countdown == SIZE_MAX) {
			std::cerr << "Countdown wrapped around!" << std::endl;
			exit(EXIT_FAILURE);
		}
		curr = stack.front();
		stack.pop_front();
		if (countdown == 0 && nonterminal == curr.first->IsNonterminal()) {
			/* Found the correct node */
			done = true;
			break;
		}
		if (nonterminal == curr.first->IsNonterminal()) {
			--countdown;
		}
		for (size_t i = 0; i < curr.first->children_.size(); ++i) {
			stack.emplace_front(std::make_pair(curr.first->children_[i], i));
		}
	}
	return curr;
}

void Node::CountNodes(size_t &term_count, size_t &nonterm_count) {
	switch (op_) {
	case OpType::kProg3:
	case OpType::kProg2:
	case OpType::kIfFoodAhead:
		++nonterm_count;
		for (Node* child : children_) {
			child->CountNodes(term_count, nonterm_count);
		}
		break;
	case OpType::kMoveForward:
	case OpType::kTurnLeft:
	case OpType::kTurnRight:
		++term_count;
		break;
	}
}

bool Node::IsNonterminal() {
	switch (op_) {
	case OpType::kProg3:
	case OpType::kProg2:
	case OpType::kIfFoodAhead:
		return true;
	}
	return false;
}

bool Node::IsTerminal() {
	return !IsNonterminal();
}

void Node::CorrectParents(Node *parent) {
	this->parent_ = parent;
	if (IsNonterminal()) {
		for (Node *child : children_) {
			child->CorrectParents(this);
		}
	}
}

Node* Node::GetParent() {
	return parent_;
}
Node* Node::GetChild(size_t child_number) {
	if (child_number < children_.size()) {
		return children_[child_number];
	}
	return nullptr;
}
void Node::SetParent(Node *parent) {
	parent_ = parent;
}
void Node::SetChild(size_t child_number, Node *child) {
	if (child_number < children_.size()) {
		children_[child_number] = child;
	}
	children_.push_back(child);
}
size_t Node::GetCurrentDepth() {
	return depth_;
}
std::mt19937 &Node::GetEngine() {
	static std::random_device rd;
	static std::mt19937 mt(rd());
	return mt;
}