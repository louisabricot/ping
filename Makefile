.DEFAULT_TARGET: all

bin := ft_ping
inc_dir := inc

CC := gcc
CFLAGS := -Wall -Wextra -Werror -I ${inc_dir}
VPATH = src
src := ping.c \
	init.c \
	utils.c \
	error.c \
	ft_getopt.c \

objs_dir = obj

objs := $(addprefix ${objs_dir}/, ${src:.c=.o})

.PHONY: all
all: ${objs_dir} ${objs}
	${CC} ${CFLAGS} ${objs} -o ${bin}
	@printf "\033[0;32mExecutable created!\033[m\n"

.PHONY: clean
clean:
	@/bin/rm -rf ${objs_dir}

.PHONY: fclean
fclean: clean
	@/bin/rm -rf ${bin}

.PHONY: re
re: fclean all

${objs_dir}:
	@mkdir -p ${objs_dir}

${objs_dir}/%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@
	@printf "\033[0;36mObject files created\\033[m\n"
