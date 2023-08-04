#include "common.h"

int white_list_ip(char *ip, char *node_ip){
	//printf("\nLocal  IP:'%s'",node_ip);
	//printf("\nPublic IP:'%s'\n",ip);
	char *cmd;
	cmd=(char*)calloc(1,256);

	bzero(cmd,strlen(cmd));
	sprintf(cmd,"sudo iptables -A INPUT -s '%s' -d '%s' -j ACCEPT",ip,node_ip);
	//printf("%s\n",cmd);
	system(cmd);
	bzero(cmd,strlen(cmd));
	sprintf(cmd,"sudo iptables -A OUTPUT -d '%s' -s '%s' -j ACCEPT",ip,node_ip);
	//printf("%s\n",cmd);
	system(cmd);

	bzero(cmd,strlen(cmd));
	free(cmd);
	return 0;
}

int base_rules(char *server_ip,char *node_ip)
{
	char *cmd;
	cmd=(char*)calloc(1,256);

	bzero(cmd,strlen(cmd));	
	sprintf(cmd,"sudo iptables -F");
	//printf("%s\n",cmd);
	system(cmd);

	bzero(cmd,strlen(cmd));	
	sprintf(cmd,"sudo iptables -X");
	//printf("%s\n",cmd);
	system(cmd);
	bzero(cmd,strlen(cmd));	
	sprintf(cmd,"sudo iptables -F INPUT");
	//printf("%s\n",cmd);
	system(cmd);
	bzero(cmd,strlen(cmd));	
	sprintf(cmd,"sudo iptables -F OUTPUT");
	//printf("%s\n",cmd);
	system(cmd);
	bzero(cmd,strlen(cmd));	
	sprintf(cmd,"sudo iptables -F -t nat");
	//printf("%s\n",cmd);
	system(cmd);

	bzero(cmd,strlen(cmd));	
	sprintf(cmd,"sudo iptables -A INPUT -s %s -d %s -j ACCEPT",server_ip,node_ip);
	//printf("%s\n",cmd);
	system(cmd);
	bzero(cmd,strlen(cmd));	
	sprintf(cmd,"sudo iptables -A OUTPUT -d %s -s %s -j ACCEPT",server_ip,node_ip);
	//printf("%s\n",cmd);
	system(cmd);

	char *ip;
	ip=(char*)calloc(1,25);
	FILE *fp;
	fp = fopen("../config_files/white_list_ip.txt","r");
	if(fp!=NULL){
		bzero(ip,strlen(ip));
		while(fgets(ip,20,fp)){
			for(int i=0;i<strlen(ip);i++){
				char ch = ip[i];
				if(ch<48){
					if(ch!=46)
						ip[i]='\0';
				}
				else if(ch>57){
					ip[i]='\0';
				}
			}
			white_list_ip(ip,node.node_ip);
		}
		fclose(fp);
	}
	bzero(ip,strlen(ip));
	free(ip);

	bzero(cmd,strlen(cmd));	
	sprintf(cmd,"sudo iptables -A INPUT -d %s -j DROP",node_ip);
	//printf("%s\n",cmd);
	system(cmd);
	bzero(cmd,strlen(cmd));	
	sprintf(cmd,"sudo iptables -A OUTPUT -s %s -j DROP",node_ip);
	//printf("%s\n",cmd);
	system(cmd);

	bzero(cmd,strlen(cmd));	
	sprintf(cmd,"sudo iptables -A INPUT -i lo -j ACCEPT");
	//printf("%s\n",cmd);
	system(cmd);
	bzero(cmd,strlen(cmd));	
	sprintf(cmd,"sudo iptables -A OUTPUT -o lo -j ACCEPT");
	//printf("%s\n",cmd);
	system(cmd);

	printf("IPTable Rules configured\n");

	bzero(cmd,strlen(cmd));	
	free(cmd);	
	return 0;	
}

int honeypot_rules()
{
	char *cmd;
	cmd=(char*)calloc(1,256);

	bzero(cmd,strlen(cmd));	
	sprintf(cmd,"sudo iptables -A FORWARD -p tcp --syn -m connlimit --connlimit-above 25 -m physdev --physdev-in tap2 -j REJECT");
	system(cmd);

	bzero(cmd,strlen(cmd));	
	sprintf(cmd,"sudo iptables -A FORWARD -p tcp --syn -m connlimit --connlimit-above 25 -m physdev --physdev-out tap2 -j REJECT");
	system(cmd);

	bzero(cmd,strlen(cmd));	
	free(cmd);	
	return 0;
}

int set_data_control()
{
	base_rules(node.server_ip,node.node_ip);
	//honeypot_rules();	
	system("sudo ufw disable > /dev/null");
	system("systemctl disable docker > /dev/null");
	system("chown -R cdac:cdac ../../BB_Code_1_5");
	return 0;
}
