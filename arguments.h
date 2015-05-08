/**
 * @file	arguments.h
 * @version	1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This file contains functions to make some verifications about the program
 * arguments.
 */
#ifndef _ARGUMENTS_H_
#define _ARGUMENTS_H_



/**
 * Tests if a string represents an ipv4 address.
 *
 * @param s The string to test.
 *
 * @return  1 if it's an ipv4 address,
 *          0 otherwise.
 */
int isIpv4(char * s);

/**
 * Tests if a string represents an ipv6 address.
 *
 * @param s The string to test.
 *
 * @return  1 if it's an ipv6 address,
 *          0 otherwise.
 */
int isIpv6(char * s);

/**
 * Tests if a string represents a domain name.
 *
 * @param s The string to test.
 *
 * @return  1 if it's a domain name,
 *          0 otherwise.
 */
int isDomainName(char * s);

/**
 * Checks the program arguments coherence.
 *
 * @param argc      The count of arguments.
 * @param argv      The list of arguments.
 *
 * @return          1 if the arguments are usable,
 *                  0 otherwise.
 */
int checkArg(int argc, char ** argv);



#endif

