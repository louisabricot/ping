#include "../inc/ft_ping.h"

s_session g_session;

void	end_session(int sig)
{
	struct timeval	duration;
	struct timeval	end;

	(void)sig;
	gettimeofday(&end, NULL);
	timersub(&end, &g_session.start, &duration);
	
	printf("\n--- %s ft_ping statistics ---\n", g_session.host.name);
	printf("%lu packets transmitted, %lu received", g_session.info.nsent, g_session.info.nrecv);
	if (g_session.info.err)
		printf(", +%lu errors", g_session.info.err);
	if (g_session.info.nsent) {
		printf(", %d%% packet loss", (int)((g_session.info.nsent - g_session.info.nrecv) * 100 / g_session.info.nsent));
		printf(", time %ldms\n", duration.tv_sec * 1000  + duration.tv_usec / 100);
	}
	if (g_session.info.nrecv)
		printf("rtt min/avg/max = %.3f/%.3f/%.3f ms\n", g_session.info.tmin / 1000 , g_session.info.tsum / g_session.info.nrecv / 1000, g_session.info.tmax / 1000 );
	
	close(g_session.fd);
	exit(SUCCESS);
}

static void				status(int sig)
{
	struct s_info		info;
	double				avg;
	unsigned long		loss;

	(void)sig;
	info = g_session.info;
	avg = info.nrecv == 0 ? 0 : info.tsum / info.nrecv;
	loss = info.nsent == 0 ? 0 : (info.nsent - info.nrecv) / info.nsent;
	printf("%lu/%lu packets, %lu%% loss, min/avg/max = %.3f/%.3f/%.3f ms\n", info.nrecv, info.nsent, loss * 100 , info.tmin / 1000, avg / 1000, info.tmax / 1000);
}

static void				set_interval_timer(void)
{
	struct itimerval	interval;

	ft_bzero(&interval, sizeof(struct itimerval));
	interval.it_value = g_session.opt.interval;
	setitimer(ITIMER_REAL, &interval, NULL);
}

static void				check_deadline(void)
{
	struct timeval		now;
	struct timeval		next;

	gettimeofday(&now, NULL);
	timeradd(&now, &g_session.opt.interval, &next);
	if (timercmp(&now, &g_session.deadline, >=))
		end_session(SUCCESS);
}

static void				ping_target(int sig)
{
	(void)sig;
	
	if (!timerisset(&g_session.opt.deadline)) {

		//If count option is set, the program stops when we SENT <count> packets
		if (g_session.info.nsent == g_session.opt.count)
			end_session(SUCCESS);
	} else {
		check_deadline();
		
		//If both count and deadline options are set, the program stops when we RECEIVED <count> packets
		if (g_session.info.nrecv == g_session.opt.count)
			end_session(SUCCESS);
	}
	send_packet(&g_session);
	if (timerisset(&g_session.opt.interval))
		set_interval_timer();
}

int		main(int ac, char **av)
{
	setup_session(ac, av, &g_session);
	
	signal(SIGINT, &end_session);
	signal(SIGQUIT, &status);
	signal(SIGALRM, &ping_target);

	printf("FT_PING %s (%s) %lu(%lu) bytes of data.\n", g_session.host.name, g_session.host.ip, sizeof(g_session.echo.data), sizeof(g_session.echo));

	gettimeofday(&g_session.start, NULL);
	
	if (timerisset(&g_session.opt.deadline))
		timeradd(&g_session.start, &g_session.opt.deadline, &g_session.deadline);
	
	ping_target(SUCCESS);
	while (true)
	{
		receive_packet(&g_session);
	}
	return (SUCCESS);
}
