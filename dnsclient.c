/**
 * @file	main.c
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
 * This file contains the main function of the program.
 */
#include "structures.h"
#include "toolbox.h"
#include "socket.h"
#include "queries.h"
#include "responses.h"
#include "arguments.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>



/**
 * The name of the main program.
 */
char progname[] = "dnsclient";



/**
 * Entry point of the program.
 * 
 * @param argc  The count of command-line arguments.
 * @param argv  An IP address or a DNS name to resolute.
 *
 * @return      EXIT_SUCCESS if good ending of the programm,
 *              EXIT_FAILURE else.
 */
int main(int argc, char ** argv)
{
    int i = 0;
    int protocol = 0;
    int tcp_bytes = 0;
    int iqry_bytes = 0;
    int sockfd = 0;
    int ipv4 = 0;               // put to 1 if the argument is an IPv4 address
    int ipv6 = 0;               // put to 1 if the argument is an IPv6 address
    int domain_name = 0;        // put to 1 if the argument is a domain name
    int query_size = 0;
    int resp_size = 0;
    char * primary = "8.8.8.8";     // primary DNS server (Google)
    char * secundary = "8.8.4.4";   // secundary DNS server (Google)
    char * dns_query = NULL;
    char * dns_response = NULL;
    char * result = NULL;
    struct sockaddr_in dest;
    struct Dns_Header * rsp_header = NULL;
    
    if (!checkArg(argc, argv))
    {
        return EXIT_FAILURE;
    }
    
    // by default, access to the server using UDP
    if (argc == 2 || (argc == 4 && strcmp(argv[2], "udp") == 0))
    {
        protocol = IPPROTO_UDP;
        tcp_bytes = 0;
    }
    else
    {
        protocol = IPPROTO_TCP;
        tcp_bytes = 2;
    }
    
    if (isIpv4(argv[argc-1]))
    {
        ipv4 = 1;
        iqry_bytes = strlen(".in-addr.arpa");
    }
    else if (isIpv6(argv[argc-1]))
    {
        ipv6 = 1;
        iqry_bytes = strlen(".in-addr.arpa");
    }
    else
    {
        domain_name = 1;
        iqry_bytes = 0;
    }
    
    if (ipv6)
    {
        printf("IPv6 support not yet implemented at that time. \n");
        return EXIT_FAILURE;
    }
    
    query_size = tcp_bytes + sizeof(struct Dns_Header) + strlen(argv[argc-1]) +
                iqry_bytes + 2 + sizeof(struct Dns_Question);
    
    dns_query = malloc(query_size * sizeof(char));
    memset(dns_query, '\0', query_size);
    
    if (!fillRequest(protocol, !domain_name, argv[argc-1], dns_query))
    {
        cleanMem(sockfd, dns_query, dns_response);
        
        return EXIT_FAILURE;
    }
    
    // loop if the primary DNS server failed, and use the secundary DNS server
    for(i=0; i<2; i++)
    {
        dest = (i == 0) ? setAddr(primary) : setAddr(secundary);
        
        if ((sockfd = initSock(protocol, &dest)) == -1)
        {
            cleanMem(sockfd, dns_query, dns_response);
                
            return EXIT_FAILURE;
        }
    
        if ((resp_size = contactServer(protocol, sockfd, dns_query, query_size,
            &dest, &dns_response)) == -1)
        {
            cleanMem(sockfd, dns_query, dns_response);
            
            return EXIT_FAILURE;
        }
        
        rsp_header = (struct Dns_Header *) &dns_response[tcp_bytes];
        
        if (rsp_header->rcode != 2)
        {
            i++;
        }
        else if (i == 0)
        {
            printf("Retry with the secundary DNS server \n");
        }
    }
    
    result = findRes(protocol, dns_response, resp_size, dns_query, query_size);
    
    if (result == NULL)
    {
        cleanMem(sockfd, dns_query, dns_response);
        
        return EXIT_FAILURE;
    }
    
    printRes(result, !domain_name);
    
    cleanMem(sockfd, dns_query, dns_response);
    
    return EXIT_SUCCESS;
}

