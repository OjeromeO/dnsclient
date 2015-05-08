/**
 * @file	queries.c
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
/**
 * NOTE: valgrind (--track-origins=yes) finds an error on fillQname(),
 * on the strtok : "Use of uninitialised value of size 8".
 * But it seems not to be a real problem, because ip and tmp must be NULL
 * to work with strtok (so it's because of strtok).
 */
#include "global.h"
#include "structures.h"
#include "queries.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>



int fillQname(char * qname, char * s, int is_iquery)
{
    int countp = 0;             // count of '.'
    int ssize = strlen(s);
    int i = 0;
    int j = ssize;
    int finalsize = 0;          // size of the final string put into the query
    unsigned int toksize = 0;
    char * tmp = NULL;
    char * token = NULL;
    char * ip = NULL;
    char * ref = NULL;          // keep a reference to the allocated buffer
    
    finalsize = (is_iquery) ? (ssize + (int)strlen(".in-addr.arpa")) : ssize;
    
    tmp = malloc((finalsize+1) * sizeof(char));
    memset(tmp, '\0', finalsize);
    
    // put into tmp the final domain name to convert into DNS format
    if (is_iquery)
    {
        ip = malloc((ssize+1) * sizeof(char));
        memset(ip, '\0', ssize);
        
        ref = ip;
        
        memcpy(ip, s, ssize);
        
        // put into tmp the ip address, reversing the ip address
        for(i=0; i<4; i++)
        {
            token = strtok(ip, ".");
            toksize = strlen(token);
            
            memcpy(&tmp[j-toksize], token, toksize);
        
            ip = NULL;
            
            if (i < 3)
            {
                tmp[j-toksize-1] = '.';
            }
            
            j = j - toksize - 1;
        }
        
        free(ref);
        
        strcat(tmp, ".in-addr.arpa");
    }
    else
    {
        memcpy(tmp, s, ssize);
    }
    
    ref = tmp;
    
    for(i=0; i<finalsize; i++)
    {
        if (tmp[i] == '.')
        {
            countp++;
        }
    }
    
    token = NULL;
    toksize = 0;
    j = 0;
    
    // replace the '.' by the length of the following label
    for(i=0; i<=countp; i++)
    {
        token = strtok(tmp, ".");
        toksize = strlen(token);
        
        if (toksize > 63)
        {
            printf("%s: Error while filling the query \n", progname);
            printf("fillQname: Label too long \n");
            return 0;
        }
        
        qname[j] = toksize;
        memcpy(&qname[j+1], token, toksize);
        
        tmp = NULL;
        j = j + toksize + 1;
    }
    
    free(ref);
    
    return 1;
}

int fillRequest(int protocol, int is_iquery, char * name, char * dns_query)
{
    int type = 1;
    int tcp_bytes = 0;
    int query_size = -1;
    int iqry_bytes = 0;
    char * qname = NULL;
    struct Dns_length * l = NULL;
    struct Dns_Header * header = NULL;
    struct Dns_Question * question = NULL;
    
    if (protocol != IPPROTO_UDP && protocol != IPPROTO_TCP)
    {
        printf("%s: Error while filling the query \n", progname);
        printf("fillRequest: Bad protocol \n");
        return 0;
    }
    
    tcp_bytes = (protocol == IPPROTO_TCP) ? 2 : 0;
    
    if (is_iquery)
    {
        iqry_bytes = strlen(".in-addr.arpa");
        type = 12;
    }
    
    query_size = tcp_bytes + sizeof(struct Dns_Header) + strlen(name) +
                iqry_bytes + 2 + sizeof(struct Dns_Question);
    
    l = (struct Dns_length *) dns_query;
    header = (struct Dns_Header *) &dns_query[tcp_bytes];
    qname = &dns_query[tcp_bytes + sizeof(struct Dns_Header)];
    question = (struct Dns_Question *) &dns_query[tcp_bytes +
                sizeof(struct Dns_Header) + strlen(name) + iqry_bytes + 2];
    
    l->length = htons(query_size-2);
        
    // in udp, the header is written over the 2 tcp bytes, and it's allright
    header->id = htons(getpid());
    header->qr = 0;
    header->opcode = 0;
    header->aa = 0;
    header->tc = 0;
    header->rd = 1;
    header->ra = 0;
    header->z = 0;
    header->rcode = 0;
    header->qdcount = htons(1);
    header->ancount = 0;
    header->nscount = 0;
    header->arcount = 0;
        
    if (!fillQname(qname, name, is_iquery))
    {
        return 0;
    }
        
    question->qtype = htons(type);
    question->qclass = htons(1);
    
    return 1;
}

