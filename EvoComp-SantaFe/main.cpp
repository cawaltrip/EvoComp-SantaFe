/*
 * main.cpp
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
 * Source file containing the overall execution of the genetic program.
 * Reads in command line arguments from the user including a map file to be
 * used as a test for the Ant.  It then creates a population of potential
 * solutions, runs the evolution process and writes the results to file.
 *
 * @file
 * @date 23 November 2015
 *
 */

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include "options.h"
#include "population.h"
#include "trail_map.h"
namespace po = boost::program_options;

/**
 * Reads command line arguments for the program.  Certain arguments will cause
 * the execution of the program to halt, e.g. `--help` or `--input` with an
 * an invalid filename.
 * @param[in]	argc	Number of items in argv.
 * @param[in]	argv	List of command line arguments.
 * @param[out]	opts	Command line options that were specified.
 */
 void ParseCommandLine(int argc, char **argv, Options &opt);
 /**
  * Returns the utility usage syntax.
  * @param[in]	program_name	Name of the exectued program (i.e, argv[0]).
  *
  * @return	Returns a `std::string` of usage details with the name of the 
  *			calling program inserted into the string.
  */
 std::string GetUsageString(std::string program_name);
 /** 
  * Returns a vector of the lines in the given file.  This is passed to the
  * TrailMap to construct a new map object.
  * @param[in]	filename	Name of the file to parse.
  * @return	Returns a `std::vector<std::string>` containing the
  *			contents of the file read in.
  */
std::vector<std::string> ParseDataFile(std::string filename);

static Options opts;

int main(int argc, char **argv, char **envp) {
	ParseCommandLine(argc, argv, opts);

	std::vector<TrailMap> maps;
	std::vector<TrailMap> secondary_maps;
	std::vector<TrailMap> verification_maps;
	std::vector<Population> populations;

	/* Create all the maps */
	for (std::string fn : opts.map_files_) {
		maps.emplace_back(TrailMap(ParseDataFile(fn), 
								   opts.action_count_limit_));
	}
	if (opts.secondary_maps_exist_) {
		for (std::string fn : opts.secondary_map_files_) {
			secondary_maps.emplace_back(TrailMap(ParseDataFile(fn), 
												 opts.action_count_limit_));
		}
	}
	if (opts.verification_maps_exist_) {
		for (std::string fn : opts.verification_map_files_) {
			verification_maps.emplace_back(TrailMap(ParseDataFile(fn),
													opts.action_count_limit_));
		}
	}

	/* Create the populations */
	populations.emplace_back(Population(opts, maps));
	if (opts.secondary_maps_exist_) {
		populations.emplace_back(Population(populations.front(), 
											secondary_maps));
	}

	/* Evolve the populations in tandem */
	for (size_t i = 0; i < opts.evolution_count_; ++i) {
		for (auto p : populations) {
			p.Evolve(opts.elitism_count_);
			if (i % 100 == 0) {
				std::clog << i << ": " << p.GetBestFitness();
				std::clog << "(" << p.GetWorstFitness() << ")\n";
				std::clog << p.BestSolutionToString(true, false) << "\n";
			}
			/** @todo	Do file IO that's need here. */
		}
	}

	/*
	 * Evolution is done.  Write last of results to file.  If a verification 
	 * map set exists, then run all the populations through the maps.
	 */
	if (opts.verification_maps_exist_) {
		for (auto p : populations) {
			p.SetMaps(verification_maps);
		}
		for (auto p : populations) {
			p.CalculateFitness();
			std::clog << "Verification: " << p.GetBestFitness() << "\n";
			/** @todo	Do new necessary file IO here. */
		}
	}

	return 0;
}
void ParseCommandLine(int argc, char **argv, Options &opts) {
	/* Vector to hold return values */
	std::vector<std::string> filenames;

	po::options_description basic_opts("Basic Options");
	po::options_description pop_opts("Population Options");
	po::options_description indiv_opts("Individual Options");
	po::options_description input_opts("Input File Options");
	po::options_description cmd_opts;
	po::positional_options_description positional_opts;
	po::variables_map vm;

	/* Basic Options */
	basic_opts.add_options()
		("help,h", "print this help and exit")
		("verbose,v", "print extra logging information");
	pop_opts.add_options()
		("generations,g",
		 po::value<size_t>(&opts.evolution_count_)->default_value(500),
		 "Number of generations to evolve.")
		("elitism,e",
		 po::value<size_t>(&opts.elitism_count_)->default_value(2),
		 "Number of elite individuals to keep between generations.")
		("population-size,p",
		 po::value<size_t>(&opts.population_size_)->default_value(100),
		 "Number of individuals in a population.")
		("action-limit,a",
		 po::value<size_t>(&opts.action_count_limit_)->default_value(400),
		 "Maximum number of actions to evaluate.")
		("tournament-size,t",
		 po::value<size_t>(&opts.tournament_size_)->default_value(3),
		 "Number of Individuals in a tournament.")
		("proportional-tournament-rate,r",
		 po::value<double>(&opts.proportional_tournament_rate_)
		 ->default_value(1.00),
		 "Rate that tournament is fitness based instead of parsimony based.");
	indiv_opts.add_options()
		("mutation,m", 
		 po::value<double>(&opts.mutation_rate_)->default_value(0.03),
		 "Rate of mutation per node in the tree.")
		("nonterminal-crossover-rate,n",
		 po::value<double>(&opts.nonterminal_crossover_rate_)
		 ->default_value(0.90),
		 "Rate that nonterminals are chosen as crossover point.")
		("min-depth,d", 
		 po::value<size_t>(&opts.tree_depth_min_)->default_value(3),
		 "Minimum tree depth.")
		("max-depth,x", 
		 po::value<size_t>(&opts.tree_depth_max_)->default_value(6),
		 "Maximum tree depth.");
	input_opts.add_options()
		("input,I",
		 po::value<std::vector<std::string>>(&opts.map_files_)
		 ->required(),
		 "Specify input file(s)")
		("secondary,S",
		 po::value<std::vector<std::string>>(&opts.secondary_map_files_),
		 "Secondary set of input file(s) to compare against.")
		("verification,V",
		 po::value<std::vector<std::string>>(&opts.verification_map_files_),
		 "Set of input file(s) to use for verification but not evolution.");
	positional_opts.add("input", -1);

	cmd_opts.add(basic_opts).add(pop_opts).add(indiv_opts).add(input_opts);

	po::store(po::command_line_parser(argc, argv).options(cmd_opts)
			  .positional(positional_opts).run(), vm);

	if (vm.count("help")) {
		std::cout << GetUsageString(std::string(argv[0])) << std::endl;
		std::cout << cmd_opts << std::endl;
		exit(EXIT_SUCCESS);
	}

	po::notify(vm);
	
	if (!vm.count("verbose")) {
		std::clog.rdbuf(nullptr);
	}

	if (vm.count("input")) {
		for (auto fn : opts.map_files_) {
			if (!(std::ifstream(fn).good())) {
				std::cerr << fn << " not found!" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
	} else {
		std::cerr << "Please specify input files" << std::endl;
		std::cerr << GetUsageString(std::string(argv[0])) << std::endl;
		std::cerr << cmd_opts << std::endl;
		exit(EXIT_FAILURE);
	}

	if (vm.count("secondary")) {
		for (auto fn : opts.secondary_map_files_) {
			if (!(std::ifstream(fn).good())) {
				std::cerr << fn << " not found!" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		opts.secondary_maps_exist_ = true;
	} else {
		opts.secondary_maps_exist_ = false;
	}

	if (vm.count("verification")) {
		for (auto fn : opts.verification_map_files_) {
			if (!(std::ifstream(fn).good())) {
				std::cerr << fn << " not found!" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		opts.verification_maps_exist_ = true;
	} else {
		opts.verification_maps_exist_ = false;
	}

	if (opts.elitism_count_ > opts.population_size_) {
		std::cerr << "Population Size must be larger than Elitism Count.  ";
		std::cerr << "Adjust these values and recompile the program.";
		std::cerr << std::endl;
		exit(EXIT_FAILURE);
	}
}
std::string GetUsageString(std::string program_name) {
	size_t found = program_name.find_last_of("/\\");
	std::string usage = "Usage: " + program_name.substr(found + 1);
	usage += " [options] input_file_1 [input_file_2...]";

	return usage;
}
std::vector<std::string> ParseDataFile(std::string filename) {
	std::ifstream inf;
	std::string line;
	std::vector<std::string> map_file_contents;

	inf.open(filename);
	if (!inf) {
		std::cerr << "Failed to open file: " << filename << std::endl;
		exit(EXIT_FAILURE);
	}

	/* Parse input file */
	while (std::getline(inf, line)) {
		map_file_contents.push_back(line);
	}
	return map_file_contents;
}