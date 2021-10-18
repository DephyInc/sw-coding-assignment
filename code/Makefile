#Simple Makefile for coding assignment

APP = csv_aligner
C_SRCS = src/csv_aligner.cpp
C_FLAGS = -Wall -Wextra -Werror
INC = -Iinc

all: ${APP}

clean:
	rm -f ${APP}

${APP}: ${C_SRCS}
	g++ ${C_FLAGS} ${INC} -o ${APP} ${C_SRCS}