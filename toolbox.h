/**
 * @file	toolbox.h
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
 * This file contains useful variables, functions or structures for the project.
 */
#ifndef _TOOLBOX_H_
#define _TOOLBOX_H_



/**
 * Clean and free the memory and file descriptors used.
 *
 * @param sockfd        The file descriptor of the socket used.
 * @param dns_query     A pointer to the request buffer used.
 * @param dns_resp      A pointer to the response buffer used.
 */
void cleanMem(int sockfd, char * dns_query, char * dns_resp);



#endif

