#include "../inc/ft_ping.h"

void		strtotimeval(char *arg, struct timeval *time)
{
	double	n;

	n = parsed(arg, INTERVAL_MIN, INTERVAL_MAX);
	dtotimeval(n, time);
}
