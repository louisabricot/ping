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
# include "ft_getopt.h"

/* 
 * PING OPTIONS
 */

# define OPTIONS		"c:nw:W:vi:vht:"
# define COUNT_OPTION		'c'
# define NUMERIC_OPTION		'n'
# define DEADLINE_OPTION	'w'
# define TIMEOUT_OPTION		'W'
# define TTL_OPTION		't'
# define VERBOSE_OPTION		'v'
# define INTERVAL_OPTION	'i'
# define HELP_OPTION		'h'

# define INTERVAL_MIN		0.0
# define INTERVAL_MAX		INT32_MAX

# define DEADLINE_MIN		0
# define DEADLINE_MAX		INT32_MAX

# define COUNT_MIN			1
# define COUNT_MAX			INT64_MAX

# define TIMEOUT_MIN		0
# define TIMEOUT_MAX		INT32_MAX

# define TTL_MIN			1
# define TTL_MAX			MAXTTL

/*
 * ERROR MANAGEMENT
 */

# define NB_OPTIONS			8
# define OPT_INDEX			0
# define DEF_INDEX			1
# define USAGE_MESSAGE		"Usage: ft_ping [OPTION...] HOST ...\nSend ICMP ECHO_REQUEST packets to network hosts.\n\nYou must be root to start pinging.\n\nOptions implemented:\n"
# define PING_OPTIONS	{ \
	{"-c <count>", "stop after sending <count> replies"}, \
	{"-h", "print help and exit"}, \
	{"-i <interval>", "wait <interval> seconds between sending each packet"}, \
	{"-n", "do not resolve host addresses"}, \
	{"-t <ttl>", "specify <ttl> as time to live"}, \
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
# define ECHO_DATA_LEN		56
# define IPHDR_IHL			5

typedef struct			s_opt
{
	unsigned long		count;
	int					numeric;
	struct timeval		interval;
	bool				verbose;
	struct timeval		timeout;
	unsigned int		ttl;
	struct timeval		deadline;

}						s_opt;

typedef struct			s_info
{
	unsigned long		nsent;
	unsigned long		nrecv;
	unsigned long		nloss;
	unsigned long		seq_n;
	unsigned long		err;
	double				tmin;
	double				tmax;
	double				tsum;
}						s_info;

typedef struct			s_host
{
	char				*name;
	char				ip[INET_ADDRSTRLEN];
	char				reverse[NI_MAXHOST];
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

// setup.c
void					setup_session(int ac, char **av, s_session *session);

// receive.c
void					receive_packet(s_session *session);

// getopt.c
int						ft_getopt(int ac, char **av, const char *optstring);

// send.c
void					send_packet(s_session *session);

// utils.c
void					error_exit(char *msg);
double					timevaltod(struct timeval *time);
void					usage(void);
unsigned long			parseul(char *arg, unsigned long min, unsigned long max);
double					parsed(char *arg, double min, double max);
void					dtotimeval(double time_in_sec, struct timeval *time);
void					*ft_bzero(void *b, size_t len);
void					*ft_memcpy(void *dst, const void *src, size_t n);
#endif
