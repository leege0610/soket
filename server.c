#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define CLNT_MAX 10
#define BUFFERSIZE 200

int g_clnt_socks[CLNT_MAX];
int g_clnt_count = 0;

void sendAllClnt(char * msg, int my_sock)
{
	for(int i = 0; i < g_clnt_count; i++)
	{
		if(g_clnt_socks[i] != my_sock)
		{
			write(g_clnt_socks[i], msg, strlen(msg)+1);
		}
	}
}

void * clnt_connection(void * arg)
{
	int clnt_sock = (int)arg;
	int str_len = 0;
	
	char msg[BUFFERSIZE];
	int i;
	
	while(1)
	{
		str_len = read(clnt_sock, msg, sizeof(msg));
		if(str_len == -1)
		{
			printf("clnt[%d] close\n", clnt_sock);	
			break;
		}
		sendAllClnt(msg, clnt_sock);
		printf("%s\n", msg);
	}
	
	close(clnt_sock);
	pthread_exit(0);
	return NULL;
}

int main(int argc, char ** argv)
{
	int serv_sock;
	int clnt_sock;
	
	pthread_t t_thread;
	
	struct sockaddr_in clnt_addr;
	int clnt_addr_size;
	
	struct sockaddr_in serv_addr;
	serv_sock = socket(PF_INET,SOCK_STREAM,0);
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(7989);
	
	if(bind(serv_sock,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
	{
		printf("bind error\n");	
	}
	
	if(listen(serv_sock, 5) == -1)
	{
		printf("listen error\n");
	}
	
	char buff[200];
	int recv_len = 0;
	
	while(1)
	{
		clnt_addr_size = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
		
		g_clnt_socks[g_clnt_count++] = clnt_sock;
		pthread_create(&t_thread, NULL, clnt_connection, (void *)clnt_sock);


	}
}
