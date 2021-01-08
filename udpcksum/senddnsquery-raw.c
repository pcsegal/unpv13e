#include	"udpcksum.h"

/*
 * Build a DNS A query for "a.root-servers.net" and write it to
 * the raw socket.
 */

/* Change (by pcsegal): Use ip and udphdr instead of udpiphdr. */

#define IP_UDP_HSIZ    (sizeof(struct ip) + sizeof(struct udphdr))

/* include send_dns_query */
void
send_dns_query(void)
{
	size_t		nbytes;
	char		*buf, *ptr;

	buf = Malloc(IP_UDP_HSIZ + 100);
	ptr = buf + IP_UDP_HSIZ;  /* leave room for IP/UDP headers */

	*((uint16_t *) ptr) = htons(1234);	/* identification */
	ptr += 2;
	*((uint16_t *) ptr) = htons(0x0100);	/* flags: recursion desired */
	ptr += 2;
	*((uint16_t *) ptr) = htons(1);		/* # questions */
	ptr += 2;
	*((uint16_t *) ptr) = 0;			/* # answer RRs */
	ptr += 2;
	*((uint16_t *) ptr) = 0;			/* # authority RRs */
	ptr += 2;
	*((uint16_t *) ptr) = 0;			/* # additional RRs */
	ptr += 2;

	memcpy(ptr, "\001a\014root-servers\003net\000", 20);
	ptr += 20;
	*((uint16_t *) ptr) = htons(1);		/* query type = A */
	ptr += 2;
	*((uint16_t *) ptr) = htons(1);		/* query class = 1 (IP addr) */
	ptr += 2;

	nbytes = (ptr - buf) - IP_UDP_HSIZ;
	udp_write(buf, nbytes);
	if (verbose)
		printf("sent: %d bytes of data\n", nbytes);
}
/* end send_dns_query */
