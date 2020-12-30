#include	"udpcksum.h"

/* include open_output_raw */
int		rawfd;			/* raw socket to write on */

void
open_output(void)
{
	int	on=1;
	/*
	 * Need a raw socket to write our own IP datagrams to.
	 * Process must have superuser privileges to create this socket.
	 * Also must set IP_HDRINCL so we can write our own IP headers.
	 */

	rawfd = Socket(dest->sa_family, SOCK_RAW, IPPROTO_UDP); // Change (by pcsegal): Original version had 0 instead of IPPROTO_UDP, but this doesn't work on Ubuntu 18.04.

	Setsockopt(rawfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));
}
/* end open_output_raw */

/* Change (by pcsegal): Rewrote udp_write to use udphdr and iphdr instead of udpiphdr, since udpiphdr doesn't work on Linux.
Note: this version doesn't compute the UDP checksum, leaving it at zero instead. */
void udp_write(char *buf, int userlen) {

  struct iphdr *ip = (struct iphdr *) buf;
  struct udphdr *udp = (struct udphdr *) (buf + sizeof(struct iphdr));

      // fabricate the IP header
  ip->ihl      = sizeof(struct iphdr) >> 2;
  ip->version  = IPVERSION;
  ip->tos      = 0;
  ip->tot_len  = sizeof(struct iphdr) + sizeof(struct udphdr) + userlen;
  ip->id       = 0;
  ip->ttl      = TTL_OUT;
  ip->protocol = IPPROTO_UDP;

  ip->saddr = ((struct sockaddr_in *) local)->sin_addr.s_addr;
  ip->daddr = ((struct sockaddr_in *) dest)->sin_addr.s_addr;

  ip->check = 0;

  // Source port number
  udp->source = sock_get_port(local, sizeof(struct sockaddr_in));
  // Destination port number
  udp->dest = sock_get_port(dest, sizeof(struct sockaddr_in));
  udp->len = htons(sizeof(struct udphdr) + userlen);
  udp->check = 0;

  // Calculate the IP checksum
  ip->check = in_cksum((unsigned short *)buf,
                   sizeof(struct iphdr) + sizeof(struct udphdr));

	// TO DO: Calculate UDP checksum

  Sendto(rawfd, buf, ip->tot_len, 0, (SA *) dest, destlen);
}
