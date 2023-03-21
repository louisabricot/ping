#include "../inc/ft_ping.h"

/*
 * Translate the socket address into a host name
 */

static void			reverse_lookup( s_host *host, s_echo *reply, int flag)
{
	host->addr.sin_family = AF_INET;
	host->addr.sin_addr.s_addr = reply->iphdr.saddr;

	if (!inet_ntop(AF_INET, &(host->addr.sin_addr), host->ip, INET_ADDRSTRLEN))
		dprintf(STDERR_FILENO, "ft_ping: inet_ntop\n");
	
	if (getnameinfo((struct sockaddr *)&(host->addr), sizeof(host->addr), host->reverse, NI_MAXHOST, NULL, 0, flag) != 0)
		dprintf(STDERR_FILENO, "ft_ping: getnameinfo\n");
}

static int			check_timestamp(struct timeval *unchecked, s_session *session)
{
	struct timeval	original;

	ft_memcpy(&original, session->echo.data, sizeof(struct timeval));
	if (timercmp(unchecked, &original, !=))
		return (ERROR);
	return (SUCCESS);
}

static int			parse_packet(s_session *session, s_echo *reply)
{
	s_host			host;
	struct timeval	unchecked;

	struct timeval	now;
	struct timeval	rtt;
	double			time;
	
	gettimeofday(&now, NULL);
	
	//Reverse lookup source addr
	ft_bzero(&host, sizeof(struct s_host));
	reverse_lookup(&host, reply, session->opt.numeric);

	if (reply->icmphdr.type == ICMP_ECHOREPLY)
	{
		if (reply->icmphdr.un.echo.sequence != session->echo.icmphdr.un.echo.sequence)
			return (ERROR);
		
		//Check the received timestamp is the same as the one sent
		ft_memcpy(&unchecked, reply->data, sizeof(struct timeval));
		if (check_timestamp(&unchecked, session) == ERROR)
			return (ERROR);
		
		//Compute Round Trip Time
		timersub(&now, &unchecked, &rtt);
		time = rtt.tv_sec * TO_MICROSECONDS + rtt.tv_usec;

		printf("%lu bytes from %s (%s): icmp_seq=%i ttl=%i time=%.2f ms\n", sizeof(s_echo) - sizeof(struct iphdr), host.reverse, host.ip, reply->icmphdr.un.echo.sequence, reply->iphdr.ttl, time / 1000 );

		//Update stats
		session->info.tsum += time;
		session->info.tmin = session->info.tmin == 0 ? time : session->info.tmin;
		session->info.tmin = session->info.tmin < time ? session->info.tmin : time;
		session->info.tmax = session->info.tmax < time ? time : session->info.tmax;

		session->info.nrecv++;
	} else if (session->opt.verbose) {

		printf("%lu bytes from %s: type %i code %i\n", sizeof(s_echo) - sizeof(struct iphdr), host.ip, reply->icmphdr.type, reply->icmphdr.code );
		session->info.err++;
	}
	return (SUCCESS);
}

void	setup_msghdr(struct msghdr *msg, struct iovec *iov, s_host *host)
{

	ft_bzero(msg, sizeof(struct msghdr));
	msg->msg_name = &host->addr;
	msg->msg_namelen = sizeof(struct sockaddr_in);
	msg->msg_iov = iov;
	msg->msg_iovlen = 1;
}

void	receive_packet(s_session *session)
{
	struct msghdr	response;
	struct iovec	iov;
	s_echo			packet;

	ft_bzero(&packet, sizeof(s_echo));
	iov.iov_base = &packet;
	iov.iov_len = sizeof(s_echo);
	setup_msghdr(&response, &iov, &session->host);
	if (recvmsg(session->fd, &response, 0) == ERROR)
		dprintf(STDERR_FILENO, "ft_ping: recvmsg\n");
	parse_packet(session, &packet);
}
