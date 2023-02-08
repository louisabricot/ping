#include "../inc/ft_ping.h"

s_session g_session;

void	end_session(int sig)
{
	struct timeval	end;
	struct timeval	duration;
	double			avg;
	double			time;
	double			loss;

	(void)sig;
	
	gettimeofday(&end, NULL);
	timersub(&end, &g_session.start, &duration);
	time = duration.tv_sec * TO_MICROSECONDS + duration.tv_usec;

	avg = g_session.info.nrecv == 0 ? 0 : g_session.info.tsum / g_session.info.nrecv;
	loss = (g_session.info.nsent - g_session.info.nrecv) / g_session.info.nsent;
	printf("\n--- %s ft_ping statistics ---\n", g_session.host.name);
	printf("%lu packets transmitted, %lu received, %.0f%% packet loss, time %.2f ms\n", g_session.info.nsent, g_session.info.nrecv, loss * 100, time / 1000 );
	if (g_session.info.nrecv)
		printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%d.%03d ms\n", g_session.info.tmin / 1000 , avg / 1000, g_session.info.tmax /1000, 0, 0);
	exit(SUCCESS);
}

void	status(int sig)
{
	(void)sig;
	double	avg;
	unsigned long loss;

	avg = g_session.info.nrecv == 0 ? 0 : g_session.info.tsum / g_session.info.nrecv;
	loss = (g_session.info.nsent - g_session.info.nrecv) / g_session.info.nsent;
	dprintf(STDOUT_FILENO, "%lu/%lu packets, %lu%% loss, min/avg/ewma/max = %.3f/%.3f/%d/%.3f ms\n", g_session.info.nrecv, g_session.info.nsent, loss * 100 , g_session.info.tmin / 1000, avg / 1000, 0, g_session.info.tmax / 1000);
}

void	ping_target(int sig)
{
	struct timeval		next;
	struct itimerval	interval;
	
	(void)sig;
	
	send_packet(&g_session);
	receive_packet(&g_session);

	//Check deadline and counter
	gettimeofday(&next, NULL);
	timeradd(&next, &g_session.opt.interval, &next);
	if (timerisset(&g_session.opt.deadline))
	{
		if (timercmp(&next, &g_session.deadline, >) || g_session.info.nrecv == g_session.opt.count)
			end_session(SUCCESS);
	}
	else if (g_session.info.nsent == g_session.opt.count)
		end_session(SUCCESS);
	
	//Set interval for next ping
	bzero(&interval, sizeof(struct itimerval));
	interval.it_value = g_session.opt.interval;
	setitimer(ITIMER_REAL, &interval, NULL);
}

void	usage(void)
{
	char	*opt[NB_OPTIONS][2] = PING_OPTIONS;

	dprintf(STDOUT_FILENO, USAGE_MESSAGE);
	for (int i = 0; i < NB_OPTIONS; i++)
		dprintf(STDOUT_FILENO, "%-20s%s\n", opt[i][OPT_INDEX], opt[i][DEF_INDEX]);
	exit(SUCCESS);
}

int		main(int ac, char **av)
{
	s_session	*session;

	session = &g_session;
	
	if (ac == 1)
		usage();

	//Init ping session
	init_session(ac, av, session);
	
	//Setup socket
	init_socket(session);
	
	//Setup signals
	signal(SIGINT, end_session);
	signal(SIGQUIT, status);
	signal(SIGALRM, ping_target);

	//Print banner
	dprintf(STDOUT_FILENO, "FT_PING: %s (%s) %lu(%lu) bytes of data.\n", session->host.name, session->host.ip, sizeof(session->echo.data), sizeof(session->echo));

	//Set start time
	gettimeofday(&session->start, NULL);
	
	//Set deadline and intervals
	if (timerisset(&session->opt.deadline))
		timeradd(&session->start, &session->opt.deadline, &session->deadline);
	
	//Ping target
	ping_target(SUCCESS);
	
	while (true)
		;
	return (SUCCESS);
}
