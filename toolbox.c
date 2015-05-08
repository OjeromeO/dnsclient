/**
 * @file	toolbox.c
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
#include "global.h"
#include "toolbox.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



void cleanMem(int sockfd, char * dns_query, char * dns_resp)
{
    free(dns_query);
    free(dns_resp);
            
    if (sockfd != 0 && close(sockfd) == -1)
    {
        printf("%s: Error while closing the connection \n", progname);
        perror("cleanMem: close");
    }
}

