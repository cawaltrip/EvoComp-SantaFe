/*
 * population.h
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
#include <vector>
#include "individual.h"
#include "options.h"
#include "trail_map.h"

/**
 * @class	Population
 * Container for all of the different genetic program representations.
 * The constructor of this class passes all the information that is
 * provided in the main program to all of the different "subclasses" such
 * as the `Ant` and `Node` classes.
 */
class Population {
public:
	/** 
	 * `Population` class constructor.  Creates the entire population using
	 * ramped half and half based on the minimum and maximum tree sizes
	 * defined in the main program.  Also passes the other variables that
	 * are needed by the `Ant` and `Node` classes to those respective classes.
	 *
	 * @param[in]	population_size					Number of individuals in 
	 *												the population.
	 * @param[in]	mutation_rate					How often a single node 
	 *												will mutate.
	 * @param[in]	nonterminal_crossover_rate		Adjustable variable used 
	 *												with the 90/10 
	 *												nonterminal/terminal rate.
	 * @param[in]	tournament_size					The number of individuals 
	 *												compared when selecting an 
	 *												individual for crossover.
	 * @param[in]	proportional_tournament_rate	Adjustable variable used to
	 *												to determine whether
	 *												selection is based on
	 *												fitness or parsimony.
	 * @param[in]	depth_min						The lower bound on maximum 
	 *												tree size.
	 * @param[in]	depth_max						The upper bound on maximum 
	 *												tree size.
	 * @param[in]	maps							Vector of the different map
	 *												files that were read in.
	 */
	Population(size_t population_size, double mutation_rate, 
			   double nonterminal_crossover_rate, size_t tournament_size, 
			   double proportional_tournament_rate, size_t depth_min, 
			   size_t depth_max, std::vector<TrailMap> maps);
	Population(Options opts, std::vector<TrailMap> maps);
	Population(const Population &copy, std::vector<TrailMap> new_maps);
	/** 
	 * The evolve function is the wrapper for the different stages of
	 * evolution for the genetic program.  Specifically, `Evolve()` selects
	 * elite individuals, then completes a generation by using tournament
	 * selection to select two parents for crossover.  After crossover, the
	 * newly made individual is mutated.  This group becomes the population
	 * for the next generation.
	 *
	 * @param[in]	elitism_count	How many elite individuals to move from
	 *								the current generation to the next. This
	 *								currently isn't used and an elitism of
	 *								two individuals is hardcoded in the
	 *								`Elitism()` function.
	 *
	 * @todo	Properly implement Elitism.  Will require sorting the entire
	 *			population.
	 */
	void Evolve(size_t elitism_count = 2);
	/**
	 * Calculate the fitness of an individual based on the genetic program
	 * represented by the tree of an individual.
	 */
	void CalculateFitness();
	/**
	 * Calculates and sets the largest, smallest and average tree size
	 * variables.
	 */
	void CalculateTreeSize();
	/** 
	 * Sets the maps that the Individuals in the population will calculuate
	 * their fitness based on.
	 */
	void SetMaps(std::vector<TrailMap> maps);
	/** 
	 * Returns the `ToString()` function of every individual in the population.
	 * 
	 * @param[in]	include_fitness	Include the fitness for each individual.
	 * @param[in]	latex			Wrap LaTeX code around the output.
	 *
	 * @return	The `ToString()` of every individual delimited by newlines.
	 */
	std::string ToString(bool include_fitness, bool latex);
	/**
	 * Returns the `ToString()` function of the individual with the best 
	 * raw fitness score.
	 *
	 * @param[in]	include_fitness	Include the fitness for each individual.
	 * @param[in]	latex			Wrap LaTeX code around the output.
	 *
	 * @return	The `ToString()` of the individual with the best raw fitness.
	 */
	std::string BestSolutionToString(bool include_fitness, bool latex);
	/**
	 * Returns the number of nodes in the largest tree.
	 *
	 * @return	The number of nodes in the largest tree (not the index of the
	 *			individual in the population).
	 */
	size_t GetLargestTreeSize();
	/**
	 * Returns the number of nodes in the smallest tree.
	 *
	 * @return	The number of nodes in the smallest tree (not the index of the
	 *			individual in the population).
	 */
	size_t GetSmallestTreeSize();
	/**
	 * Returns the average number of nodes in a tree.
	 *
	 * @return	The average number of nodes in the trees.
	 */
	size_t GetAverageTreeSize();
	/**
	 * Returns the total number of nodes from all trees.
	 *
	 * @return	The total number of nodes from all trees in the population.
	 */
	size_t GetTotalNodeCount();
	/**
	 * Returns the best raw fitness score.
	 *
	 * @return	The best raw fitness score.
	 */
	double GetBestFitness();
	/**
	 * Returns the worst raw fitness score.
	 *
	 * @return	The worst raw fitness score.
	 */
	double GetWorstFitness();
	/**
	 * Returns the average raw fitness score across all individuals in the 
	 * population.
	 *
	 * @return	The average raw fitness score.
	 */
	double GetAverageFitness();
	/** Get a vector of all of the completed maps for the best solution. */
	std::vector<std::string> GetBestSolutionMap(bool latex);
private:
	/** 
	 * Ramped half and half initializes all of the trees in the population.
	 * The ramping is done by providing a minimum tree depth and maximum
	 * tree depth.  The trees are evenly distributed between all the steps
	 * between these two numbers.  Half of the trees in each step are full
	 * trees, and the other half aren't forced to be (they still must conform
	 * to the maximum size, but may be shallower).
	 *
	 * @param[in]	population_size		The number of individuals in the 
	 *									population.
	 * @param[in]	depth_min			The lower bound on maximum tree size.
	 * @param[in]	depth_max			The upper bound on maximum tree size.
	 */
	void RampedHalfAndHalf(size_t population_size, 
						   size_t depth_min, size_t depth_max);
	/** 
	 * Point crossover where two parents will create one offspring.  The child
	 * is constructed by replacing a subtree in `parent1` with a subtree of
	 * `parent2`.  Once this new tree is created, it is mutated and placed
	 * into the next generation's population.
	 * 
	 * @param[in,out]	parent1		This is a copy of an individual in the
	 *								population and serves both as one of the
	 *								parents of the crossover operation but
	 *								is also the actual object that is modified
	 *								in the process.
	 * @param[in]		parent2		This is a copy of an individual in the
	 *								population and a subtree of this tree is
	 *								spliced into `parent1`.
	 */
	void Crossover(Individual *parent1, Individual *parent2);
	/** 
	 * Implements tournament selection using the weighted fitness score.  The
	 * weighted fitness score exists because parsimony pressure is used to
	 * help prevent code growth.
	 */
	size_t SelectIndividual();
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
	std::vector<Individual> pop_;
	std::vector<TrailMap> maps_;
	double mutation_rate_;
	double nonterminal_crossover_rate_;
	size_t tournament_size_;
	double proportional_tournament_rate_;

	size_t largest_tree_;
	size_t smallest_tree_;
	size_t avg_tree_;
	size_t total_nodes_;
	size_t best_index_;
	size_t second_best_index_;
	double best_fitness_;
	double worst_fitness_;
	double avg_fitness_;
};