#include "../inc/ft_ping.h"

/*
 * Implementation of the Internet checksum (see RFC 1071) to sign ICMP packets
 */

static unsigned short	internet_checksum(unsigned short *addr, size_t count)
{
	long sum;

	sum = 0;
	while (count > 1)
	{
		sum += *addr++;
		count -= sizeof(unsigned short);
	}

	if (count > 0) //odd
		sum += *(unsigned char *)addr;

	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);
	return (~sum);
}

/*
 *
 */

void		send_packet(s_session *session)
{
	int				nbytes;
	struct timeval	now;

	//Add sequence number and identifier
	session->echo.icmphdr.un.echo.sequence = ++(session->info.seq_n);

	//Add timestamp to echo data to easily match response
	bzero(session->echo.data, sizeof(ECHO_DATA_LEN));
	gettimeofday(&now, NULL);
	memcpy(session->echo.data, &now, sizeof(struct timeval));

	//Computee Internet checksum of ICMP header + data payload
	session->echo.icmphdr.checksum = 0;
	session->echo.icmphdr.checksum = internet_checksum((unsigned short *)&session->echo.icmphdr, sizeof(struct icmphdr) + ECHO_DATA_LEN);

	//Finally, send packet
	nbytes = sendto(session->fd, &session->echo, sizeof(s_echo), 0, (const struct sockaddr *)&session->host.addr, sizeof(struct sockaddr));
	if (nbytes == ERROR) {
		session->info.err++;
	} else {
		session->info.nsent++;
	}
}
