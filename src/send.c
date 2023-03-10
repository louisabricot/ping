#include "../inc/ft_ping.h"

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

static void			set_icmphdr(struct icmphdr *icmphdr, s_session *session)
{
	icmphdr->type = ICMP_ECHO;
	icmphdr->code = 0;
	icmphdr->un.echo.sequence = ++(session->info.seq_n);
	
	//Compute Internet checksum of ICMP header + data payload
	session->echo.icmphdr.checksum = 0;
	session->echo.icmphdr.checksum = internet_checksum((unsigned short *)&session->echo.icmphdr, sizeof(struct icmphdr) + ECHO_DATA_LEN);
}

static void			set_echo_data(unsigned char *data)
{
	struct timeval	now;
	
	gettimeofday(&now, NULL);
	bzero(data, sizeof(ECHO_DATA_LEN));
	memcpy(data, &now, sizeof(struct timeval));
}

void		send_packet(s_session *session)
{
	set_echo_data(session->echo.data);
	set_icmphdr(&session->echo.icmphdr, session);

	if (sendto(session->fd, &session->echo, sizeof(s_echo), 0, (const struct sockaddr *)&session->host.addr, sizeof(struct sockaddr)) == ERROR)
		dprintf(STDERR_FILENO, "ft_ping: error sendto\n");
	else
		session->info.nsent++;
}
