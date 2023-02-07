#include "../inc/ft_ping.h"

void	error_exit(char *msg)
{
	dprintf(STDERR_FILENO, "ft_ping: %s: %s\n", msg, strerror(errno));
	//if (g_session.fd)
	//	close(fd);
	exit(ERROR);
}
