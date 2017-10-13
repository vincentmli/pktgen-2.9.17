/*-
 * Copyright (c) <2010>, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 *
 * - Neither the name of Intel Corporation nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * Copyright (c) <2010-2014>, Wind River Systems, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 * 1) Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3) Neither the name of Wind River Systems nor the names of its contributors may be
 * used to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * 4) The screens displayed by the application must contain the copyright notice as defined
 * above and can not be removed without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/* Created 2010 by Keith Wiles @ intel.com */

#include "pktgen.h"

#include "pktgen-dns.h"
#include "xorshift128plus.h"

/**************************************************************************//**
 *
 * set_dns_name_format - Set the dns name format  string.
 *
 * DESCRIPTION
 * Set the given dns name format string. This will convert www.google.com to 3www6google3com
 *
 * RETURNS: N/A
 *
 */

static void set_dns_name_format(char* qname, char* host)
{

        char *bungle, *x;
        char elem[128];

        *qname = 0;
        bungle = rte_malloc(NULL,strlen(host) + 3,0);
        rte_memcpy(bungle, host, strlen(host) + 3);
        x = strtok(bungle, ".");
        while (x != NULL) {
                if (snprintf(elem, 128, "%c%s", (char)strlen(x), x) == 128) {
                        puts("String overflow.");
                        exit(1);
                }
                strncat(qname, elem, strlen(elem) + 1);
                x = strtok(NULL, ".");
        }
        rte_free(bungle);

}


/**************************************************************************//**
 *
 * pktgen_dns_hdr_ctor -  DNS header constructor routine.
 *
 * DESCRIPTION
 * Construct the DNS header in a packer buffer.
 *
 * RETURNS: N/A
 *
 * SEE ALSO:
 */

void
pktgen_dns_hdr_ctor(port_info_t *info, dnsHdr_t *duip, uint16_t ipProto)
{
        unsigned char *buffer = (unsigned char *)duip;
        unsigned int l4HdrSize = 0;
	char *qname;

        if (ipProto == PG_IPPROTO_UDP)
                l4HdrSize = sizeof(udpHdr_t);
        else
                l4HdrSize = sizeof(tcpHdr_t);

        dnsHdr_t *dnsHdr =
                (dnsHdr_t *)(buffer + sizeof(ipHdr_t) + l4HdrSize);

	/* Zero out the header space */
	memset((char *)dnsHdr, 0, sizeof(dnsHdr_t));

	/* Create the DNS header */
	dnsHdr->id      = htons(random_16());
	dnsHdr->flags   = htons(0x0100);
	dnsHdr->qdcount	= htons(1);
	dnsHdr->ancount	= 0;
	dnsHdr->nscount	= 0;
	dnsHdr->arcount	= 0;

	/* fill in dns query name */
	qname = (char*)(buffer + sizeof(ipHdr_t) + l4HdrSize + sizeof(dnsHdr_t));
	set_dns_name_format(qname , info->dns_domain);

	/* fill in dns query type and class */
        dnsQuery_t *dnsQuery =
                (dnsQuery_t *)(buffer + sizeof(ipHdr_t) + l4HdrSize + sizeof(dnsHdr_t) + (strlen((const char*)qname) + 1));
  	dnsQuery->qtype = htons (0x1);
  	dnsQuery->qclass = htons (0x1);

#if 0

	/* Create the DNS Query */
	dnsQuery->count1 = 0x6;
	dnsQuery->count2 = 0x3;
	dnsQuery->ch7 = 'o';
	dnsQuery->ch8 = 'r';
  	dnsQuery->ch9 = 'g';
  	dnsQuery->null = htons(0x0);
  	dnsQuery->qtype = htons (0x1);
  	dnsQuery->qclass = htons (0x1);


	dnsQuery->ch1 = random_ch();
	dnsQuery->ch2 = random_ch();
	dnsQuery->ch3 = random_ch();
	dnsQuery->ch4 = random_ch();
	dnsQuery->ch5 = random_ch();
	dnsQuery->ch6 = random_ch();
#endif
}
