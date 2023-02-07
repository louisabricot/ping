#include "../inc/ft_ping.h"

void		init_socket(s_session *session)
{
	int				on;
	struct timeval	timeout;

	//check permission
	if (getuid() != ROOT)
	{
		dprintf(STDERR_FILENO, "you must be root to create raw socket\n");
		exit(ERROR);
	}

	session->fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (session->fd == ERROR)
		error_exit("socket");
	
	//Stop kernel from adding IP header
	on = 1;
	if (setsockopt(session->fd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) != SUCCESS)
		error_exit("setsockopt");

	//Add a time out to recvmsg()
	dtotimeval(session->opt.timeout, &timeout);
	if (setsockopt(session->fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) != SUCCESS)
		error_exit("setsockopt");
}

static void		set_default_values(s_session *session)
{
	bzero(session, sizeof(s_session));

	session->opt.numeric = NI_NAMEREQD;
	session->opt.interval = 1;
	session->opt.verbose = false;
	session->host.name = NULL;
	//session->opt.timeout = 0;
	//session->opt.deadline = 0;
}

double		parsed(char *arg, uint64_t min, uint64_t max)
{
	double			n;
	char			*endptr;

	n = strtod(arg, &endptr);
	if (*endptr != '\0')	
	{
		dprintf(STDERR_FILENO, "ft_ping: invalid argument '%s'\n", arg);
		exit(ERROR);
	}
	
	//Check range
	if (n < min || n > max)
	{
		dprintf(STDERR_FILENO, "ft_ping: invalid argument: '%f': out of range: %ld <= value <= %ld\n", n, min, max);
		exit(ERROR);
	}
	return (n);
}

unsigned long	parseul(char *arg, uint64_t min, uint64_t max)
{
	unsigned long	n;
	char			*endptr;

	n = strtoul(arg, &endptr, DEC_BASE);
	if (*endptr != '\0')
	{
		dprintf(STDERR_FILENO, "ft_ping: invalid argument '%s'\n", arg);
		exit(ERROR);
	}
	
	//Check range
	if (n < min || n > max)
	{
		dprintf(STDERR_FILENO, "ft_ping: invalid argument: '%ld': out of range: %ld <= value <= %ld\n", n, min, max);
		exit(ERROR);
	}
	return (n);
}

void				resolve_host(s_host *host)
{
	struct addrinfo hints;
	struct addrinfo *res;

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

static void			parse_options(int ac, char **av, s_session *session)
{
	int				opt;

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
				session->opt.deadline = parseul(ft_optarg, DEADLINE_MIN, DEADLINE_MAX);
				break ;
			case TIMEOUT_OPTION:
				session->opt.timeout = parsed(ft_optarg, TIMEOUT_MIN, TIMEOUT_MAX);
				break ;
			case VERBOSE_OPTION:
				session->opt.verbose = true;
				break ;
			case INTERVAL_OPTION:
				session->opt.interval = parsed(ft_optarg, INTERVAL_MIN, INTERVAL_MAX);
				//if (session->opt.interval < 0.2 && getuid() != ROOT)
					//error_exit();
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

void			init_session(int ac, char **av, s_session *session)
{
	set_default_values(session);
	parse_options(ac, av, session);
	resolve_host(&session->host);
	//setup_echo(session);
}
