#include "../inc/ft_ping.h"

static void			setup_socket(s_session *session)
{
	int				on;

	//Check uid
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

void					resolve_host(s_host *host)
{
	struct addrinfo		hints;
	struct addrinfo		*res;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;

	//Translate host name into socket address
	if (getaddrinfo(host->name, NULL, &hints, &res) != SUCCESS)
		error_exit("getaddrinfo: invalid host");
	host->addr = *(struct sockaddr_in *)res->ai_addr;

	//Translate socket address into IP
	inet_ntop(AF_INET, &(host->addr.sin_addr), host->ip, INET_ADDRSTRLEN);
	freeaddrinfo(res);
}

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

static void			set_iphdr(struct iphdr *iphdr, s_session *session)
{
	iphdr->version = IPVERSION;
	iphdr->ihl = IPHDR_IHL;
	iphdr->tot_len = sizeof(s_echo);
	iphdr->ttl = session->opt.ttl;
	iphdr->protocol = IPPROTO_ICMP;
	iphdr->check = 0; //filled by kernel
	iphdr->saddr = INADDR_ANY;
	iphdr->daddr = session->host.addr.sin_addr.s_addr;
}

static void			set_session(s_session *session)
{
	bzero(session, sizeof(s_session));

	//Setup default values
	session->opt.count = INT64_MAX;
	session->opt.numeric = NI_NUMERICHOST;
	session->opt.interval.tv_sec = 1;
	session->host.name = NULL;
	session->opt.ttl = IPDEFTTL;

}

void				setup_session(int ac, char **av, s_session *session)
{
	set_session(session);
	parse_options(ac, av, session);

	//If interval is set at 0.0, signal alarm is never triggered, bypass by setting an interval every 1 microsecond
	if (session->opt.interval.tv_sec == 0 && session->opt.interval.tv_usec == 0)
		session->opt.interval.tv_usec = 1;
	
	//Check there is a host
	if (!session->host.name)
		usage();

	resolve_host(&session->host);

	setup_socket(session);
	set_iphdr(&session->echo.iphdr, session);

}
