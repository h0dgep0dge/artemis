/******************************************************\
* main.c: The main server loop for the artemis project *
* By Robbie Mckennie                                   *
\******************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>

void handle_update(char *update_buffer);

int main() {
	int fds[2],r;
	
	if(socketpair(AF_UNIX,SOCK_STREAM,0,fds) < 0) error(1,errno,"socketpair");
	r = fork();
	if(r < 0) error(1,errno,"fork");
	if(r == 0) {
		if(dup2(fds[1],1) < 0) error(1,errno,"dup2");
		execlp("./index.js","./index.js",NULL);
		close(1);
		return 0;
	}
	while(1) {
		int sc;
		char raw;
		char update_buffer[2];
		while(read(fds[0],&raw,1) > 0 && raw != '#');
		for(sc = 0;sc < 2;) {
			if(read(fds[0],&raw,1) <= 0) error(1,errno,"read");
			update_buffer[sc++] = raw;
		}
		// write(1,update_buffer,2);
		handle_update(update_buffer);
	}
	return 0;
}

void handle_update(char *update_buffer) {
	switch(update_buffer[0]) {
		case 'u':
			switch(update_buffer[1]) {
				case '1':
					printf("Up, press\n");
					break;
				case '0':
					printf("Up, release\n");
					break;
				default:
					printf("Unknown state\n");
			}
			break;
		case 'd':
			switch(update_buffer[1]) {
				case '1':
					printf("Down, press\n");
					break;
				case '0':
					printf("Down, release\n");
					break;
				default:
					printf("Unknown state\n");
			}
			break;
		case 'l':
			switch(update_buffer[1]) {
				case '1':
					printf("Left, press\n");
					break;
				case '0':
					printf("Left, release\n");
					break;
				default:
					printf("Unknown state\n");
			}
			break;
		case 'r':
			switch(update_buffer[1]) {
				case '1':
					printf("Right, press\n");
					break;
				case '0':
					printf("Right, release\n");
					break;
				default:
					printf("Unknown state\n");
			}
			break;
		case 'a':
			switch(update_buffer[1]) {
				case '1':
					printf("A, press\n");
					break;
				case '0':
					printf("A, release\n");
					break;
				default:
					printf("Unknown state\n");
			}
			break;
		case 'b':
			switch(update_buffer[1]) {
				case '1':
					printf("B, press\n");
					break;
				case '0':
					printf("B, release\n");
					break;
				default:
					printf("Unknown state\n");
			}
			break;
		case 'x':
			switch(update_buffer[1]) {
				case '1':
					printf("X, press\n");
					break;
				case '0':
					printf("X, release\n");
					break;
				default:
					printf("Unknown state\n");
			}
			break;
		case 'y':
			switch(update_buffer[1]) {
				case '1':
					printf("Y, press\n");
					break;
				case '0':
					printf("Y, release\n");
					break;
				default:
					printf("Unknown state\n");
			}
			break;
		case 't':
			switch(update_buffer[1]) {
				case '1':
					printf("Start, press\n");
					break;
				case '0':
					printf("Start, release\n");
					break;
				default:
					printf("Unknown state\n");
			}
			break;
		case 'e':
			switch(update_buffer[1]) {
				case '1':
					printf("Select, press\n");
					break;
				case '0':
					printf("Select, release\n");
					break;
				default:
					printf("Unknown state\n");
			}
			break;
		default:
			printf("Unknown key\n");
	}
}
