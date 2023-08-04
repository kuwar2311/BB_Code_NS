/**************************************************************
file   : common.h
author : Kuwar Singh
date   : 2018-02-08
**************************************************************/
#include "common.h"
int set_memory_allocation(Node *node){
	node->server_ip 	= (char *)calloc(1,25);
	node->network_type 	= (char *)calloc(1,25);
	node->interface 	= (char *)calloc(1,25);
	node->node_ip 		= (char *)calloc(1,25);
	node->subnet 		= (char *)calloc(1,25);
	node->netmask 		= (char *)calloc(1,25);
	node->gateway 		= (char *)calloc(1,25);
	node->dns 		= (char *)calloc(1,25);
	node->virtual_tech 	= (char *)calloc(1,50);
	node->image_path 	= (char *)calloc(1,256);
	node->organization 	= (char *)calloc(1,75);
	node->organization_sector = (char *)calloc(1,50);
	node->organization_region = (char *)calloc(1,50);
}

int set_Client_conf(char *file,Node *node){
	char *buffer;
	buffer = (char *)calloc(1,512);
	set_memory_allocation(node);
	printf("\n==========================================\n");
	printf("Please Input the Node client Details..\n");
	printf("\tIP Type Public or DHCP   : ");
	scanf("%s",node->network_type);
	if(!strcmp(node->network_type,"Public")){
		printf("\tCentral Server IP: ");
		scanf("%s",node->server_ip);
		printf("\tInterface name   : ");
		scanf("%s",node->interface);
		printf("\tNetwork subnet   : ");
		scanf("%s",node->subnet);
		printf("\tNode IP Address  : ");
		scanf("%s",node->node_ip);
		printf("\tNetwork Netmask  : ");
		scanf("%s",node->netmask);
		printf("\tGateway IP       : ");
		scanf("%s",node->gateway);
		printf("\tDNS IP           : ");
		scanf("%s",node->dns);
		printf("\tVirtualization Technology\n\t(docker or virtualbox or docker|virtualbox): ");
		scanf("%s",node->virtual_tech);
		printf("\tVirtual images Path: ");
		scanf("%s",node->image_path);
		printf("\tOrganization name where node is deployed Example: CDAC-Mohali-Punjab    : ");
		scanf("%s",node->organization);
		printf("\tOrganization Sector Examole: education, energy, govt-national etc. : ");
		scanf("%s",node->organization_sector);
		printf("\tOrganization Region Examole: East, West, North, South, North-East : ");
		scanf("%s",node->organization_region);
		printf("\tLongitude of location: ");
		scanf("%f",&node->longitude);
		printf("\tLatitude of location: ");
		scanf("%f",&node->latitude);
		printf("\tTimer to get configuration file: ");
		scanf("%d",&node->conf_timer);
		printf("\tTimer to send captured data to central server: ");
		scanf("%d",&node->data_timer);
		printf("\tTimer to send pcap data to central server: ");
		scanf("%d",&node->pcap_timer);

		FILE *fp;
		fp = fopen(file, "w");
		if(fp!=NULL){
			bzero(buffer,strlen(buffer));
			sprintf(buffer,"<INIT>\n\t#ip address of the central server\n\t<server_ip>%s</server_ip>",node->server_ip);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t#network type of the client node as Public, Private or DHCP\n\t<network_type>Public</network_type>");

			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer, "\n\t#interface name of the client node as eth0\n\t<interface>%s</interface>",node->interface);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer, "\n\t#ip address of the node client base machine\n\t<node_ip>%s</node_ip>",node->node_ip);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer, "\n\t#subnet of the network\n\t<subnet>%s</subnet>",node->subnet);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t#netmask of the node\n\t<netmask>%s</netmask>",node->netmask);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t#gateway ip of the node machine\n\t<gateway>%s</gateway>",node->gateway);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t#dns ip of the node machine\n\t<dns>%s</dns>",node->dns);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer, "\n\t#virtualization technology used for the honeypot machines.\n\t#docker or virtualbox or both docker|virtualbox\n\t<virtual_tech>%s</virtual_tech>",node->virtual_tech);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer, "\n\t#honeypot images path.\n\t<image_path>%s</image_path>",node->image_path);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t#timer in seconds to check new  configuration file at server side\n\t<conf_timer>%d</conf_timer>",node->conf_timer);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t#timer in seconds to get captured data from honeypots\n\t<data_timer>%d</data_timer>",node->data_timer);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t#timer in seconds to send captured pcap data to server\n\t<pcap_timer>%d</pcap_timer>",node->pcap_timer);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t#Organization name where node is deployed Example: CDAC-Mohali-Punjab.\n\t<organization>%s</organization>",node->organization);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t#Organization Sector\n\t<organization_sector>%s</organization_sector>",node->organization_sector);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t#Organization Region East, West, North, South, North-East \n\t<organization_region>%s</organization_region>",node->organization_region);
			fwrite(buffer,1,strlen(buffer),fp);                                                                                   	

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t#longitude of node location\n\t<longitude>%lf</longitude>",node->longitude);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t#latitude of node location\n\t<latitude>%lf</latitude>\n</INIT>",node->latitude);
			fwrite(buffer,1,strlen(buffer),fp);
			fclose(fp);
			printf("config_files/initial.xml file created successfully......\n");
		}
		fp = fopen("../node_confs/config.conf", "w");
		if(fp!=NULL){
			char *str;
			str = (char*)calloc(1,128);

			printf("\tMAC Address of the Device: ");
			scanf("%s",str);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"<config>\n\t<flag>0</flag>\n\t<location>%s</location>\n\t<organization_sector>%s</organization_sector>\n\t<organization_region>%s</organization_region>\n\t<longitude>%lf</longitude>\n\t<latitude>%lf</latitude>\n\t<mac_address>%s</mac_address>\n\t<network_type>Public</network_type>",node->organization,node->organization_sector,node->organization_region,node->longitude,node->latitude,str);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t<static_ip>%s</static_ip>",node->node_ip);
			fwrite(buffer,1,strlen(buffer),fp);

			int count=0;
			printf("\tNumber of IP's available for Honeypot: ");
			scanf("%d",&count);

			while(count>0){
				printf("\tIP Address: ");
				scanf("%s",str);
				bzero(buffer,strlen(buffer));
				sprintf(buffer,"\n\t<public_ip>%s</public_ip>",str);
				fwrite(buffer,1,strlen(buffer),fp);
				count--;			
			}

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t<netmask>%s</netmask>",node->netmask);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t<gateway>%s</gateway>",node->gateway);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t<dns>%s</dns>\n</config>",node->dns);
			fwrite(buffer,1,strlen(buffer),fp);
			fclose(fp);
			printf("node_confs/config.conf created sucessfully.\n");
			free(str);
		}
		else{
			printf("error Config.conf\n");
		}
	}
	else if(!strcmp(node->network_type,"DHCP")){
		printf("\tCentral Server IP: ");
		scanf("%s",node->server_ip);
		printf("\tInterface name   : ");
		scanf("%s",node->interface);
		printf("\tVirtualization Technology\n\t(docker or virtualbox or docker|virtualbox): ");
		scanf("%s",node->virtual_tech);
		printf("\tVirtual images Path: ");
		scanf("%s",node->image_path);
		printf("\tNode Location    : ");
		scanf("%s",node->organization);
		printf("\tLongitude of location: ");
		scanf("%f",&node->longitude);
		printf("\tLatitude of location: ");
		scanf("%f",&node->latitude);
		printf("\tTimer to get configuration file: ");
		scanf("%d",&node->conf_timer);
		printf("\tTimer to send captured data to central server: ");
		scanf("%d",&node->data_timer);
		printf("\tTimer to send pcap data to central server: ");
		scanf("%d",&node->pcap_timer);

		FILE *fp;
		fp = fopen(file, "w");
		if(fp!=NULL){
			bzero(buffer,strlen(buffer));
			sprintf(buffer,"<INIT>\n\t#ip address of the central server\n\t<server_ip>%s</server_ip>",node->server_ip);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t#network type of the client node as Public, Private or DHCP\n\t<network_type>DHCP</network_type>");
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer, "\n\t#interface name of the client node as eth0\n\t<interface>%s</interface>",node->interface);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer, "\n\t#virtualization technology used for the honeypot machines.\n\t#docker or virtualbox or both docker|virtualbox\n\t<virtual_tech>%s</virtual_tech>",node->virtual_tech);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer, "\n\t#honeypot images path.\n\t<image_path>%s</image_path>",node->image_path);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t#timer in seconds to check new  configuration file at server side\n\t<conf_timer>%d</conf_timer>",node->conf_timer);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t#timer in seconds to get captured data from honeypots\n\t<data_timer>%d</data_timer>",node->data_timer);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t#timer in seconds to send captured pcap data to server\n\t<pcap_timer>%d</pcap_timer>",node->pcap_timer);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t#node location of the client node machine\n\t<organization>%s</organization>",node->organization);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t#longitude of node location\n\t<longitude>%lf</longitude>",node->longitude);
			fwrite(buffer,1,strlen(buffer),fp);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"\n\t#latitude of node location\n\t<latitude>%lf</latitude>\n</INIT>",node->latitude);
			fwrite(buffer,1,strlen(buffer),fp);
			fclose(fp);
			printf("config_files/initial.xml file created successfully......\n");
		}
		fp = fopen("../node_confs/config.conf", "w");
		if(fp!=NULL){
			char *str;
			str = (char*)calloc(1,128);

			printf("\tMAC Address of the Device: ");
			scanf("%s",str);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"<config>\n\t<flag>0</flag>\n\t<location>%s</location>\n\t<mac_address>%s</mac_address>\n\t<network_type>DHCP</network_type>\n</config>",node->organization,str);
			fwrite(buffer,1,strlen(buffer),fp);
			fclose(fp);
			printf("node_confs/config.conf file created successfully......\n");
			free(str);
		}
	}
	printf("============================================\n");
	printf("Execute bb_client again\n");
	free(buffer);
}


int display_hp(){
	struct Honeypot *ptr = hp_list;

	printf("\n=================== Running HP Details =======================\n");
	printf("HP_ID  HP_Name  VM_Type     VM_Name       IP_Address    HP_Type  Start_Time   UConf_ID     Thread ID\n");
	while(ptr!=NULL){
		printf("%3d    %7s  %7s  %12s  %14s  %5s    %10d  %5d %18lu\n",ptr->hp_id,ptr->hp_name,ptr->vm_type,ptr->vm_name,ptr->ip,ptr->hp_type,ptr->start_time,ptr->uconf_id,threads[ptr->hp_id]);
		sleep(2);
		ptr=ptr->next_hp;
	}
	printf("\n==============================================================\n");
	return 0;
}

void write_file(char *file_name, char *str){
	FILE *fp = fopen(file_name,"w");
	if(fp!=NULL){
		fprintf(fp,"%s",str);
		fclose(fp);
	}
}

void realtime_file(){
	char *nd;
	nd=(char*)calloc(1,128);

	check_path("/home/Node/");
	write_file("/home/Node/node_location",node.organization);
	write_file("/home/Node/server_ip",node.server_ip);

	bzero(nd,strlen(nd));
	sprintf(nd,"%d",node.node_id);
	write_file("/home/Node/node_id",nd);


	bzero(nd,strlen(nd));
	sprintf(nd,"%f",node.longitude);
	write_file("/home/Node/node_longitude",nd);

	bzero(nd,strlen(nd));
	sprintf(nd,"%f",node.latitude);
	write_file("/home/Node/node_lattitude",nd);

	free(nd);
}

int main(){
	printf("\n=================== Client Node Started ======================\n");
	FILE *fp;
	long size=-1;
	char *str_cmd;
	str_cmd = (char *)calloc(1,128);
	startupFile = (char*)calloc(1,256);
	servCmdFile = (char*)calloc(1,256);	
	hp_type     = (char*)calloc(1,128);

	struct Honeypot ahp_conf;
	int conf_timer=0;
	int data_timer=0;
	int pcap_timer=0;
	hp_id=0;

	memory_allocate(&ahp_conf);
	ahp_config = &ahp_conf;
	hp_list=NULL;

	if(!get_initial_details("../config_files/initials.xml",&node)){
		set_Client_conf("../config_files/initials.xml",&node);
	}
	while(1){
		FILE *fp = fopen("../config_files/client_reg","r");
		if(fp!=NULL){
			fclose(fp);
			break;
		}
		printf("============================================\n");
		printf("node_confs/config.conf Detail\n");
		system("sudo cat ../node_confs/config.conf");
		printf("============================================\n");
		printf("Do you want to proceed with above configuration ?\n");
		printf("If yes please press 1, for new configuration press 2.\n");
		int flag=0;
		printf("\tOption: ");
		scanf("%d",&flag);

		printf("\tYour Option: %d\n",flag);
		if(flag==1){
			break;
		}
		else if(flag==2){			
			set_Client_conf("../config_files/initials.xml",&node);
		}
		else{
			printf("Plese select a valid option");
		}
	}


	node_details(node);

	printf("\n================ Configuring IPtables Rule====================\n");
	set_data_control();

	printf("\n================ Configuring Node Network ====================\n");	
	configure_network(&node);
	
	sleep(5);

	bzero(str_cmd,strlen(str_cmd));
	sprintf(str_cmd,"sudo echo 'nameserver %s' > /etc/resolv.conf",node.dns);
	system(str_cmd);
	free(str_cmd);

	logger("Node Network Configured Successfully.","Main-Module");

	time_sync(node.server_ip);
	logger("Date Time Successfully updated with Server.","Main-Module");

	printf("\n============ Verifying Node is registered or not =============\n");
	fp=fopen("../config_files/client_reg","r");
	if(fp != NULL){
		fseek(fp,0,SEEK_END);
		size = ftell(fp);
		fclose(fp);
	}
	if(size<=0){
		printf("The node is not registered yet.\n");
		if(!register_node(&node)){
			printf("Unable to register node\n");
			exit(0);
		}
		else{
			logger("Node Registered Successfully.","Main-Module");
			printf("Node is registered successfully..\n");
			printf("Node ID: %d\n",node.node_id);
		}
	}
	else{
		if(read_nodeid(&node) == -1){
			printf("Error reading client_config file\n");
			exit(0);
		}
		printf("The node is already registered.\n");
		logger("Registered Node.","Main-Module");
		printf("Node ID: %d\n",node.node_id);
	}
	//Make file for snort realtime data
	realtime_file();

	printf("\n================== Honeypot configuration ====================\n");
	fp=fopen("../honeypot_config_file/honeypot.conf","r");
	if(fp!=NULL){
		fclose(fp);
		configure_manager("../honeypot_config_file/honeypot.conf");
		display_hp();	
	}
	else{
		printf("Honeypot Configuration file does not found.\n");
		printf("Generate a configuration file at server end\n");
	}

	start_pcap_capturing();

	printf("\n========= Timer for Reconfiguration & Data Capturing =========\n");

	conf_timer = 0;
	data_timer = node.data_timer;
	pcap_timer = node.pcap_timer;

	while(1){

		if(conf_timer<=0){
			//time_sync(node.server_ip);
			int resp_code;
			int exit_flag=0;
			while(1){
				resp_code=get_conf_file();
				if(resp_code != 2){
					break;
				}
				exit_flag++;
				if(exit_flag>5){
					remove_all_containers();
					exit(0);
				}
				sleep(60);
			}
			system("sudo iptables -F");
			if(resp_code==1){
				data_collector(1);
				hp_list=NULL;
				if(configure_manager("../config_files/honeypot.conf")==1){
					system("sudo mv -f ../config_files/honeypot.conf ../honeypot_config_file/honeypot.conf");
				}
				//updated on 10-11-2022
				else{
					configure_manager("../honeypot_config_file/honeypot.conf");
				}
				display_hp();
				start_pcap_capturing();
			}			
			conf_timer = node.conf_timer;
		}
		if(data_timer<=0){
			//time_sync(node.server_ip);
			data_collector(0);
			display_hp();
			data_timer = node.data_timer;
		}
		if(pcap_timer<=0){
			//time_sync(node.server_ip);
			pcap_data_collection();	
			pcap_timer = node.pcap_timer;
			start_pcap_capturing();
		}

		int t;
		if((conf_timer <= data_timer) && (conf_timer <= pcap_timer))
			t=conf_timer;
		if((data_timer <= pcap_timer) && (data_timer <= conf_timer))
			t=data_timer;
		if((pcap_timer <= data_timer) && (pcap_timer <= conf_timer))
			t=pcap_timer;

		system("sudo date");
		printf("\t Conf Timer: %d Seconds remaining.\n",conf_timer);
		printf("\t Data Timer: %d Seconds remaining.\n",data_timer);
		printf("\t Pcap Timer: %d Seconds remaining.\n",pcap_timer);
		
		set_data_control();

		conf_timer = conf_timer - t;
		data_timer = data_timer - t;
		pcap_timer = pcap_timer - t;
		sleep(t);
	}
	printf("\n==============================================================\n");
	return 0;
}
