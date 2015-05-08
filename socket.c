/**
 * @file	socket.c
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
#include "global.h"
#include "socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>



struct sockaddr_in setAddr(char * ip)
{
    struct sockaddr_in dest;
    
    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    
    inet_aton(ip, &(dest.sin_addr));
    
    return dest;
}

int initSock(int protocol, struct sockaddr_in * dest)
{
    int sockfd = -1;
    int sock_type = -1;
    
    if (protocol != IPPROTO_UDP && protocol != IPPROTO_TCP)
    {
        printf("%s: Error while creating the connection \n", progname);
        printf("initSock: Bad protocol \n");
        return -1;
    }
    
    sock_type = (protocol == IPPROTO_UDP) ? SOCK_DGRAM : SOCK_STREAM;
    
    if ((sockfd = socket(AF_INET, sock_type, protocol)) == -1)
    {
        printf("%s: Error while creating the connection \n", progname);
        perror("initSock: socket");
        return -1;
    }
    
    if (protocol == IPPROTO_TCP)
    {
        if (connect(sockfd, (struct sockaddr*)dest, sizeof(struct sockaddr_in))
            == -1)
        {
	        printf("%s: Error while initiating the connection \n", progname);
            perror("initSock: connect");
            
            if (close(sockfd) == -1)
            {
                printf("%s: Error while closing the connection \n", progname);
                perror("initSock: close");
            }
            
            return -1;    
        }
    }
    
    return sockfd;
}

int sendRequest(int protocol, int sockfd, char * buf, int len,
                struct sockaddr_in * dest)
{
    int sent = -1;
    
    if (protocol == IPPROTO_TCP)
    {
        sent = send(sockfd, buf, len, 0);
    }
    else if (protocol == IPPROTO_UDP)
    {
        sent = sendto(sockfd, buf, len, 0, (struct sockaddr*)dest, 
                    sizeof(struct sockaddr_in));
    }
    else
    {
        printf("%s: Error while sending the DNS query \n", progname);
        printf("sendRequest: Bad protocol \n");
        return -1;
    }
    
    if (sent == -1)
    {
        printf("%s: Error while sending the DNS query \n", progname);
        perror("sendRequest: sendto");
                
        if (close(sockfd) == -1)
        {
            printf("%s: Error while closing the connection \n", progname);
            perror("sendRequest: close");
        }
    }
            
    return sent;
}

int recvResponse(int protocol, int sockfd, char ** buf_ptr_addr,
                  struct sockaddr_in * from)
{
    socklen_t fromlen = sizeof(struct sockaddr_in);
    int maxsize = UDP_MAX_LENGTH;
    int resp_size = 0;
    int last_size = 0;
    unsigned short tcp_resp_size = 0;
    int i = 1;
    
    *buf_ptr_addr = malloc(maxsize * sizeof(char));
    memset(*buf_ptr_addr, '\0', maxsize);
    
    if (protocol != IPPROTO_UDP && protocol != IPPROTO_TCP)
    {
        printf("%s: Error while receiving the DNS response \n", progname);
        printf("recvResponse: Bad protocol \n");
        return -1;
    }
    
    if (protocol == IPPROTO_UDP)
    {
        // the response (or its interesting part for us) length won't be greater
        // than 512, because of the message compression
        last_size = recvfrom(sockfd, *buf_ptr_addr, maxsize, 0,
                            (struct sockaddr *)from, &fromlen);
        
        if (last_size == 0 || last_size == -1)
        {
            printf("%s: Error while receiving the DNS response \n", progname);
            printf("recvResponse: ");
            
            if (last_size == 0)
            {
                printf("recvfrom: The DNS server closed the connection \n");
            }
            else
            {
                perror("recvfrom");
            }
            
            if (close(sockfd) == -1)
            {
                printf("%s: Error while closing the connection \n", progname);
                perror("recvResponse: close");
            }
            
            return -1;
        }
        
        resp_size += last_size;
    }
    
    if (protocol == IPPROTO_TCP)
    {
        do
        {
            if (resp_size != 0)
            {
                *buf_ptr_addr = realloc(*buf_ptr_addr, i*maxsize*sizeof(char));
            }
            
            last_size = recv(sockfd, &(*buf_ptr_addr)[maxsize*(i-1)],
                            maxsize, 0);
            
            if (last_size == 0 || last_size == -1)
            {
                printf("%s: Error while receiving the DNS response \n",
                        progname);
                printf("recvResponse: ");
                
                if (last_size == 0)
                {
                    printf("recv: The DNS server closed the connection \n");
                }
                else
                {
                    perror("recv");
                }
                
                if (close(sockfd) == -1)
                {
                    printf("%s: Error while closing the connection \n",
                            progname);
                    perror("recvResponse: close");
                }
            
                return -1;
            }
            
            resp_size += last_size;
            i++;
            
            memcpy(&tcp_resp_size, *buf_ptr_addr, 2);
        }   // if we haven't received all the response
        while((resp_size-2) != ntohs(tcp_resp_size));
    }
    
    return resp_size;
}

int contactServer(int protocol, int sockfd, char * dns_query, int query_size,
                    struct sockaddr_in * dest, char ** buf_ptr_addr)
{
    int i = 0;
    int end = 0;
    int nb = 0;
    int resp_size = 0;
    fd_set readfds;
    struct timeval timeout;
    struct sockaddr_in from;
    
    if (protocol != IPPROTO_TCP && protocol != IPPROTO_UDP)
    {
        printf("%s: Error while sending the DNS query \n", progname);
        printf("contactServer: Bad protocol \n");
        return -1;
    }
    
    if (protocol == IPPROTO_TCP)
    {
        if (sendRequest(protocol, sockfd, dns_query, query_size, dest) == -1)
        {
            return -1;
        }
            
        if ((resp_size = recvResponse(protocol, sockfd, buf_ptr_addr, &from))
            == -1)
        {
            return -1;
        }
        
        return resp_size;
    }
    
    // if we reach this place we are using the UDP protocol
    while(i < 3 && !end)
    {
        if (sendRequest(protocol, sockfd, dns_query, query_size, dest) == -1)
        {
            return -1;
        }
        
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        
        // on Linux, select() modify timeout
        if (i == 0)
        {
            timeout.tv_sec = 0;
            timeout.tv_usec = 500000;
        }
        
        if (i == 1)
        {
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
        }
        
        if (i == 2)
        {
            timeout.tv_sec = 2;
            timeout.tv_usec = 0;
        }
        
        nb = select(sockfd+1, &readfds, NULL, NULL, &timeout);
        
        if (nb == -1)
        {
            printf("%s: Error while waiting for the DNS response \n", progname);
            perror("contactServer: select");
            return -1;
        }
        
        // if select didn't return because of timeout
        if (nb != 0 && FD_ISSET(sockfd, &readfds))
        {
            if ((resp_size = recvResponse(protocol, sockfd, buf_ptr_addr,
                &from)) == -1)
            {
                return -1;
            }
            
            // if it's a message from the server
            if (memcmp(&(dest->sin_addr), &(from.sin_addr),
                sizeof(struct in_addr)) == 0)
            {
                end = 1;
            }
            else
            {
                memset(buf_ptr_addr, '\0', resp_size);
            }
        }
        
        i++;
    }
    
    if (!end)
    {
        printf("%s: Error while waiting for the DNS response \n", progname);
        printf("contactServer: No response received: it may have been lost, ");
        printf("or maybe the network is overloaded \n");
    }
    
    return (end) ? resp_size : -1 ;
}

