/**
 * @file	socket.h
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
 * This file contains functions to manipulate sockets and sockets structures.
 */
#ifndef _SOCKET_H_
#define _SOCKET_H_

#define UDP_MAX_LENGTH 512



/**
 * Sets an IP address structure.
 *
 * @param ip    A pointer to the IPv4 address.
 *
 * @return      The address structure.
 */
struct sockaddr_in setAddr(char * ip);

/**
 * Creates a socket, and initiate the connection if needed.
 *
 * @param protocol  The protocol to use with the socket.
 * @param dest      A pointer to the destination's address structure.
 *
 * @return          The socket file descriptor if the operation succeeded,
 *                  -1 otherwise.
 */
int initSock(int protocol, struct sockaddr_in * dest);

/**
 * Sends a request to a DNS server.
 * Note: The "dest" pointer should be NULL if a connection-mode socket is used.
 *
 * @param protocol  The protocol to use with the socket.
 * @param sockfd    The file descriptor of the socket to use.
 * @param buf       A pointer to the request to send.
 * @param len       The size of the request.
 * @param dest      A pointer to the destination's address structure.
 *
 * @return          The number of bytes sent if the operation succeeded,
 *                  -1 otherwise.
 */
int sendRequest(int protocol, int sockfd, char * buf, int len,
                struct sockaddr_in * dest);

/**
 * Receives a response from a DNS server.
 * Note: The buffer used to store the response is dynamically allocated.
 * Note: The destination's address structure will ve filled by the function.
 *
 * @param protocol      The protocol to use with the socket.
 * @param sockfd        The file descriptor of the socket to use.
 * @param buf_ptr_addr  A pointer to the pointer to the buffer.
 * @param from          A pointer to the destination's address structure.
 *
 * @return              The number of bytes received if the operation succeeded,
 *                      -1 otherwise.
 */
int recvResponse(int protocol, int sockfd, char ** buf_ptr_addr,
                    struct sockaddr_in * from);

/**
 * Contacts a DNS server to resolve a name or an IP address.
 *
 * @param protocol      The protocol to use with the socket.
 * @param sockfd        The file descriptor of the socket to use.
 * @param dns_query     A pointer to the request to send.
 * @param query_size    The size of the request.
 * @param dest          A pointer to the destination's address structure.
 * @param buf_ptr_addr  A pointer to the pointer to the response buffer.
 *
 * @return              The number of bytes received if the operation succeeded,
 *                      -1 otherwise.
 */
int contactServer(int protocol, int sockfd, char * dns_query, int query_size,
                    struct sockaddr_in * dest, char ** buf_ptr_addr);



#endif

