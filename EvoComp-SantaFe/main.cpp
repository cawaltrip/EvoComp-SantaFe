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
 */

#include <iostream>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

/**
 * Reads command line arguments for the program.  Certain arguments will cause
 * the execution of the program to halt, e.g. `--help` or `--input` with an
 * an invalid filename.
 * @param[in]	argc	Number of items in argv.
 * @param[in]	argv	List of command line arguments.
 * @return	Returns the filename specified as a `std::string`.
 * @todo	Allow for multiple inputs to be specified so that the Ant can
 *			be evaluated using multiple maps.
 * @todo	Allow for some maps to be withheld as a final test (after 
 *			evolution is complete) to test the bias of the genetic program 
 *			towards a specialized solution versus a generalized solution.
 */
 std::string ParseCommandLine(int argc, char **argv);
 /** 
 * The main function. Calls the command line parser, calls the map parser,
 * creates and evolves the population and writes the results to file.
 * @param[in]	argc	Number of items in argv.
 * @param[in]	argv	List of command line arguments.
 * @param[in]	envp	Environment variables from user (Not currently used).
 */
int main(int argc, char **argv, char **envp) {

	return 0;
}

