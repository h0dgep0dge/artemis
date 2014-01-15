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
#include <ctype.h>

int conf_to_id(char *name);
char *str_tolower(char *str,int len);
int nid_to_lid(char nid);
char actions[9][10][20];
char mappings[10][20];
char pin[5];

int main(int argc,char **argv) {
	char conf[100];
	if(argc == 1) strcpy(conf,"./artemis.conf");
	else strcpy(conf,argv[1]);
	char linebuffer[300];
	FILE *c;
	int i,fds[2],r;
	struct stat buffer;
	
	strcpy(pin,"1234");
	
	for(i = 0;i < 8;i++) {
		strcpy(actions[i][0],"Up");
		strcpy(actions[i][1],"Down");
		strcpy(actions[i][2],"Left");
		strcpy(actions[i][3],"Right");
	
		strcpy(actions[i][4],"a");
		strcpy(actions[i][5],"b");
		strcpy(actions[i][6],"x");
		strcpy(actions[i][7],"y");
	
		strcpy(actions[i][8],"q");
		strcpy(actions[i][9],"Return");
	}
	
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
	
	if(stat(conf,&buffer) == 0) {
		if((c = fopen(conf,"r")) == NULL) error(1,errno,"fopen");
		int line;
		for(line = 1;fgets(linebuffer,200,c) != NULL;line++) {
			char playerbuffer[100];
			char keybuffer[100];
			char valuebuffer[100];
			int i,c;
			for(i = 0;isspace(linebuffer[i]);i++);
			if(i == strlen(linebuffer)) // Empty line
				continue;
			if(linebuffer[i] == '#') continue; // Line is a comment
			for(c = 0;linebuffer[i] != ':';i++,c++) playerbuffer[c] = linebuffer[i];
			if(c == 0) { // Player missing
				fprintf(stderr,"Player missing on line %i \n",line);
				return 1;
			}
			i++;
			playerbuffer[c] = '\0';
			for(c = 0;linebuffer[i] != ':';i++,c++) keybuffer[c] = linebuffer[i];
			if(c == 0) { // Key missing
				fprintf(stderr,"Key missing on line %i \n",line);
				return 1;
			}
			i++;
			keybuffer[c] = '\0';
			for(c = 0;isalnum(linebuffer[i]);i++,c++) valuebuffer[c] = linebuffer[i];
			if(c == 0) { // Value missing
				fprintf(stderr,"Value missing or malformed on line %i \n",line);
				return 1;
			}
			valuebuffer[c] = '\0';
			if(strlen(playerbuffer) != 1) {
				fprintf(stderr,"Malformed player field on line %i \n",line);
				return 1;
			}
			else if(!isdigit(playerbuffer[0])) {
				fprintf(stderr,"Player field must be a number \n");
				return 1;
			}
			if(strcmp(str_tolower(keybuffer,100),"pin") == 0) {
				if(strlen(valuebuffer) != 4) {
					fprintf(stderr,"Pin needs to be 4 characters long \n");
					return 1;
				}
				strcpy(pin,valuebuffer);
				continue;
			}
			if(playerbuffer[0] == '0') {
				fprintf(stderr,"Player field must be greater than 0 \n");
				return 1;
			}
			i = conf_to_id(str_tolower(keybuffer,100));
			if(i < 0) {
				fprintf(stderr,"Unkown key on line %i \n",line);
				continue;
			}
			strcpy(actions[playerbuffer[0]-'1'][i],valuebuffer);
			//printf("%s %s\n",actions[playerbuffer[0]-'1'][i],valuebuffer);
		}
		fclose(c);
	} else {
		int i;
		if((c = fopen(conf,"w")) == NULL) error(1,errno,"fopen");
		fprintf(c,"# Config file format\n"
		          "# player:button:key\n\n"
		          "0:pin:1234\n\n");
		for(i = 0;i < 10;i++) fprintf(c,"1:%s:%s\n",mappings[i],actions[0][i]);
		fprintf(c,"\n");
		for(i = 0;i < 10;i++) fprintf(c,"2:%s:%s\n",mappings[i],actions[1][i]);
		fclose(c);
	}
	//int a,b;
	//for(a = 0;a < 9;a++) for(b = 0;b < 10;b++) printf("%i %s %s\n",a,mappings[b],actions[a][b]);
	//return 0;
	r = fork();
	if(r < 0) error(1,errno,"fork");
	if(r == 0) {
		if(dup2(fds[1],1) < 0) error(1,errno,"dup2");
		execlp("node","node","socket.js",pin,NULL);
		close(1);
		return 0;
	}
	while(1) {
		int sc,lid,r;
		char raw;
		char update_buffer[3];
		while(read(fds[0],&raw,1) > 0 && raw != '#');
		for(sc = 0;sc < 3;) {
			if(read(fds[0],&raw,1) <= 0) error(1,errno,"read");
			update_buffer[sc++] = raw;
		}
		if((lid = nid_to_lid(update_buffer[0])) < 0) continue;
		if(!isdigit(update_buffer[2])) continue;
		if(update_buffer[2] == '0') continue;
		r = fork();
		if(r < 0) error(1,errno,"fork");
		if(r > 0) continue;
		if(update_buffer[1] == '0') execlp("xdotool","xdotool","keyup",actions[update_buffer[2]-'1'][lid],NULL); //printf("%s up\n",actions[lid]);
		if(update_buffer[1] == '1') execlp("xdotool","xdotool","keydown",actions[update_buffer[2]-'1'][lid],NULL); //printf("%s down\n",actions[lid]);
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
