#include "../inc/ft_ping.h"

void	dtotimeval(double n, struct timeval *time)
{
	double fractional;
	double integral;

	fractional = modf(n, &integral);
	time->tv_sec = (time_t)integral;
	time->tv_usec = (suseconds_t)(fractional * TO_MICROSECONDS);
}
