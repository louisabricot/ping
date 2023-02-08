#include "../inc/ft_ping.h"

void	dtotimeval(double n, struct timeval *time)
{
	double fractional;
	double integral;

	fractional = modf(n, &integral);
	time->tv_sec = integral;
	time->tv_usec = fractional * TO_MICROSECONDS;
}

//returns in milliseconds
double	timevaltod(struct timeval *time)
{
	return time->tv_sec * 1000 + time->tv_usec / 1000; 
}
