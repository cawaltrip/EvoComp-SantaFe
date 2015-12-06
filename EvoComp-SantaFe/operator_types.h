/*
 * operator_types.h
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
 * Enumerates the types of actions an Ant can make.  These are the operators
 * that a node in the GP tree can be (both the terminal and nonterminal nodes).
 *
 * @file
 * @date 23 November 2015
 */
#pragma once
/**
 * Enumerated class of possible actions of an Ant.
 */
enum OpType {
	kProg3,	/**< (Nonterminal) 3-Tuple: Execute branches from left to right. */
	kProg2,	/**< (Nonterminal) 2-Tuple: Execute branches from left to right */
	kIfFoodAhead, /**< (Nonterminal) Check if there's food one space ahead */
	kMoveForward, /**< (Terminal) Move forward one space */
	kTurnLeft, /**< (Terminal) Turn left without moving from location */
	kTurnRight /**< (Terminal) Turn right without moving from location */
};