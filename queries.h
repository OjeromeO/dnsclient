/**
 * @file	queries.h
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
 * This file contains the functions to prepare DNS queries.
 */
#ifndef _QUERIES_H_
#define _QUERIES_H_



/**
 * Fills the question qname field with the resource to resolve, converting it to
 * the DNS format.
 *
 * @param qname     A pointer to the qname section.
 * @param s         The IP address or the domain name to resolve.
 * @param is_iquery A bool representing if the name is used for a standard (0)
 *                  or inverse (1) query.
 * @return          1 if the operation succeeded,
 *                  0 otherwise.
 */
int fillQname(char * qname, char * s, int is_iquery);

/**
 * Fills the DNS request (length, header, question);
 *
 * @param protocol  The protocol used to send/receive DNS messages.
 * @param is_iquery A bool representing if it's a result for a standard (0)
 *                  or inverse (1) query.
 * @param name      The IP address or the domain name to resolve.
 * @param dns_query A pointer to the request which will be sent.
 *
 * @return          1 if the operation succeeded,
 *                  0 otherwise.
 */
int fillRequest(int protocol, int is_iquery, char * name, char * dns_query);



#endif

