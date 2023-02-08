#include "../inc/ft_ping.h"

void	setup_echo(s_session *session)
{
	//Fill IP header
	session->echo.iphdr.version = IPVERSION;
	session->echo.iphdr.ihl = IPHDR_IHL;
	session->echo.iphdr.tot_len = sizeof(s_echo);
	session->echo.iphdr.ttl = DEFAULT_LINUX_TTL;
	session->echo.iphdr.protocol = IPPROTO_ICMP;
	session->echo.iphdr.check = 0; //filled by kernel
	session->echo.iphdr.saddr = INADDR_ANY;
	session->echo.iphdr.daddr = session->host.addr.sin_addr.s_addr;

	//Fill ICMP header
	session->echo.icmphdr.type = ICMP_ECHO;
	session->echo.icmphdr.code = 0;
}
