/**
 * @file	responses.c
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
#include "global.h"
#include "responses.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>



int checkHeader(struct Dns_Header * query_hdr, struct Dns_Header * response_hdr)
{
    int code = response_hdr->rcode;
    
    if (response_hdr->id != query_hdr->id || response_hdr->qr != 1 ||
        response_hdr->opcode != 0 || response_hdr->rd != 1 ||
        response_hdr->z != 0 || response_hdr->qdcount != htons(1))
    {
        printf("%s: Error while reading the response \n", progname);
        printf("checkHeader: Unexpected DNS header \n");
        return 0;
    }
    
    if (response_hdr->ancount == 0 && code == 0)
    {
        printf("%s: Error while reading the response \n", progname);
        printf("checkHeader: Unexpected DNS header \n");
        return 0;
    }
    
    if (code != 0)
    {
        if (code >= 6 && code <= 15)
        {
            printf("%s: Error while reading the response \n", progname);
            printf("checkHeader: Unexpected DNS header \n");
        }
        
        switch (code)
        {
            case 1:
                printf(" >>> DNS query format error \n");
                break;
            case 2:
                printf(" >>> Internal DNS server failure \n");
                return -1;
                break;
            case 3:
                printf(" >>> Domain name doesn't exist \n");
                break;
            case 4:
                printf(" >>> Type of query not supported by DNS server \n");
                break;
            case 5:
                printf(" >>> Requested operation refused by DNS server \n");
        }
        
        return 0;
    }

    return 1;
}

int checkQuestion(char * query_qname, struct Dns_Question * query_qtn,
            char * response_qname, struct Dns_Question * response_qtn)
{
    if (strcmp(query_qname, response_qname) != 0 || memcmp(query_qtn,
        response_qtn, sizeof(struct Dns_Question)))
    {
        printf("%s: Error while reading the response \n", progname);
        printf("checkHeader: Unexpected question section \n");
        return 0;
    }
    
    return 1;
}

char * findRes(int protocol, char * dns_response, int resp_size,
                char * dns_query, int qry_size)
{
    int i = 0;
    int tcp_bytes = 0;
    int prev_size = 0;                      // size already read
    int aname_size = 0;
    char * qry_qname = NULL;
    char * rsp_qname = NULL;
    char * rsp_aname = NULL;
    struct Dns_Header * qry_header = NULL;
    struct Dns_Question * qry_question = NULL;
    struct Dns_Header * rsp_header = NULL;
    struct Dns_Question * rsp_question = NULL;
    struct Dns_RR * rsp_answer = NULL;
    
    if (protocol != IPPROTO_UDP && protocol != IPPROTO_TCP)
    {
        printf("%s: Error while reading the DNS response \n", progname);
        printf("findRes: Bad protocol \n");
        return NULL;
    }    
    
    tcp_bytes = (protocol == IPPROTO_TCP) ? 2 : 0;
    
    qry_header = (struct Dns_Header *) &dns_query[tcp_bytes];
    qry_qname = &dns_query[tcp_bytes + sizeof(struct Dns_Header)];
    qry_question = (struct Dns_Question *) &dns_query[qry_size -
                    sizeof(struct Dns_Question)];
    
    rsp_header = (struct Dns_Header *) &dns_response[tcp_bytes];
    rsp_qname = &dns_response[tcp_bytes + sizeof(struct Dns_Header)];
    rsp_question = (struct Dns_Question *) &dns_response[qry_size -
                    sizeof(struct Dns_Question)];
    
    if (resp_size <= (qry_size + 2 + (int)sizeof(struct Dns_Question)))
    {
        printf("%s: Error while reading the response \n", progname);
        printf("findRes: Incomplete DNS response \n");
        return NULL;
    }
    
    if (!checkHeader(qry_header, rsp_header) ||
        !checkQuestion(qry_qname, qry_question, rsp_qname, rsp_question))
    {
        return NULL;
    }
    
    prev_size = qry_size;
    
    // loop to look all the response received
    for(i=0; i<ntohs(rsp_header->ancount); i++)
    {
        if (resp_size <= (prev_size + 2 + (int)sizeof(struct Dns_Question)))
        {
            printf("%s: Error while reading the response \n", progname);
            printf("findRes: Incomplete DNS response \n");
            return NULL;
        }
        
        rsp_aname = &dns_response[prev_size];
        
        if ((aname_size = strlen(rsp_aname)) < 2 ||
            resp_size <= (prev_size + aname_size + (int)sizeof(struct Dns_RR)))
        {
            printf("%s: Error while reading the response \n", progname);
            printf("findipres: Incomplete DNS response \n");
            return NULL;
        }
        
        rsp_answer = (struct Dns_RR *) &dns_response[prev_size + aname_size];
        
        if (resp_size < (prev_size + aname_size + (int)sizeof(struct Dns_RR) +
                         ntohs(rsp_answer->rdlength)))
        {
            printf("%s: Error while reading the response \n", progname);
            printf("findipres: Incomplete DNS response \n");
            return NULL;
        }
        
        if (rsp_answer->class == ntohs(1) &&
            (rsp_answer->type == ntohs(1) || rsp_answer->type == ntohs(12)))
        {
            return &dns_response[prev_size + aname_size +
                                sizeof(struct Dns_RR)];
        }
        
        prev_size = prev_size + aname_size + sizeof(struct Dns_RR) +
                       ntohs(rsp_answer->rdlength);
    }
    
    printf("%s: Error while reading the response \n", progname);
    printf("findRes: Unable to find IP Ressource Record \n");
    
    return NULL;
}

void printRes(char * result, int is_iquery)
{
    int i = 0;
    int end = 0;
    int countlabel = 0;
    
    char domain_name[256];
    memset(domain_name, '\0', 256);
    
    if (result == NULL)
    {
        printf(" >>> No result in the response \n");
    }
    
    if (!is_iquery)
    {
        printf(" >>> IPv4 address: %d.%d.%d.%d \n",
                (unsigned char)result[0],
                (unsigned char)result[1],
                (unsigned char)result[2],
                (unsigned char)result[3]);
    }
    else
    {
        // copy the result and put a '.' when the char represents a length byte
        while(!end)
        {
            if (i != countlabel)
            {
                memcpy(&domain_name[i], &result[i], 1);
            }
            else
            {
                domain_name[countlabel] = '.';
                countlabel = i + result[i]+1;
            }
            
            i++;
            
            end = (result[i] == '\0') ? 1 : 0 ;
        }
        
        printf(" >>> Domain name: %s \n", &(domain_name[1]));
    }
    
    return;
}

