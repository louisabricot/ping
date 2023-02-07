#include "../inc/ft_ping.h"
# define INVALID_OPTION "invalid option"
# define REQUIRES_ARG "option requires an argument"

char *ft_optarg = NULL;
int ft_optind = 1;
int ft_opterr = 1;
int ft_optopt = '?';

static void	print_error(char *progname, char *errmsg)
{
	dprintf(STDERR_FILENO, "%s: %s -- '%c'\n", progname, errmsg, ft_optopt);
}

static int getarg(int ac, char **av, char option, char **next_char)
{
	if (!*next_char || **next_char == '\0') {
		if (ft_optind == ac) {
			ft_optopt = option;
			print_error(av[0], REQUIRES_ARG);
			return -1;
		} else {
			ft_optarg = av[ft_optind];
			ft_optind++;
		}
	} else {
		ft_optarg = *next_char;
		*next_char = NULL;
	}
	return 0;
}

static int requires_arg(char *option)
{
	if (*(option + 1) == ':')
		return 1;
	return 0;
}

int		ft_getopt(int ac, char **av, const char *optstring)
{
	static char	*next_char = NULL;
	char		*curr;
	char		*p;

	ft_optarg = NULL;
	curr = next_char;
	if (!curr) {

		if (ft_optind == ac) {
			ft_optind = 1;
			return -1;
		}
		if (av[ft_optind] != NULL && av[ft_optind][0] == '-' && av[ft_optind][1] != '\0')
			curr = &(av[ft_optind][1]);
		ft_optind++;
		if (!curr)
			return 63;
			
	}
	next_char = curr + 1;
	if ((p = strchr(optstring, *curr)) != NULL) {
		ft_optopt = 0;
		if (requires_arg(p))
			if (getarg(ac, av, *curr, &next_char))
				return 63;
	} else {
		ft_optopt = *curr;
		print_error(av[0], INVALID_OPTION);
	}
	if (next_char && *next_char == '\0')
		next_char = NULL;
	return *curr;
}
