#ifndef FT_PING_H
# define FT_PING_H

# include <signal.h>
# include <sys/time.h>
# include <stdio.h>
# include <stdint.h>
# include <stdbool.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <math.h>
# include <strings.h>
# include <string.h>
# include <netdb.h>
# include <errno.h>
# include <unistd.h>
# include <arpa/inet.h>

/* 
 * PING OPTIONS
 */

# define OPTIONS			"c:nw:W:vi:vh"
# define COUNT_OPTION		'c'
# define NUMERIC_OPTION		'n'
# define DEADLINE_OPTION	'w'
# define TIMEOUT_OPTION		'W'
# define VERBOSE_OPTION		'v'
# define INTERVAL_OPTION	'i'
# define INTERVAL_THRESHOLD	0.2
# define HELP_OPTION		'h'

# define INTERVAL_MIN		0
# define INTERVAL_MAX		INT32_MAX

# define DEADLINE_MIN		0
# define DEADLINE_MAX		INT32_MAX

# define COUNT_MIN			1
# define COUNT_MAX			INT64_MAX

# define TIMEOUT_MIN		0
# define TIMEOUT_MAX		INT32_MAX

/*
 * ERROR MANAGEMENT
 */

# define NB_OPTIONS			8
# define OPT_INDEX			0
# define DEF_INDEX			1
# define USAGE_MESSAGE		"\nUsage\nft_ping [options] <destination>\n\nOptions:\n"
# define PING_OPTIONS	{ \
	{"<destination>", "dns name of ip address"}, \
	{"-c <count>", "stop after <count> replies"}, \
	{"-h", "print help and exit"}, \
	{"-i <interval>", "seconds between sending each packet"}, \
	{"-n", "no dns name resolution"}, \
	{"-v", "verbose output"}, \
	{"-w <deadline>", "reply wait <deadline> in second"}, \
	{"-W <timeout>", "time to wait for response"} \
}

/*
 * MISCELLANEOUS
 */

# define SUCCESS			0
# define ERROR				-1
# define TO_MICROSECONDS	1e+6 
# define DEC_BASE			10
# define ROOT				0
# define HOST_LEN			1024 //TODO: change
# define ECHO_DATA_LEN		56
# define DEFAULT_LINUX_TTL	64
# define IPHDR_IHL			5
/*
 * GLOBAL VARIABLES FOR FT_GETOPT
 */

extern int	ft_optind;
extern char	*ft_optarg;
extern int	ft_opterr;
extern int	ft_optopt;

typedef struct			s_opt
{
	unsigned long		count;
	int					numeric;
	struct timeval		interval; //in seconds
	bool				verbose;
	struct timeval		timeout;
	struct timeval		deadline;

}						s_opt;

typedef struct			s_info
{
	unsigned long		nsent;
	unsigned long		nrecv;
	unsigned long		nloss;
	unsigned long		seq_n;
	double				tmin;
	double				tmax;
	double				tsum;
}						s_info;

typedef struct			s_host
{
	char				*name;
	char				ip[INET_ADDRSTRLEN];
	char				reverse[HOST_LEN];
	struct sockaddr_in	addr;
}						s_host;

typedef struct			s_echo
{
	struct	iphdr		iphdr;
	struct	icmphdr		icmphdr;
	unsigned char		data[ECHO_DATA_LEN];
}						s_echo;

typedef struct			s_session
{
	int					fd;
	struct timeval		start;
	struct timeval		deadline;
	s_echo				echo;
	s_host				host;
	s_info				info;
	s_opt				opt;
}						s_session;

void	dtotimeval(double n, struct timeval *time);
void	error_exit(char *msg);
void	init_session(int ac, char **av, s_session *session);
void	init_socket(s_session *session);
int	ft_getopt(int ac, char **av, const char *optstring);
void	usage(void);
int			receive_packet(s_session *session);
void		send_packet(s_session *session);
double	timevaltod(struct timeval *time);
void	setup_echo(s_session *session);
#endif
