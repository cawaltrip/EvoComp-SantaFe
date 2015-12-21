/*
 * individual.h
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
 * @date 6 December 2015
 */
#pragma once

#include <random>
#include <utility>
#include <vector>
#include "node.h"
#include "trail_map.h"

/**
 * @class	Individual
 * Contains a single tree that represents the set of instructions that the
 * ant will follow.
 */
class Individual {
public:
	/**
	 * `Individual` class empty constructor.  Used only to create an empty
	 * root node of the tree.
	 */
	Individual();
	/** `Individual` class constructor that also generates the tree. */
	Individual(size_t depth_max, bool full_tree);
	/** 
	 * `Individual` class copy constructor.  This copy constructor creates a 
	 * new root node, copies the tree from the source tree and then corrects
	 * the pointers to all the `parent_` variables afterwards.
	 */
	Individual(const Individual &to_copy);
	/** Call `root_->Erase()` to delete the entire solution tree. */
	void Erase();
	/** 
	 * Create a `std::string` of the tree.  Optionally LaTeX functions can be
	 * wrapped around the string to make importing into TeX documents
	 * easier.
	 */
	std::string ToString(bool latex);
	/** 
	 * Generate the solution tree.  The tree can either be full to the
	 * maximum depth specified or a sparse tree of any size up to the maximum
	 * depth.
	 */
	void GenerateTree(size_t depth_max, bool full_tree);
	/** 
	 * Mutate the solution.  The probability of an individual node is passed
	 * to the tree.
	 */
	void Mutate(double mutation_rate);
	/** 
	 * Select a random terminal or nonterminal node and return the node and
	 * its index according to its parent node.
	 */
	std::pair<Node*, size_t> GetRandomNode(bool nonterminal);
	/**
	 * Calculate the number of terminal and nonterminal nodes and store them
	 * for later retrieval.
	 */
	void CalculateTreeSize();
	/**
	 * Calculate the scores of the individual based on the maps given.
	 */
	void CalculateScores(std::vector<TrailMap*> maps);
	/**
	 * Calculate the fitness of the individual based on the scores that were
	 * set previously.
	 */
	void CalculateFitness();
	/**
	 * Correct the `parent_` variable pointers for the entire tree.  This is
	 * necessary after copying the tree during the crossover operation.
	 */
	void CorrectTree();
	/** Return the fitness of the Individual */
	double GetFitness();
	/** Return the size of the tree of the Individual */
	size_t GetTreeSize();
	/** Return the number of terminal nodes in the tree. */
	size_t GetTerminalCount();
	/** Return the number of nonterminal nodes in the tree. */
	size_t GetNonterminalCount();
	/** Return the pointer to the root node of the tree. */
	Node* GetRootNode();
	/** 
	 * Set the root node of the tree.
	 *
	 * @param[in]	*root	New root node pointer.
	 */
	void SetRootNode(Node *root);
	/**
	 * Return a `std::string` representation of a map with the route the ant
	 * traversed shown.
	 */
	std::vector<std::string> PrintSolvedMap(std::vector<TrailMap*> maps,
											bool latex);
	std::string CallGraphViz(std::string graph_name);
private:
	/**
	 * A static random engine that can be shared throughout the entire class.
	 * Based on the idea found in:
	 * http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3551.pdf
	 * This class uses the well-defined STL Mersenne Twister engine, mt19937.
	 * The first time this method is called, a std::mt19937 engine is
	 * initialized and seeded by `std::random_device` and is returned.  All
	 * subsequent calls return the originally created engine.
	 *
	 * @return	A Mersenne Twister Engine seeded by `std::random_device`.
	 */
	std::mt19937 &GetEngine();
	/** Run the genetic program on a list of maps */
	void RunSimulation(std::vector<TrailMap*> maps);
	/** Run the genetic program on a single map */
	void RunSimulation(TrailMap *map);
	
	Node *root_; /**< Root node of the solution tree. */
	std::vector<std::pair<size_t, size_t>> scores_; /**< Raw food counts. */
	double fitness_; /**< Fitness score. */
	size_t original_max_depth_; /**< The original maximum depth of the tree. */
	size_t terminal_count_;	/**< Number of terminal nodes in the tree. */
	size_t nonterminal_count_; /**< Number of nonterminal nodes in the tree. */

};

