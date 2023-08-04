#include "common.h"
void configure_network(struct Node *node){
	char *str_docker;
	char *str_vbox;
	char *found;
	char *str_tech;
	str_tech=(char*)calloc(1,128);

	sprintf(str_tech,"%s",node->virtual_tech);
	found=strchr(str_tech,'|');
	if(found){
		size_t len = found - str_tech;
		str_vbox=malloc(len+1);
		memcpy(str_vbox,str_tech,len);
		str_vbox[len]='\0';

		len = strlen(str_tech) -strlen(str_vbox);
		str_docker=malloc(len);
		memcpy(str_docker,found+1,len-1);
		str_docker[len-1]='\0';
		if(!strcmp(str_vbox,"docker") || !strcmp(str_docker,"docker")){
			docker_network(node);
		}
		else{
			printf("Please check initials.xml for correct virtual technology.\n");
			printf("Any one option from following:\n\t1) docker\n\t2) virtualbox\n\t3) docker|virtualbox\n");
			free(str_docker);
			free(str_vbox);
			free(str_tech);
			exit(0);
		}
		free(str_docker);
		free(str_vbox);
	}
	else{
		if(!strcmp(str_tech,"docker")){
			docker_network(node);
		}
		else if(!strcmp(str_tech,"virtualbox")){
			virtual_network(node);	
		}
		else{
			printf("Please check initials.xml for correct virtual technology.\n");  
			printf("Any one option from following:\n\t1) docker\n\t2) virtualbox\n\t3) docker|virtualbox\n");
			free(str_tech);
			exit(0);
		}
	}
	free(str_tech);
}

void docker_network(struct Node *node){
	char *command;
	command = (char*)calloc(1,512);
	if(!strcmp(node->network_type, "DHCP")){

		//sprintf(command,"sudo bash ../network_script/docker/dhcp.sh %s",node->interface);
		//system(command);
		dhcp_network(node->interface);
	}
	else if(!strcmp(node->network_type, "Public")){
		FILE *fp = fopen("/etc/docker/daemon.json","r");
		if(fp!=NULL){
			fclose(fp);
			system("sudo brctl addbr dhcpbr");
			system("sudo ip link set dhcpbr up");
			bzero(command,strlen(command));
			sprintf(command,"sudo brctl addif dhcpbr %s",node->interface);
			system(command);     
		}
		system("sudo service docker start");
		remove_docker_container();
		remove_docker_network();
		system("sudo service docker stop");
		remove_network();
		//system("sudo rm -rf /etc/docker/daemon.json");
		system("sudo service docker start");
		sleep(5);
		public_network(node->subnet, node->node_ip, node->netmask, node->gateway, node->interface);
	}
	else if(!strcmp(node->network_type, "Private")){
		FILE *fp = fopen("/etc/docker/daemon.json","r");
		if(fp!=NULL){
			fclose(fp);
			system("sudo brctl addbr dhcpbr");
			system("sudo ip link set dhcpbr up");
			bzero(command,strlen(command));
			sprintf(command,"sudo brctl addif dhcpbr %s",node->interface);
			system(command);
		}
		system("sudo service docker start");
		remove_docker_container();
		remove_docker_network();
		system("sudo service docker stop");
		remove_network();
		//system("sudo rm -rf /etc/docker/daemon.json");
		system("sudo service docker start");
		sleep(5);
		public_network(node->subnet, node->node_ip, node->netmask, node->gateway, node->interface);
	}
	else{
		printf("Please check initial.xml file for correct network type:\n");
		printf("Network type should be \"Public\" or \"Private\" or \"DHCP\"\n");
		exit(0);
	}
}

void virtual_network(struct Node *node){
	char *command;
	command = (char*)calloc(1,512);
	if(!strcmp(node->network_type, "DHCP")){
		sprintf(command,"sudo bash ../network_script/virtual/dhcp.sh %s",node->interface);
		system(command);
	}
	else if(!strcmp(node->network_type, "Public")){
		sprintf(command,"sudo bash ../network_script/virtual/public.sh %s %s %s %s %s",node->subnet,node->node_ip,node->netmask,node->gateway,node->interface);
		system(command);
	}
	else if(!strcmp(node->network_type, "Private")){
		sprintf(command,"sudo bash ../network_script/virtual/private.sh %s %s %s %s %s",node->subnet,node->node_ip,node->netmask,node->gateway,node->interface);
		system(command);
	}
	else{
		printf("Please check initial.xml file for correct network type:\n");
		printf("Network type should be \"Public\" or \"Private\" or \"DHCP\"\n");
		exit(0);
	}
}

int dhcp_network(char *interface){
	printf("----------------- DHCP Network Setup ------------\n");
	char *str;
	str = (char *)calloc(1,128);
	sprintf(str,"{\"bridge\":\"dhcpbr\"}");

	FILE *fp;
	fp = fopen("/etc/docker/daemon.json","w");
	fwrite(str,1,strlen(str),fp);
	fclose(fp);

	system("sudo brctl addbr dhcpbr");
	system("sudo ip link set dhcpbr up");
	bzero(str,strlen(str));
	sprintf(str,"sudo brctl addif dhcpbr %s",interface);
	system(str);
	bzero(str,strlen(str));
	sprintf(str,"sudo ifconfig %s 0.0.0.0",interface);
	system(str);
	system("sudo service docker restart");
	system("sudo ip link set dhcpbr down");
	system("sudo ip link set dhcpbr up");
	sleep(2);
	system("sudo dhclient dhcpbr");

	free(str);
	printf("--------------------- END -----------------------\n");
}


int public_network(char *subnet, char *ip, char *netmask, char *gw, char *interface){
	printf("------------ Public Network Setup ---------------\n");
	char *str,*br;
	str = (char *)calloc(1,128);
	br  = (char *)calloc(1,128);

	sprintf(str,"sudo docker network create --driver bridge --subnet %s --gateway %s my_bridge",subnet,gw);
	//printf("Command: %s\n",str);
	system(str);

	FILE *fp;
	system("sudo brctl show | grep br- |awk {'print $1'} > br");
	fp = fopen("br","r");
	while(fgets(br,127,fp)!=NULL){
		//printf("Bridge: %s\n",br);
		size_t ln = strlen(br) -1;
		if(*br && br[ln] == '\n')
			br[ln] = '\0';
		//printf("bridge: %s\n",br);
	}
	fclose(fp);

	sprintf(str,"sudo brctl addif %s %s",br, interface);
	//printf("Command: %s\n",str);
	system(str);

	//sprintf(str,"sudo ifconfig %s 0.0.0.0",interface);
	//printf("Command: %s\n",str);
	//system(str);

	sprintf(str,"sudo ifconfig %s %s netmask %s",br,ip,netmask);
	//printf("Command: %s\n",str);
	system(str);

	sprintf(str,"sudo route add default gw %s %s",gw,br);
	//printf("Command: %s\n",str);
	system(str);

	sprintf(str,"sudo echo 'nameserver %s' > /etc/resolv.conf",node.dns);
	//printf("Command: %s\n",str);
	system(str);

	//sprintf(str,"sudo ifconfig %s 0.0.0.0",interface);
	//printf("Command: %s\n",str);
	//system(str);

	free(str);
	printf("--------------------- END -----------------------\n");
}


int remove_docker_network(){
	printf("----------- Remove Docker Network ---------------\n");
	char *cmd,*str;
	cmd = (char*)calloc(1,128);
	str = (char*)calloc(1,128);

	FILE *fp;
	system("sudo docker network ls -q > docker_network");
	fp = fopen("docker_network","r");
	while(fgets(str,127,fp)!=NULL){
		//printf("Network: %s\n",str);
		size_t ln = strlen(str) -1;
		if(*str && str[ln] == '\n')
			str[ln] = '\0';
		//printf("Network ID: %s\n",str);
		sprintf(cmd,"sudo docker network rm %s",str);
		system(cmd);
		bzero(str,strlen(str));
		bzero(cmd,strlen(cmd));
	}
	fclose(fp);
	free(cmd);
	free(str);
	printf("--------------------- END -----------------------\n");
}


int remove_network(){
	printf("--------------- Remove System Network -----------\n");
	char *cmd,*str;
	cmd = (char*)calloc(1,128);
	str = (char*)calloc(1,128);

	FILE *fp;
	system("sudo brctl show |awk {'print $1'}|sed -n '2,15p' > network");
	fp = fopen("network","r");
	while(fgets(str,127,fp)!=NULL){
		size_t ln = strlen(str) -1;
		if(*str && str[ln] == '\n')
			str[ln] = '\0';
		sprintf(cmd,"sudo ip link set %s down",str);
		system(cmd);
		sprintf(cmd,"sudo brctl delbr %s",str);
		system(cmd);
		bzero(str,strlen(str));
		bzero(cmd,strlen(cmd));
	}
	fclose(fp);
	free(cmd);
	free(str);
	printf("--------------------- END -----------------------\n");
}

int remove_docker_container(){
	printf("--------------- Remove Docker Containers --------\n");
	char *cmd,*str;
	cmd = (char*)calloc(1,128);
	str = (char*)calloc(1,128);

	FILE *fp;

	system("sudo docker ps -q > docker_container");
	fp = fopen("docker_container","r");
	while(fgets(str,127,fp)!=NULL){
		size_t ln = strlen(str) -1;
		if(*str && str[ln] == '\n')
			str[ln] = '\0';
		sprintf(cmd,"sudo docker stop %s",str);
		system(cmd);
		sprintf(cmd,"sudo docker rm %s",str);
		system(cmd);
	}
	fclose(fp);

	system("sudo docker ps -aq > docker_container");
	fp = fopen("docker_container","r");
	while(fgets(str,127,fp)!=NULL){
		size_t ln = strlen(str) -1;
		if(*str && str[ln] == '\n')
			str[ln] = '\0';
		system(cmd);
		sprintf(cmd,"sudo docker rm %s",str);
		system(cmd);
	}
	fclose(fp);
	free(cmd);
	free(str);
	printf("--------------------- END -----------------------\n");
}

