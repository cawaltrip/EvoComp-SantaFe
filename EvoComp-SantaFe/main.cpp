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
 * @todo	Determine how and where to store Ant's position data.
 */

#include <fstream>
#include <iostream>
#include <vector>
#include <boost/program_options.hpp>
#include "trail_map.h"
namespace po = boost::program_options;

/**
 * Reads command line arguments for the program.  Certain arguments will cause
 * the execution of the program to halt, e.g. `--help` or `--input` with an
 * an invalid filename.
 * @param[in]	argc	Number of items in argv.
 * @param[in]	argv	List of command line arguments.
 * @return	Returns the filenames as a `std::vector<std::string>`.
 * @todo	Allow for some maps to be withheld as a final test (after 
 *			evolution is complete) to test the bias of the genetic program 
 *			towards a specialized solution versus a generalized solution.
 */
 std::vector<std::string> ParseCommandLine(int argc, char **argv);
 /**
  * Returns the utility usage syntax.
  * @param[in]	program_name	Name of the exectued program (i.e, argv[0]).
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
 * Returns string representation of map.
 * @param[in]	map		Map to print.
 * @param[in]	latex	Add LaTeX wrappings or not.
 * @return	Returns a `std::string` containing contents of the map.
 */
std::string PrintMap(std::vector<std::vector<char>> map, bool latex);

int main(int argc, char **argv, char **envp) {
	/* Genetic Program Constants */
	const size_t kEvolutionCount = 1000;
	const size_t kElitismCount = 2;

	/* Map Constants */
	const size_t kStepLimit = 300;

	/* Population Constants */
	const size_t kPopulationSize = 100;
	const double kMutationRate = 0.03;
	const double kNonterminalCrossoverRate = 0.90;
	const size_t kTournamentSize = 3;

	/* Individual/Node Constants */
	const size_t kTreeDepthMin = 3;
	const size_t kTreeDepthMax = 6;

	
	std::vector<std::string> files = ParseCommandLine(argc, argv);
	for (std::string f : files) {
		TrailMap map(ParseDataFile(f), kStepLimit);
		std::cout << map.ToString(false) << std::endl << std::endl << std::endl;
	}

	return 0;
}
std::vector<std::string> ParseCommandLine(int argc, char **argv) {
	/* Vector to hold return values */
	std::vector<std::string> filenames;

	/* Parse Command Line arguments and return filename string */
	po::options_description opts("Options");
	opts.add_options()
		("help,h", "print this help and exit")
		("verbose,v", "print extra logging information")
		("input,i", po::value<std::vector<std::string>>(),
		 "specify input file(s)");
	po::positional_options_description positional_opts;
	positional_opts.add("input", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(opts)
			  .positional(positional_opts).run(), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << GetUsageString(std::string(argv[0])) << std::endl;
		std::cout << opts << std::endl;
		exit(EXIT_SUCCESS);
	}

	if (!vm.count("verbose")) {
		std::clog.rdbuf(nullptr);
	}

	if (vm.count("input")) {
		filenames = vm["input"].as<std::vector<std::string>>();
		for (auto fn : filenames) {
			if (!(std::ifstream(fn).good())) {
				std::cerr << fn << " not found!" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
	} else {
		std::cerr << "Please specify input files" << std::endl;
		std::cerr << GetUsageString(std::string(argv[0])) << std::endl;
		std::cerr << opts << std::endl;
		exit(EXIT_FAILURE);
	}
	return filenames;
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
std::string PrintMap(TrailMap map, bool latex) {
	return map.ToString(latex);
}