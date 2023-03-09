#include "../inc/ft_ping.h"

/*
 * Creates a RAW socket and set socket options
 * IP_HDRINCL is set to prevent the kernel from adding IP header
 * SO_RCVTIMEO is set with timeout value to enable timeout option
 */

static void			setup_socket(s_session *session)
{
	int				on;

	//Create raw socket
	session->fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (session->fd == ERROR)
		error_exit("socket");
	
	//Stop kernel from adding IP header
	on = 1;
	if (setsockopt(session->fd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) != SUCCESS) {
		close(session->fd);
		error_exit("setsockopt");
	}

	//Add a time out to recvmsg()
	if (setsockopt(session->fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&session->opt.timeout, sizeof(struct timeval)) != SUCCESS) {
		close(session->fd);
		error_exit("setsockopt");
	}
}

/*
 * Converts the hostname into an Ipv4 and socket address
 */

void					resolve_host(s_host *host)
{
	struct addrinfo		hints;
	struct addrinfo		*res;

	bzero(&hints, sizeof(struct addrinfo));

	//Resolve hostname
	hints.ai_family = AF_INET;
	if (getaddrinfo(host->name, NULL, &hints, &res) != SUCCESS)
		error_exit("getaddrinfo");
	
	//Store IPv4 address
	host->addr = *(struct sockaddr_in *)res->ai_addr;

	//Convert IPv4 address into text
	inet_ntop(AF_INET, &(host->addr.sin_addr), host->ip, INET_ADDRSTRLEN);
	freeaddrinfo(res);
}

/*
 * Parses the program's arguments and set the session struct accordingly
 * If more than one hostname are passed, only the first hostname is taken into account
 * If an unknown option is parsed, the program prints usage and exit
 */

static void				parse_options(int ac, char **av, s_session *session)
{
	unsigned long		deadline;
	double				timeout;
	double				interval;
	int					opt;

	while ((opt = ft_getopt(ac, av, OPTIONS)) != -1)
	{
		switch (opt)
		{
			case COUNT_OPTION:
				session->opt.count = parseul(ft_optarg, COUNT_MIN, COUNT_MAX);
				break ;
			case NUMERIC_OPTION:
				session->opt.numeric = NI_NUMERICHOST;
				break ;
			case DEADLINE_OPTION:
				deadline = parseul(ft_optarg, DEADLINE_MIN, DEADLINE_MAX);
				dtotimeval(deadline, &session->opt.deadline);
				break ;
			case TIMEOUT_OPTION:
				timeout = parsed(ft_optarg, TIMEOUT_MIN, TIMEOUT_MAX);
				dtotimeval(timeout, &session->opt.timeout);
				break ;
			case VERBOSE_OPTION:
				session->opt.verbose = true;
				break ;
			case INTERVAL_OPTION:
				interval = parsed(ft_optarg, INTERVAL_MIN, INTERVAL_MAX);
				dtotimeval(interval, &session->opt.interval);
				break ;
			case TTL_OPTION:
				session->opt.ttl = parseul(ft_optarg, TTL_MIN, TTL_MAX);
				break ;
			case '?':
				if (!session->host.name)
					session->host.name = av[ft_optind - 1];
				break ;
			default:
				usage();
		}
	}
}

static void			setup_packet(s_session *session)
{
	//Fill IP header
	session->echo.iphdr.version = IPVERSION;
	session->echo.iphdr.ihl = IPHDR_IHL;
	session->echo.iphdr.tot_len = sizeof(s_echo);
	session->echo.iphdr.ttl = session->opt.ttl;
	session->echo.iphdr.protocol = IPPROTO_ICMP;
	session->echo.iphdr.check = 0; //filled by kernel
	session->echo.iphdr.saddr = INADDR_ANY;
	session->echo.iphdr.daddr = session->host.addr.sin_addr.s_addr;

	//Fill ICMP header
	session->echo.icmphdr.type = ICMP_ECHO;
	session->echo.icmphdr.code = 0;
}

/*
 * Fill the session struct with parsed values and set the ping packet accordingly
 */

void				setup_session(int ac, char **av, s_session *session)
{
	bzero(session, sizeof(s_session));
	
	//Setup default values
	session->opt.numeric = NI_NAMEREQD;
	session->opt.interval.tv_sec = 1;
	session->host.name = NULL;
	session->opt.ttl = IPDEFTTL;
	
	parse_options(ac, av, session);
	
	//Check there is a host
	if (!session->host.name)
		usage();

	resolve_host(&session->host);

	setup_socket(session);

	//Setup ping packet
	setup_packet(session);
}
