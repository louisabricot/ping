#include "../inc/ft_ping.h"

s_session g_session;

void	end_session(int sig)
{
	(void)sig;
	unsigned int	duration;
	struct timeval	end;

	duration = (diff.tv_sec * 1000) + (diff.tv_usec / 1000);
	gettimeofday(&end, NULL);
	dprintf(STDOUT_FILENO, "--- %s ft_ping statistics ---\n", g_session.host.name);
	dprintf(STDOUT_FILENO, "%lu packets transmitted, %lu received, %lu%% packet loss, time %ims\n", g_session.info.nsent, g_session.info.nrecv, g_session.info.loss, duration);
	dprintf(STDOUT_FILENO, "rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%ld.%03ld ms\n",
	g_session.info.tmin / 1000, (g_session.info.tsum / g_session.info.nrev) / 1000, g_session.info.tmax / 1000, 0, 0);
	exit(0);
}

void	status(int sig)
{

	(void)sig;
	dprintf(STDOUT_FILENO, "%lu/%lu packets, %lu%% loss, min/avg/ewma/max = %.3f/%.3f/%.3f/%.3f ms\n", g_session.info.nrecv, g_session.info.nsent, g_session.info.nloss, g_session.info.tmin, g_session.info.tsum / g_session.info.recv, 0, g_session.info.tmax);
}

void	ping_target(int sig)
{
	(void)sig;

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

	printf("options:\ncount %lu\nnumeric %i\ninterval %f\nverbose %i\ntimeout %f\ndeadline %lu\n", session->opt.count, session->opt.numeric, session->opt.interval, session->opt.verbose, session->opt.timeout, session->opt.deadline);
	
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

	//Ping target
	ping_target(SUCCESS);
	
	while (true)
		;
	return (SUCCESS);
}
