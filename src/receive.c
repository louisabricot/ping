#include "../inc/ft_ping.h"

static void		reverse_lookup( s_host *host, int flag )
{
	int ret;

	ret = getnameinfo((struct sockaddr *)&(host->addr), sizeof(host->addr), host->reverse, HOST_LEN, NULL, 0, flag);
	if (ret != SUCCESS)
		error_exit("getnameinfo");
}

static int		parse_reply(s_session *session, s_echo *reply)
{
	s_host			host;
	struct timeval	arrived;
	struct timeval	rtt;
	struct timeval	sent;
	double			time;
	
	gettimeofday(&arrived, NULL);
	
	//Parse reply
	if (reply->icmphdr.un.echo.id != getpid()) 
		return (ERROR);
	if (reply->icmphdr.type == ICMP_ECHOREPLY)
	{
		//Reverse lookup source addr
		bzero(&host, sizeof(struct s_host));
		host.addr.sin_family = AF_INET;
		host.addr.sin_addr.s_addr = reply->iphdr.saddr;
		inet_ntop(AF_INET, &host.addr.sin_addr, host.ip, INET_ADDRSTRLEN);
		reverse_lookup(&host, session->opt.numeric);

		//Compute rtt
		memcpy(&sent, reply->data, sizeof(struct timeval));
		timersub(&arrived, &sent, &rtt);
		time = rtt.tv_sec * TO_MICROSECONDS + rtt.tv_usec;
	
		//Print info
		printf("%lu bytes from %s (%s): icmp_seq=%i ttl=%i time=%.2f ms\n", sizeof(s_echo) - sizeof(struct iphdr), host.reverse, host.ip, reply->icmphdr.un.echo.sequence, reply->iphdr.ttl, time / 1000 );

		//Update stats
		session->info.tsum += time;
		session->info.tmin = session->info.tmin == 0 ? time : session->info.tmin;
		session->info.tmin = session->info.tmin < time ? session->info.tmin : time;
		session->info.tmax = session->info.tmax < time ? time : session->info.tmax;

		session->info.nrecv++;
	} else {

		printf("received: code %i type %i\n", reply->icmphdr.code, reply->icmphdr.type );
	}
	return (SUCCESS);
}

int		receive_packet(s_session *session)
{
	struct msghdr	packet;
	struct iovec	iov;
	s_echo			reply;

	//Init reply struct
	bzero(&packet, sizeof(struct msghdr));
	bzero(&reply, sizeof(s_echo));
	packet.msg_name = &session->host.addr;
	packet.msg_namelen = sizeof(struct sockaddr_in);
	iov.iov_base = &reply;
	iov.iov_len = sizeof(s_echo);
	packet.msg_iov = &iov;
	packet.msg_iovlen = 1;

	//Receive packet
	if (recvmsg(session->fd, &packet, 0) == ERROR)
		return (ERROR);
	parse_reply(session, &reply);
	return (SUCCESS);
}
