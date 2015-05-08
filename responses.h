/**
 * @file	responses.h
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
 * This file contains the functions to manipulate DNS responses,
 * and verify the data they contain.
 */
#ifndef _RESPONSES_H_
#define _RESPONSES_H_

#include "structures.h"



/**
 * Checks the DNS response header coherence.
 *
 * @param query_hdr     Header of the DNS query sent.
 * @param response_hdr  Header of the DNS response received.
 *
 * @return              1 if the rest of the DNS response is usable,
 *                      0 if it's not,
 *                      -1 if we have server failure code.
 */
int checkHeader(struct Dns_Header * query_hdr,
                struct Dns_Header * response_hdr);

/**
 * Checks the DNS response question coherence.
 *
 * @param query_qname       Name in the question of the DNS query sent.
 * @param query_qtn         Question of the DNS query sent.
 * @param response_qname    Name in the question of the DNS response received.
 * @param response_qtn      Question of the DNS response received.
 *
 * @return                  1 if the rest of the DNS response is usable,
 *                          0 otherwise.
 */
int checkQuestion(char * query_qname, struct Dns_Question * query_qtn,
            char * response_qname, struct Dns_Question * response_qtn);

/**
 * Finds the IP address requested, in the DNS response.
 *
 * @param protocol      The protocol used to send/receive DNS messages.
 * @param dns_response  The whole DNS response sent by the server.
 * @param resp_size     Size of the response received from the server.
 * @param dns_query     The whole DNS query sent to the server.
 * @param qry_size      Size of the query sent to the server.
 *
 * @return              A pointer to the requested IP/name in the response,
 *                      NULL if it is truncated or can't be found.
 */
char * findRes(int protocol, char * dns_response, int resp_size,
                char * dns_query, int qry_size);

/**
 * Prints the result, from the DNS Resource Record format into
 * a human-readable format.
 *
 * @param result    A pointer to the result.
 * @param is_iquery A bool representing if it's a result for a standard (0)
 *                  or inverse (1) query.
 */
void printRes(char * result, int is_iquery);



#endif

