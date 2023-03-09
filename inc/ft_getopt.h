#ifndef FT_GETOPT_H
# define FT_GETOPT_H

# include <stddef.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>

# define INVALID_OPTION "invalid option"
# define REQUIRES_ARG "option requires an argument"

extern int	ft_optind;
extern char	*ft_optarg;
extern int	ft_opterr;
extern int	ft_optopt;

#endif
