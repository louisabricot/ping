#include "../inc/ft_ping.h"

void	error_exit(char *msg)
{
	dprintf(STDERR_FILENO, "ft_ping: %s: %s\n", msg, strerror(errno));
	exit(ERROR);
}

/*
 * Parse arg string into double and check number is within min and max range
 */

double				parsed(char *arg, double min, double max)
{
	double			n;
	char			*endptr;

	n = strtod(arg, &endptr);

	//Check arg is a valid option
	if (*endptr != '\0')	
	{
		dprintf(STDERR_FILENO, "ft_ping: invalid argument '%s'\n", arg);
		exit(ERROR);
	}
	
	//Check range
	if (n < min || n > max)
	{
		dprintf(STDERR_FILENO, "ft_ping: invalid argument: '%f': out of range: %f <= value <= %f\n", n, min, max);
		exit(ERROR);
	}
	return (n);
}

/*
 * Parse arg string into unsigned long and check number is within min max range
 */

unsigned long			parseul(char *arg, unsigned long min, unsigned long max)
{
	unsigned long		n;
	char				*endptr;

	n = strtoul(arg, &endptr, DEC_BASE);

	//Check arg is a valid option
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

/*
 * Set the timeval struct time with time_in_sec
 * The integral part of time_in_sec sets tv_sec and the fractional part is converted into microseconds
 * time_in_sec is a double integer representing time in seconds
 */

void				dtotimeval(double time_in_sec, struct timeval *time)
{
	double			fractional;
	double			integral;

	fractional = modf(time_in_sec, &integral);
	time->tv_sec = integral;
	time->tv_usec = fractional * TO_MICROSECONDS;
}

/*
 * Print usage and exit
 */

void					usage(void)
{
	char				*opt[NB_OPTIONS][2] = PING_OPTIONS;

	dprintf(STDOUT_FILENO, USAGE_MESSAGE);
	for (int i = 0; i < NB_OPTIONS; i++)
		dprintf(STDOUT_FILENO, "%-20s%s\n", opt[i][OPT_INDEX], opt[i][DEF_INDEX]);
	exit(SUCCESS);
}

