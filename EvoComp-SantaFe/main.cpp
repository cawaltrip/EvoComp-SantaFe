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
/**
 * @todo	Go through and make sure documentation is updated everywhere.
 * @todo	Implement some sort of parsimony pressure in addition to or 
 *			instead of parsimony selection?
 */
#include <fstream>
#include <iostream>
#include <sstream>
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
/** 
 * Return a string formatted to write to file.
 *
 * @param[in]	best_fitness		The fitness score of the best indiviudal.
 * @param[in]	avg_fitness			The average fitness of the generation.
 * @param[in]	best_solution_size	Tree size of the best individual.
 * @param[in]	avg_size			Average tree size of the generation.
 *
 * @return	The parameters separated by commas and formatted as 
 *			a `std::string`.
 */
std::string FormatOutput(double best_fitness, double avg_fitness,
							 size_t best_solution_size, size_t avg_size);
/* 
 * All of the command line options are stored in this object and this object
 * is passed where needed to read the options.
 */
static Options opts;

int main(int argc, char **argv, char **envp) {
	ParseCommandLine(argc, argv, opts);

	std::vector<TrailMap*> maps;
	std::vector<TrailMap*> secondary_maps;
	std::vector<TrailMap*> verification_maps;
	std::vector<std::pair<Population*,std::ofstream*>> populations;

	/* Create all the maps */
	for (std::string fn : opts.map_files_) {
		maps.emplace_back(new TrailMap(ParseDataFile(fn), 
								   opts.action_count_limit_));
	}
	if (opts.secondary_maps_exist_) {
		for (std::string fn : opts.secondary_map_files_) {
			secondary_maps.emplace_back(new TrailMap(ParseDataFile(fn), 
												 opts.action_count_limit_));
		}
	}
	if (opts.verification_maps_exist_) {
		for (std::string fn : opts.verification_map_files_) {
			verification_maps.emplace_back(new TrailMap(ParseDataFile(fn),
													opts.action_count_limit_));
		}
	}

	/* Create the populations */
	populations.emplace_back(
		std::make_pair(new Population(opts, maps),
					   new std::ofstream(opts.output_file_,
										 std::ios::out | std::ios::trunc)));
	if (opts.secondary_maps_exist_) {
		populations.emplace_back(
			std::make_pair(new Population(*(populations.front().first), 
										  secondary_maps),
						   new std::ofstream(opts.secondary_output_file_, 
											 std::ios::out | 
											 std::ios::trunc)));
	}

	/* Evolve the populations in tandem */
	for (size_t i = 0; i < opts.evolution_count_; ++i) {
		for (auto p : populations) {
			p.first->Evolve();
			(*p.second) << FormatOutput(p.first->GetBestFitness(), 
									 p.first->GetAverageFitness(), 
									 p.first->GetBestTreeSize(), 
									 p.first->GetAverageTreeSize());
			(*p.second) << "\n";
			
			if (i % 10 == 0) {
				std::clog << "Generation " << i << " completed.\n";
			}
			if (i % 100 == 0) {
				std::clog << "Current best solution: \n";
				std::clog << FormatOutput(p.first->GetBestFitness(),
										  p.first->GetAverageFitness(),
										  p.first->GetBestTreeSize(),
										  p.first->GetAverageTreeSize());
				std::clog << "\n";
				//std::clog << p.first->GetBestSolutionGraphViz();
				//std::clog << "\n";
			}
		}
	}
	for (auto p : populations) {
		p.second->close();
	}

	/* GraphViz output if specified at the command line */
	if (opts.graphviz_output_) {
		std::ofstream graph_output_file(opts.graphviz_file_, 
										std::ios::out | std::ios::trunc);
		size_t counter = 0;
		for (auto p : populations) {
			std::string graph_name = "SantaFe" + std::to_string(counter++);
			graph_output_file << p.first->GetBestSolutionGraphViz(graph_name);
		}
		graph_output_file.close();
	}

	/* Run verification maps if specified at the command line */
	if (opts.verification_maps_exist_) {
		std::ofstream verification_output_file;
		verification_output_file.open(opts.verification_output_file_,
									  std::ios::out | std::ios::trunc);
		for (auto p : populations) {
			p.first->SetMaps(verification_maps);
			p.first->CalculateFitness();
			verification_output_file << FormatOutput(
				p.first->GetBestFitness(), p.first->GetAverageFitness(),
				p.first->GetBestTreeSize(), p.first->GetAverageTreeSize());
		}
		verification_output_file.close();
	}

	return 0;
}
void ParseCommandLine(int argc, char **argv, Options &opts) {
	/* Command Line Option Categories */
	po::options_description basic_opts("Basic Options");
	po::options_description pop_opts("Population Options");
	po::options_description indiv_opts("Individual Options");
	po::options_description io_opts("Input/Output File Options");
	po::options_description cmd_opts;
	po::positional_options_description positional_opts;
	po::variables_map vm;

	/* Basic Options */
	basic_opts.add_options()
		("help,h", "print this help and exit")
		("verbose,v", "print extra logging information");
	/* Population Options */
	pop_opts.add_options()
		("generations,g",
		 po::value<size_t>(&opts.evolution_count_),
		 "Number of generations to evolve.")
		("population-size,p",
		 po::value<size_t>(&opts.population_size_),
		 "Number of individuals in a population.")
		("action-limit,a",
		 po::value<size_t>(&opts.action_count_limit_),
		 "Maximum number of actions to evaluate.")
		("tournament-size,t",
		 po::value<size_t>(&opts.tournament_size_),
		 "Number of Individuals in a tournament.")
		("proportional-tournament-rate,r",
		 po::value<double>(&opts.proportional_tournament_rate_),
		 "Rate that tournament is fitness based instead of parsimony based.");
	/*Individual Options */
	indiv_opts.add_options()
		("mutation,m", 
		 po::value<double>(&opts.mutation_rate_),
		 "Rate of mutation per node in the tree.")
		("nonterminal-crossover-rate,n",
		 po::value<double>(&opts.nonterminal_crossover_rate_),
		 "Rate that nonterminals are chosen as crossover point.")
		("min-depth,d", 
		 po::value<size_t>(&opts.tree_depth_min_),
		 "Minimum tree depth.")
		("max-depth,x", 
		 po::value<size_t>(&opts.tree_depth_max_),
		 "Maximum tree depth.");
	/* Input/Output Options */
	io_opts.add_options()
		("input,I",
		 po::value<std::vector<std::string>>(&opts.map_files_)->required(),
		 "Specify input file(s)")
		("secondary,S",
		 po::value<std::vector<std::string>>(&opts.secondary_map_files_),
		 "Secondary set of input file(s) to compare against.")
		("verification,V",
		 po::value<std::vector<std::string>>(&opts.verification_map_files_),
		 "Set of input file(s) to use for verification but not evolution.")
		("graphviz,G",
		 po::value<std::string>(&opts.graphviz_file_),
		 "Specify the file for GraphViz output file.")
		("output,O", po::value<std::string>(&opts.output_file_),
		 "Output file location for main GP population.")
		("secondary-output,T",
		 po::value<std::string>(&opts.secondary_output_file_),
		 "Output file for secondary GP population.")
		("verification-output,W",
		 po::value<std::string>(&opts.verification_output_file_),
		 "Output file for verification GP population.");
	/* All unspecified options are treated as input files */
	positional_opts.add("input", -1);

	cmd_opts.add(basic_opts).add(pop_opts).add(indiv_opts).add(io_opts);

	po::store(po::command_line_parser(argc, argv).options(cmd_opts)
			  .positional(positional_opts).run(), vm);

	/*
	 * Help is checked before notify, because we don't want to create an
	 * error if the user only specifies the help flag and doesn't set any
	 * of the required flags (even though these have all been removed and 
	 * replaced with default values.
	 */
	if (vm.count("help")) {
		std::cout << GetUsageString(std::string(argv[0])) << std::endl;
		std::cout << cmd_opts << std::endl;
		exit(EXIT_SUCCESS);
	}

	/* 
	 * Make sure all the options are properly set and error if required
	 * fields are missing.
	 */
	po::notify(vm);
	
	/* 
	 * Check all of the basic options for validity (such as input files 
	 * existing 
	 */
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

	if (vm.count("graphviz")) {
		opts.graphviz_output_ = true;
	} else {
		opts.graphviz_output_ = false;
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
std::string FormatOutput(double best_fitness, double avg_fitness, 
					  size_t best_solution_size, size_t avg_size) {
	std::stringstream ss;
	ss << best_fitness << "," << best_solution_size << ",";
	ss << avg_fitness << "," << avg_size;
	return ss.str();
}