NAME=mathwait
LOWERNAME=mathwait
OBJS = ${LOWERNAME}.c
CFLAGS= -g -O2 -Wformat-security -Wduplicated-cond -Wfloat-equal -Wshadow -Wjump-misses-init -Wlogical-not-parentheses -Wnull-dereference -Wall -Wextra -Werror -pedantic-errors -I.
CC = gcc

${NAME}:${LOWERNAME}.c
	${CC} -o ${NAME} ${CFLAGS} ${OBJS}
