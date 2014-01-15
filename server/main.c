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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int conf_to_id(char *name);
char *str_tolower(char *str,int len);
int nid_to_lid(char nid);
char actions[10][20];
char mappings[10][20];

int main() {
	char conf[] = "./artemis.conf";
	char linebuffer[200];
	FILE *c;
	int fds[2],r;
	
	strcpy(actions[0],"Up");
	strcpy(actions[1],"Down");
	strcpy(actions[2],"Left");
	strcpy(actions[3],"Right");
	
	strcpy(actions[4],"a");
	strcpy(actions[5],"b");
	strcpy(actions[6],"x");
	strcpy(actions[7],"y");
	
	strcpy(actions[8],"e");
	strcpy(actions[9],"t");
	
	strcpy(mappings[0],"up");
	strcpy(mappings[1],"down");
	strcpy(mappings[2],"left");
	strcpy(mappings[3],"right");
	
	strcpy(mappings[4],"a");
	strcpy(mappings[5],"b");
	strcpy(mappings[6],"x");
	strcpy(mappings[7],"y");
	
	strcpy(mappings[8],"select");
	strcpy(mappings[9],"start");
	
	if(socketpair(AF_UNIX,SOCK_STREAM,0,fds) < 0) error(1,errno,"socketpair");
	if((c = fopen(conf,"r")) == NULL) error(1,errno,"fopen");
	int line;
	for(line = 1;fgets(linebuffer,200,c) != NULL;line++) {
		char keybuffer[100];
		char valuebuffer[100];
		int i,c;
		for(i = 0;isspace(linebuffer[i]);i++);
		if(i == strlen(linebuffer)) // Empty line
			continue;
		for(c = 0;linebuffer[i] != ':';i++,c++) keybuffer[c] = linebuffer[i];
		if(c == 0) { // Key missing
			fprintf(stderr,"Key missing on line %i \n",line);
			return 1;
		}
		i++;
		keybuffer[c] = '\0';
		for(c = 0;isalpha(linebuffer[i]);i++,c++) valuebuffer[c] = linebuffer[i];
		if(c == 0) { // Value missing
			fprintf(stderr,"Value missing or malformed on line %i \n",line);
			return 1;
		}
		valuebuffer[c] = '\0';
		i = conf_to_id(str_tolower(keybuffer,100));
		if(i < 0) {
			fprintf(stderr,"Unkown key on line %i \n",line);
			continue;
		}
		strcpy(actions[i],valuebuffer);
	}
	int i;
	
	r = fork();
	if(r < 0) error(1,errno,"fork");
	if(r == 0) {
		if(dup2(fds[1],1) < 0) error(1,errno,"dup2");
		execlp("./index.js","./index.js",NULL);
		close(1);
		return 0;
	}
	while(1) {
		int sc,lid,r;
		char raw;
		char update_buffer[2];
		char exec_buffer[100];
		while(read(fds[0],&raw,1) > 0 && raw != '#');
		for(sc = 0;sc < 2;) {
			if(read(fds[0],&raw,1) <= 0) error(1,errno,"read");
			update_buffer[sc++] = raw;
		}
		if((lid = nid_to_lid(update_buffer[0])) < 0) continue;
		r = fork();
		if(r < 0) error(1,errno,"fork");
		if(r > 0) continue;
		if(update_buffer[1] == '0') execlp("xdotool","xdotool","keyup",actions[lid],NULL); //printf("%s up\n",actions[lid]);
		if(update_buffer[1] == '1') execlp("xdotool","xdotool","keydown",actions[lid],NULL); //printf("%s down\n",actions[lid]);
		return 0;
	}
	return 0;
}

int conf_to_id(char *name) {
	int i;
	for(i = 0;strcmp(mappings[i],name) != 0 && i < 10;i++);
	if(i >= 10) return -1;
	return i;
}


char *str_tolower(char *str,int len) {
	int i;
	for(i = 0;i < strlen(str) && i < len;i++) str[i] = tolower(str[i]);
	return str;
}

int nid_to_lid(char nid) {
	switch(tolower(nid)) {
		case 'u':
			return 0;
		case 'd':
			return 1;
		case 'l':
			return 2;
		case 'r':
			return 3;
		case 'a':
			return 4;
		case 'b':
			return 5;
		case 'x':
			return 6;
		case 'y':
			return 7;
		case 'e':
			return 8;
		case 't':
			return 9;
		default:
			return -1;
	}
}
