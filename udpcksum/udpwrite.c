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

  struct ip *ip = (struct ip *) buf;
  struct udphdr *udp = (struct udphdr *) (buf + sizeof(struct ip));
  int ip_len = sizeof(struct ip) + sizeof(struct udphdr) + userlen;

      // fabricate the IP header
  ip->ip_hl   = sizeof(struct ip) >> 2;
  ip->ip_v    = IPVERSION;
  ip->ip_tos  = 0;
  ip->ip_len  = htons(ip_len);
  ip->ip_id   = 0;
  ip->ip_ttl  = TTL_OUT;
  ip->ip_p    = IPPROTO_UDP;

  ip->ip_src.s_addr  = ((struct sockaddr_in *) local)->sin_addr.s_addr;
  ip->ip_dst.s_addr  = ((struct sockaddr_in *) dest)->sin_addr.s_addr;

  ip->ip_sum  = 0;

  // Source port number
  udp->uh_sport = sock_get_port(local, sizeof(struct sockaddr_in));
  // Destination port number
  udp->uh_dport = sock_get_port(dest, sizeof(struct sockaddr_in));
  udp->uh_ulen = htons(sizeof(struct udphdr) + userlen);
  udp->uh_sum = 0;

  // Calculate the IP checksum
  ip->ip_sum = in_cksum((unsigned short *)buf,
                   sizeof(struct ip) + sizeof(struct udphdr));

	// TO DO: Calculate UDP checksum

  Sendto(rawfd, buf, ip_len, 0, (SA *) dest, destlen);
}
