/*
 * node.h
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
 * @date 25 November 2015
 */
#pragma once

#include <random>
#include <utility> /* std::pair */
#include <vector>
#include "operator_types.h"
#include "trail_map.h"

/** 
 * @class	Node
 * Node of a generic tree data structure.  This class uses the default 
 * constructor and destructor and instead of having a copy constructor, just 
 * has a `Copy()` and `Erase()` function instead.  The tree functions provided
 * are for the use of the Individual and Population classes.
 *
 * @todo	It would be nice to use actual constructors and destructors 
 *			instead of the `Copy()` and `Erase()` functions.
 * @todo	Determine what else (if anything) is necessary for the data
 *			portion of the data structure.
 */
class Node {
public:
	/** 
	 * Creates a new version of a node, given a reference node.
	 *
	 * @todo	Determine whether I still need a pointer to the source node. 
	 */
	void Copy(Node *to_copy);
	/** Recursively erase the current node and its subtree. */
	void Erase();
	/** 
	 * A text representation of a node and its subtree.
	 *
	 * @param[in]	latex	If true, inserts LaTeX formatting.
	 *
	 * @return	Returns the tree representation as a `std::string`.
	 */
	std::string ToString(bool latex = false);
	/**
	 * Generates a random tree.
	 * @param[in]	cur_depth	The current depth of the tree.
	 * @param[in]	max_depth	The maximum depth the tree can be before 
	 * 							creating a terminal node.
	 * @param[in]	parent		Pointer to the parent node that's used to set
	 *							the parent parameter of the node.
	 * @param[in]	full_tree	If set to true, a nonterminal node will not
	 *							be created until the max depth has been
	 *							reached.  This is used for ramped half and 
	 *							half tree generation.
	 */
	void GenerateTree(size_t cur_depth, size_t max_depth, 
					  Node *parent, bool full_tree);
	/**
	 * Recursively traverses the tree and randomly changes a node's operator 
	 * to another of the same arity.
	 *
	 * @param[in]	mutation_chance		The possibility that an individual 
	 *									node will mutate.
	 * @param[in]	max_depth			Max tree depth to help curb code 
	 *									growth.
	 */
	void Mutate(double mutation_chance, size_t max_depth);
	/**
	 * Evaluate the fitness of a node and its subtree.
	 * @param[in]	&map		A reference to a map that's being evaluated.
	 *							The Individual and the TrailMap will know to
	 *							stop execution and the Map will contain the
	 *							number of food eaten.
	 */
	void Evaluate(TrailMap *map);
	/**
	 * Select a node from the tree.  Iteratively searches the tree,
	 * decrementing the `countdown` number each time only when a node
	 * desginated by the value of `nonterminal` is visited. Upon reaching 
	 * zero, the current node is returned as well its index in the child tree.
	 * @param[in]	countdown	How many more nodes remain before selection.
	 * @param[in]	nonterminal	Type of node to select.
	 * @return	Returns a pair consisting of the selected node as well as the
	 *			index of the node in the node's parent's `children_` vector.
	 */
	std::pair<Node*, size_t> SelectNode(size_t countdown, bool nonterminal);
	/** 
	 * Recursively iterates through the tree and counts the number of nodes
	 * encountered.
	 *
	 * @param[out]	term_count		The number of terminal nodes in the tree.
	 * @param[out]	nonterm_count	The number of nonterminal nodes in the 
	 *								tree.
	 */
	void CountNodes(size_t &term_count, size_t &nonterm_count);
	/** 
	 * Determines if the current node is terminal.  Checks the operator type
	 * of the node to determine if terminal or not.
	 * @return	True if a terminal operator type.
	 */
	bool IsTerminal();
	/**
	 * Determines if the current node is nonterminal.  Checks the operator type
	 * of the node to determine if nonterminal or not.
	 *
	 * @return	True if a nonterminal operator type.
	 */
	bool IsNonterminal();
	/** 
	 * Recursively iterate through the tree and set the parent point of each
	 * node and the depth of the node.  This is useful after the crossover 
	 * function has completed to make sure that the tree can be traversed in 
	 * both directions and also allows trees to leave scope and have their 
	 * memory freed.
	 *
	 * @param[in]	parent	Pointer to the parent.
	 * @param[in]	depth	The current depth.
	 */
	void CorrectNodes(Node *parent, size_t depth);

	/* Accessors/Mutators of Private Data */
	/** Returns the node's parent. */
	Node* GetParent();
	/** 
	 * Returns the child node of the node based on the index number given.
	 *
	 * @param[in]	child_number	The index number of the child to return.
	 *
	 * @return	The child node at the given index and `nullptr` otherwise.
	 */
	Node* GetChild(size_t child_number);
	/** Sets the `parent_` value of the node. */
	void SetParent(Node *parent);
	/** Sets the value of `children_` at the given index number. */
	void SetChild(size_t child_number, Node *child);
	/** Returns the depth of this node in the tree. */
	size_t GetCurrentDepth();
	std::string CallGraphViz(std::string graph_name);
private:
	struct NodeWrapper {
		Node* node;
		std::string node_name; /* Mangled name for identification purposes */
		std::string node_label; /* Label representing the type of node */
		bool nonterminal; /* Terminal or nonterminal */
	};
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
	NodeWrapper* Structify(Node *root, int counter);
	std::string GraphViz(std::string graph_name);

	/** A pointer to the parent node or `nullptr` if the root of the tree */
	Node *parent_;
	/** Pointers to children nodes. */
	std::vector<Node*> children_;
	/**
	 * The operator type of the node.  This is used to determine whether the
	 * node is terminal or nonterminal and for determining how to traverse
	 * the subtree.  The depth is simply how far into the tree we are.  This
	 * is used to help curb code growth.
	 */
	OpType op_;
	size_t depth_;

	

};