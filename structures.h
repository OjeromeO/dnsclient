/**
 * @file	structures.h
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
 * This file contains the structures describing a message using DNS protocol.
 */
#ifndef _STRUCTURES_H_
#define _STRUCTURES_H_



struct Dns_length
{
    unsigned short length;
};

struct Dns_Header
{
    unsigned short id;
    unsigned short  // flags in a diferent order than in DNS RFC, because of the
        rd: 1,      //  network byte order (big-endian) ; locally it's
        tc: 1,      //  little-endian ; and that contains bitfields, they are
        aa: 1,      //  put from the least significant bit of the least
        opcode: 4,  //  significant byte.
        qr: 1,
        rcode: 4,
        z: 3,
        ra: 1;
    unsigned short qdcount;
    unsigned short ancount;
    unsigned short nscount;
    unsigned short arcount;
};

struct Dns_Question
{
    unsigned short qtype;
    unsigned short qclass;
};

#pragma pack(push, 1)
struct Dns_RR
{
    unsigned short type;
    unsigned short class;
    unsigned int ttl;
    unsigned short rdlength;
};
#pragma pack(pop)



#endif

