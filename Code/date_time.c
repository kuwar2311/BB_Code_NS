#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

#include "common.h"


int time_sync_with_server(char *server_ip)
{
	int sockfd = 0, n = 0;
	char recvBuff[20];
	struct sockaddr_in serv_addr; 
	memset(recvBuff, '0',sizeof(recvBuff));
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\t\t Date/Time Sync : Could not create socket.\n");
		return 1;
	} 
	memset(&serv_addr, '0', sizeof(serv_addr)); 
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(5000); 

	if(inet_pton(AF_INET, server_ip, &serv_addr.sin_addr)<=0)
	{
		printf("\t\t Date/Time Sync: inet_pton error occured\n");
		return 1;
	} 

	if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)   
	{
		return 1;
	} 

	while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
	{
		recvBuff[n] = 0;
	}

	close(sockfd);

	char *cmd;
	cmd=(char*)calloc(1,128);
	/*bzero(cmd,strlen(cmd));
	sprintf(cmd, "size of received date time '%zu'", strlen(recvBuff));
	logger(cmd, "Date-Time Sync Module");*/
	if(strlen(recvBuff) < 13)
	{
		bzero(cmd,strlen(cmd));
		sprintf(cmd,"sudo date -s @%s",recvBuff);
		printf("\t\t Date/time Sync: %s\n",cmd);
		system(cmd);
	}
	free(cmd);       
	return 0;
}

int time_sync(char *server_ip){
	int i=0;
	while(1){
		i++;
		if(i>5){
			remove_all_containers();
			exit(0);
		}
		if(time_sync_with_server(server_ip)){
			printf("----------------------------------------------------------------------------------------");
			printf("\nCurrent Date-Time: \n");
			system("sudo date");
			logger("Please verify the server's Port 5000 is opened for the sync of server time","Date_time_sync");
			logger("Please check your network configuration or may be server is down","date_time_sync");
			printf("\nFailed to connect the Server (%s)\n",server_ip);
			printf("Please verify the server's Port 5000 is opened for the sync of server time\n");
			printf("Please check your network configuration or may be server is down\n");
			sleep(60);
		}
		else
			break;
	}
	return 0;
}
