/**
 * @file	arguments.c
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
#include "global.h"
#include "arguments.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



int isIpv4(char * s)
{
    int i = 0;
    int lastp = -1;             // index of the last '.'
    int countp = 0;
    int size = (int)strlen(s);
    
    int ipVal[4];
    memset(ipVal, -1, 4*sizeof(int));
    
    if (size < 7 || size > 15 || s[0] == '.' || s[size-1] == '.')
    {
        return 0;
    }
    
    // count the '.' and find the last
    for(i=0; i<size; i++)
    {
        if (s[i] == '.')
        {
            lastp = i;
            countp++;
        }
    }
    
    if (countp != 3)
    {
        return 0;
    }
    
    // check if there is not a wrong value at the end of the string
    for(i=lastp+1; i<size; i++)
    {
        if (!(s[i] >= '0' && s[i] <= '9'))
        {
            return 0;
        }
    }
    
    sscanf(s, "%d.%d.%d.%d", &ipVal[0], &ipVal[1], &ipVal[2], &ipVal[3]);
    
    for(i=0; i<4; i++)
    {
    	if (ipVal[i]>255 || ipVal[i]<0)
    	{
    		return 0;
    	}
    }
    
    return 1;
}

int isIpv6(char * s)
{
    int i = 0;
    int j = 0;
    int k = 0;
    int lastp = -1;
    int countp = 0;
    int dpfound = 0;                // put to 1 if a ':' is found
    int size = (int)strlen(s);
    char * stmp = NULL;
    
    int ipVal[8];
    memset(ipVal, -1, 8*sizeof(int));
    
    if (size < 4 || size > 39 || s[0] == ':' || s[size-1] == ':')
    {
        return 0;
    }
    
    // count the ':', and find the last
    for(i=0; i<size; i++)
    {
        if (s[i] == ':')
        {
            lastp = i;
            countp++;
        }
    }
    
    if (countp > 7)
    {
        return 0;
    }
    
    // check if there is not a wrong value at the end of the string
    for(i=lastp+1; i<size; i++)
    {
        if (!(s[i] >= '0' && s[i] <= '9') && !(s[i] >= 'a' && s[i] <= 'f'))
        {
            return 0;
        }
    }
    
    // case of '::'
    stmp = malloc((size + (7-countp)*2 + 1) * sizeof(char));
    
    for(i=0; i<(size-1); i++)
    {
        stmp[j]=s[i];
        j++;
        if ((dpfound == 0) && (s[i] == ':') && (s[i+1] == ':'))
        {   
            dpfound = 1;             
            for(k=0; k<(7-countp); k++)
            {
                stmp[j]='0';
                j++;
                stmp[j]=':';
                j++;
            }
            stmp[j]='0';
            j++;
        }
    }
    stmp[j] = s[i];
     
    sscanf(stmp, "%x:%x:%x:%x:%x:%x:%x:%x", &ipVal[0], &ipVal[1], &ipVal[2],
            &ipVal[3], &ipVal[4], &ipVal[5], &ipVal[6], &ipVal[7]);
    
    for(i=0; i<8; i++)
    {
    	if (ipVal[i] > 0xffff || ipVal[i] < 0x0)
    	{
    		return 0;
    	}
    }
    
    free(stmp);
    return 1;
}

int isDomainName(char * s)
{
    int i = 0;
    int size = (int)strlen(s);
    int is_only_digits = 1;
    
    if (size == 0 || size > 253 || s[0] == '.' || s[size-1] == '.')
    {
        return 0;
    }
    
    // verify if there are only digits
    for(i=0; i<size; i++)
    {
        if (s[i] != '.')
        {
            if (!(s[i] >= '0' && s[i] <= '9'))
            {
                is_only_digits = 0;
                i = size;
            }
        }
    }
    
    if (is_only_digits)
    {
        return 0;
    }
    
    // verify there is not unallowed characters
    for(i=0; i<size; i++)
    {
        if (!(s[i] >= '0' && s[i] <= '9') && !(s[i] == '-') && !(s[i] == '.') &&
            !(s[i] >= 'a' && s[i] <= 'z') && !(s[i] >= 'A' && s[i] <= 'Z'))
        {
            return 0;
        }
    }
    
    // verify there is not ".."
    for(i=0; i<(size-1); i++)
    {
        if (s[i] == '.' && s[i+1] == '.')
        {
            return 0;
        }
    }
    
    return 1;
}

int checkArg(int argc, char ** argv)
{
    if (argc == 1 ||
        ((argc == 2 || argc == 3) && (strcmp(argv[1], "-p") == 0 ||
        strcmp(argv[1], "udp") == 0 || strcmp(argv[1], "tcp") == 0)))
    {
        printf("%s: Missing argument(s) \n", progname);
        printf("    Usage: %s [-p protocol] ip_address | domain_name\n",
                progname);
        return 0;
    }
    
    if (argc > 4)
    {
        printf("%s: Too many arguments \n", progname);
        printf("    Usage: %s [-p protocol] ip_address | domain_name\n",
                progname);
        return 0;
    }
    
    if ((argc == 2 && !isIpv4(argv[1]) && !isIpv6(argv[1]) &&
        !isDomainName(argv[1])) ||
        (argc == 4 && ((!isIpv4(argv[3]) && !isIpv6(argv[3]) &&
        !isDomainName(argv[3])) || (strcmp(argv[2], "udp") != 0 &&
        strcmp(argv[2], "tcp") != 0) || strcmp(argv[1], "-p") != 0)))
    {
        printf("%s: Invalid argument(s) \n", progname);
        return 0;
    }
    
    return 1;
}

