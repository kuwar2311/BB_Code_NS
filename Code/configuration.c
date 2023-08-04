#include "common.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

void set_blank(struct Honeypot *h_config)
{
	h_config->hp_id=-1;
	bzero(h_config->vm_name,strlen(h_config->vm_name));
	bzero(h_config->hp_type,strlen(h_config->hp_type));
	bzero(h_config->hp_category,strlen(h_config->hp_category));
	bzero(h_config->os_type,strlen(h_config->os_type));
	bzero(h_config->vm_snapshot,strlen(h_config->vm_snapshot));
	bzero(h_config->hp_repo_path,strlen(h_config->hp_repo_path));
	h_config->next_hp=NULL;
}
void memory_allocate(struct Honeypot *h_config)
{
	h_config->vm_name       = (char*)calloc(1,128);
	h_config->hp_type       = (char*)calloc(1,128);
	h_config->hp_category   = (char*)calloc(1,128);
	h_config->os_type       = (char*)calloc(1,128);
	h_config->vm_snapshot   = (char*)calloc(1,128);
	h_config->hp_repo_path   = (char*)calloc(1,256);
	set_blank(h_config);
}

void set_value(struct Honeypot *source, struct Honeypot *dest)
{
	set_blank(dest);
	sprintf(dest->vm_name,"%s",source->vm_name);
	sprintf(dest->os_type,"%s",source->os_type);
	sprintf(dest->vm_snapshot,"%s",source->vm_snapshot);
	sprintf(dest->hp_type,"%s",source->hp_type);
	sprintf(dest->hp_category,"%s",source->hp_category);
	sprintf(dest->hp_repo_path,"%s",source->hp_repo_path);
	dest->hp_id=source->hp_id;
	dest->next_hp=hp_list;
	hp_list=dest;
}


void *ahp_honeypot(void *args)
{
	char *cmd;
	cmd= (char*)calloc(1,512);

	printf("\n######################################## Active Honeypot Started ####################################\n");
	ahp_data_flag=0;
	while(1)
	{
		printf("\n#####################################################################################################\n");
		printf("\t\t Honeypot Type    :%s\n",ahp_config->hp_type);
		printf("\t\t Honeypot Category:%s\n",ahp_config->hp_category);
		printf("#####################################################################################################\n");
		if(ahp_data_flag==0)
		{
			if(!strcmp(ahp_config->hp_category,"LIHP"))
			{
				int url_flag;
				url_flag = get_lahp_url();
				if(url_flag==1)
				{
					printf("\t\t Active Honeypot VM: %s\n",ahp_config->vm_name);

					if(check_vm_status(ahp_config->vm_name))
					{	
						vm_power_off(ahp_config->vm_name);
					}
					sprintf(cmd,"sudo mkdir %s/%s/%s/Shared/Config",node.image_path,ahp_config->os_type,ahp_config->vm_name);
					system(cmd);
					sprintf(cmd,"sudo cp -r ../url/URLDetails.xml %s/%s/%s/Shared/Config/",node.image_path,ahp_config->os_type,ahp_config->vm_name);
					system(cmd);
					vm_snapshot_restore(ahp_config->vm_name,ahp_config->vm_snapshot);
					vm_power_on(ahp_config->vm_name);
					while(check_vm_status(ahp_config->vm_name))
					{
						sleep(40);
					}
					data_collect_send(ahp_config);				
				}
				else if(url_flag==0)
				{
					printf("\n\t\t No New URL Pending to be Processed.\n");
					sleep(120);
				}
				else if(url_flag==2)
				{
					printf("\n\t\t Failed to get the url may be due to network connection.\n");
					sleep(120);
				}
			}
			else if(!strcmp(ahp_config->hp_category,"HIHP"))
			{
				int url_flag;
				url_flag = get_ahp_url();
				if(url_flag==1)
				{
					printf("\t\tActive Honeypot VM: %s\n",ahp_config->vm_name);

					if(check_vm_status(ahp_config->vm_name))
					{
						vm_power_off(ahp_config->vm_name);
					}	
					sprintf(cmd,"sudo mkdir %s/%s/%s/Shared/Config",node.image_path,ahp_config->os_type,ahp_config->vm_name);
					system(cmd);
					sprintf(cmd,"sudo cp -r ../url/URLDetails.xml %s/%s/%s/Shared/Config/",node.image_path,ahp_config->os_type,ahp_config->vm_name);
					system(cmd);
					vm_snapshot_restore(ahp_config->vm_name,ahp_config->vm_snapshot);
					vm_power_on(ahp_config->vm_name);
					while(check_vm_status(ahp_config->vm_name))
					{
						sleep(40);
					}
					data_collect_send(ahp_config);				
				}
				else if(url_flag==0)
				{
					printf("\n\t\t No New URL Pending to be Processed.\n");
					sleep(120);
				}
				else if(url_flag==2)
				{
					printf("\n\t\t Failed to get the url may be due to network connection.\n");
					sleep(120);
				}
			}
		}
		else
		{
			printf("Exiting from AHP Function\n");
			ahp_data_flag=0;
			break;
		}
	}
	free(cmd);
}

/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: Function for retrieving IP address configuration command
with ip address, netmask, gateway dns IP Value.
----------------------------------------------------------------------------*/
int set_network_value(struct Honeypot *hpNet,char *ncmd)
{
	char *ptr,*ptr1,*cmd,*temp;
	temp=(char*)calloc(1,128);
	cmd=(char*)calloc(1,128);

	FILE *fp;
	//stratupfile is configuration file which contains all the commands related to the configuration of honeypot, will executed by the guest machine.
	fp = fopen(startupFile,"a");

	if(fp!=NULL){
		strcpy(cmd,ncmd);
		while(1){
			if(ptr=strrchr(cmd,',')){
				bzero(temp,strlen(temp));
				strncpy(temp,cmd,strlen(cmd)-strlen(ptr));
				strcpy(cmd,temp);
				bzero(temp,strlen(temp));
				ptr++;

				if(!strcmp(ptr,"ip")){
					ptr1=strrchr(cmd,'@');
					strncpy(temp,cmd,strlen(cmd)-strlen(ptr1));
					strcat(temp,hpNet->ip);
					ptr1++;
					strcat(temp,ptr1);
					strcpy(cmd,temp);
				}
				else if(!strcmp(ptr,"netmask")){
					ptr1=strrchr(cmd,'@');
					strncpy(temp,cmd,strlen(cmd)-strlen(ptr1));
					strcat(temp,hpNet->netmask);
					ptr1++;
					strcat(temp,ptr1);
					strcpy(cmd,temp);
				}
				else if(!strcmp(ptr,"gateway")){
					ptr1=strrchr(cmd,'@');
					strncpy(temp,cmd,strlen(cmd)-strlen(ptr1));
					strcat(temp,hpNet->gateway);
					ptr1++;
					strcat(temp,ptr1);
					strcpy(cmd,temp);
				}                     
				else if(!strcmp(ptr,"dns")){
					ptr1=strrchr(cmd,'@');
					strncpy(temp,cmd,strlen(cmd)-strlen(ptr1));
					strcat(temp,hpNet->dns);
					ptr1++;
					strcat(temp,ptr1);
					strcpy(cmd,temp);
				}
			}
			else {
				break;
			}		
		}
		if (cmd!=NULL)
			fprintf(fp,"%s\n",cmd);
		fclose(fp);
	}
	else{
		printf("startup file '%s' does not exist.\n",startupFile);
	}
	free(cmd);
	free(temp);
	return 0;
}

/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: Function for retrieving network configuration commands
from service conf file.
----------------------------------------------------------------------------*/
void get_network_command(struct Honeypot *hpNet, xmlChar *service)
{
	xmlDocPtr doc;
	xmlNodePtr cur;
	xmlChar *scmd;

	doc = xmlParseFile(servCmdFile);
	if (doc == NULL ) {
		fprintf(stderr,"Document not parsed successfully. \n");
		return;
	}

	cur = xmlDocGetRootElement(doc);

	if (cur == NULL) {
		fprintf(stderr,"empty document\n");
		xmlFreeDoc(doc);
		return;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *) "service")) {
		fprintf(stderr,"document of the wrong type, root node != service");
		xmlFreeDoc(doc);
		return;
	}

	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)service))) {
			scmd = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
			set_network_value(hpNet, scmd);
			break;
		}
		cur = cur->next;
	}

	xmlFreeDoc(doc);
	return;
}

/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: Function for retrieving network related info from
honeypot.conf.
----------------------------------------------------------------------------*/
void get_network_details(xmlDocPtr doc, xmlNodePtr cur, struct Honeypot *hpNet){
	xmlChar *ncmd;
	hpNet->ip=NULL;
	hpNet->netmask=NULL;
	hpNet->gateway=NULL;
	hpNet->dns=NULL;

	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		//Retrieving network ip values related info from honeypot.conf.
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"IPAddress"))) {
			hpNet->ip= xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
		}
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"Netmask"))) {
			hpNet->netmask = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
		}
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"Gateway"))) {
			hpNet->gateway = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
		}
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"DNS"))) {
			hpNet->dns = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
		}
		cur = cur->next;
	}

	printf(" ------------------- Honeypot Network Details -----------------\n");
	printf("\tIP Address: %s\n",hpNet->ip);
	printf("\tNetmask Address: %s\n",hpNet->netmask);
	printf("\tGateway Address: %s\n",hpNet->gateway);
	printf("\tDNS Address: %s\n",hpNet->dns);
	printf(" --------------------------------------------------------------\n");
	//Retrieving network configuration command from serivces.conf		
	get_network_command(hpNet,"ip");
	get_network_command(hpNet,"gateway");
	get_network_command(hpNet,"dns");
	return;
}

/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: Function for writing Service configuration commands
from service conf file to startup file.
----------------------------------------------------------------------------*/
void write_service_command(xmlChar *service){

	xmlDocPtr doc;
	xmlNodePtr cur;
	xmlChar *scmd;
	FILE *fp;

	doc = xmlParseFile(servCmdFile);

	if (doc == NULL ) {
		fprintf(stderr,"Document not parsed successfully. \n");
		return;
	}

	cur = xmlDocGetRootElement(doc);

	if (cur == NULL) {
		fprintf(stderr,"empty document\n");
		xmlFreeDoc(doc);
		return;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *) "service")) {
		fprintf(stderr,"document of the wrong type, root node != service");
		xmlFreeDoc(doc);
		return;
	}

	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)service))) {
			scmd = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
			fp=fopen(startupFile,"a");
			if(fp!=NULL){
				if(scmd!=NULL){
					fprintf(fp,"%s  \n\n",scmd);
					free(scmd);
				}
				fclose(fp);
			}
		}
		cur = cur->next;
	}        
	xmlFreeDoc(doc);	
}


/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: Function for retrieving Honeypot related info about 
Virtual Machine from honeypot.conf.
----------------------------------------------------------------------------*/
int get_vm_details(xmlDocPtr doc, xmlNodePtr cur, struct Honeypot *hpConfig){

	hpConfig->vm_name = NULL;
	hpConfig->hp_type = NULL;
	hpConfig->hp_category = NULL;
	hpConfig->os_type = NULL;
	hpConfig->hp_repo_path = NULL;

	cur = cur->xmlChildrenNode;
	printf(" ---------------- Virtual Machine Details ---------------------\n");
	while (cur != NULL) {
		//honeypot_type is honeypot type like AHP,PHP, WHP.
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"HoneypotType")))
			hpConfig->hp_type = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
		//vm_type is name of virutal enviornment.
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"VmType")))
			hpConfig->vm_type = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
		//vm_name is name of virutal machine exist inf virtual box.
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"VmName")))
			hpConfig->vm_name = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
		//Name of honeypot like dionaea or honeypy
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"HoneypotName")))
			hpConfig->hp_name = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
		//Number of docker containers
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"ContainerCount")))
			hpConfig->dc_count = atoi(xmlNodeListGetString(doc,cur->xmlChildrenNode,1));
		//os_type is os type like Windows & Linux.
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"OsType")))
			hpConfig->os_type = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
		//honeypot_category is honeypot like High interaction or low (LIHP or HIHP).
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"HPCategory")))
			hpConfig->hp_category = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"SnapshotName")))
			hpConfig->vm_snapshot = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"repo")))
			hpConfig->hp_repo_path = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);

		cur = cur->next;
	}


	if(!strcmp(hpConfig->vm_type,"vbox")){
		if(hpConfig->vm_name!=NULL){
			/*setting the configuration file path which will contain all the
			commands and will executed by the guest machine*/

			if(!check_vm(hpConfig->vm_name)){
				printf("  %s Virtual machine not Registered.\n",hpConfig->vm_name);
				return 1;
			}
			else {
				if(!strcmp(hpConfig->os_type,"Windows"))
					sprintf(startupFile,"%s/%s/%s/Shared/Config.bat",node.image_path,hpConfig->os_type,hpConfig->vm_name);
				else
					sprintf(startupFile,"%s/%s/%s/Shared/Config.sh",node.image_path,hpConfig->os_type,hpConfig->vm_name);
				FILE *fp;
				fp=fopen(startupFile,"w");
				if(fp!=NULL)
					fclose(fp);
				else {
					fprintf(stderr,"  Virtual Machine Path (%s) not Found.\n",startupFile);
					return 1;
				}
			}
		}
		else{
			printf("  Virtual Machine Name can not be NULL!\n");
			return 1;
		}
	}

	printf("\tHP Type: %s\n",hpConfig->hp_type);
	printf("\tVM Name: %s\n",hpConfig->vm_name);
	printf("\tRepo   : %s\n",hpConfig->hp_repo_path);
	printf(" --------------------------------------------------------------\n");
	return 0;
}

/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: Function for retrieving service related info from 
honeypot.conf.
----------------------------------------------------------------------------*/
void get_web_server_details(xmlDocPtr doc, xmlNodePtr cur){
	xmlChar *serviceName;

	cur = cur->xmlChildrenNode;
	printf(" -------------- Configured Service Details ---------------------\n");
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"webserver_name"))) {
			serviceName = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
			//Retrieving the command related to the service.
			write_service_command(serviceName);
			printf("\tService Name: %s\n",serviceName);
			xmlFree(serviceName);
		}

		cur = cur->next;
	}
	printf(" ---------------------------------------------------------------\n");
	return;
}

/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: Function for retrieving service related info from 
honeypot.conf.
----------------------------------------------------------------------------*/
void get_service_details(xmlDocPtr doc, xmlNodePtr cur){
	xmlChar *serviceName;

	cur = cur->xmlChildrenNode;
	printf(" ---------------- Configured Service Details ------------------\n");
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"ServiceName"))) {
			serviceName = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
			//Retrieving the command related to the service.
			write_service_command(serviceName);
			printf("\tService Name: %s\n",serviceName);
			xmlFree(serviceName);
		}

		cur = cur->next;
	}
	printf(" --------------------------------------------------------------\n");
	return;
}

/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: Function for configuring honeypot.
----------------------------------------------------------------------------*/
int configure_vbox(struct Honeypot *hpConfig){
	char *buffer, *hpFile, *cmd;
	buffer = (char*)calloc(1,512);
	hpFile = (char*)calloc(1,128);
	cmd    = (char*)calloc(1,128);

	//power off the Virtual Machine
	if(check_vm_status(hpConfig->vm_name))
		vm_power_off(hpConfig->vm_name);

	//update honeypot file
	bzero(buffer,strlen(buffer));
	bzero(hpFile,strlen(hpFile));

	sprintf(cmd,"sudo mkdir %s/%s/%s/Shared/",node.image_path,hpConfig->os_type,hpConfig->vm_name);
	system(cmd);
	sprintf(cmd,"sudo mkdir %s/%s/%s/Shared/Config",node.image_path,hpConfig->os_type,hpConfig->vm_name);
	system(cmd);

	time_t rawtime;
	struct tm * timeinfo;
	char cur_time [80];
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	strftime (cur_time,80,"%Y/%m/%d %H:%M:%S",timeinfo);

	sprintf(hpFile,"%s/%s/%s/Shared/Config/honeypot.xml",node.image_path,hpConfig->os_type,hpConfig->vm_name);
	sprintf(buffer,"<honeypot>\n\t<node_id>%d</node_id>\n\t<os_type>%s</os_type>\n\t<honeypot_type>%s</honeypot_type>\n\t<honeypot_time>%s</honeypot_time>\n</honeypot>",node.node_id,hpConfig->os_type,hpConfig->hp_type,cur_time);

	FILE *fp;
	fp = fopen(hpFile,"w");
	if(fp!=NULL)
	{
		fprintf(fp,"%s",buffer);
		fclose(fp);
	}
	else
	{
		free(hpFile);
		free(cmd);
		free(buffer);
		return 1;
	}

	//Restore the snapshot
	printf("\tRestore Snapshot\n");
	vm_snapshot_restore(hpConfig->vm_name, hpConfig->vm_snapshot);

	bzero(hp_type,strlen(hp_type));

	printf("hpConfig.hp_type:%s\n",hpConfig->hp_type);

	//Active Honeypot thread
	if(!strcmp(hpConfig->hp_type,"AHP"))
	{
		int t;
		pthread_t ahp_thread;

		sprintf(hp_type,"ahp");
		//start_firewall();
		//get url list & call ahp thread for data sending.
		pthread_create(&ahp_thread,NULL,ahp_honeypot,(void*)&t);		
		pthread_detach(ahp_thread);
	}
	else if(!strcmp(hpConfig->hp_type,"PHP")){
		//Power on the Virtual Machine
		sprintf(hp_type,"php");
		//start_firewall();
		vm_power_on(hpConfig->vm_name);
	}
	else if(!strcmp(hpConfig->hp_type,"WHP")){
		//Power on the Virtual Machine
		sprintf(hp_type,"whp");
		//start_firewall();
		vm_power_on(hpConfig->vm_name);
	}
	else if(!strcmp(hpConfig->hp_type,"RHP")){
		//Power on the Virtual Machine
		sprintf(hp_type,"rhp");
		//start_firewall();
		vm_power_on(hpConfig->vm_name);
	}

	hp_id++;
	hpConfig->hp_id=hp_id;
	hpConfig->start_time=(int)time(NULL);
	hpConfig->hp_name=hpConfig->vm_name;

	hpConfig->next_hp=hp_list;
	hp_list=hpConfig;

	printf(" ---------------------------------------------------------------\n");
	free(hpFile);
	free(buffer);
	free(cmd);
	return 0;
}



/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: Function for configuring honeypot.
----------------------------------------------------------------------------*/
int configure_hp(xmlDocPtr doc, xmlNodePtr cur){

	cur = cur->xmlChildrenNode;
	struct Honeypot *hp = (struct Honeypot*) malloc(sizeof(struct Honeypot));

	while(cur !=NULL){
		//retrieving virtual Machine related info
		if((!xmlStrcmp(cur->name,(const xmlChar *)"UConfID"))){
			hp->uconf_id = atoi(xmlNodeListGetString(doc,cur->xmlChildrenNode,1));
		}
		//retrieving virtual Machine related info
		if((!xmlStrcmp(cur->name,(const xmlChar *)"VmInfo"))){
			if(get_vm_details(doc, cur, hp)){
				return 1;
			}
			sprintf(servCmdFile,"%s/%s/%s/Services.conf",node.image_path, hp->os_type, hp->vm_name);
		}
		//retrieving network related info and command
		if((!xmlStrcmp(cur->name,(const xmlChar *)"Network"))){
			if(hp->vm_name!=NULL){
				if(!strcmp(hp->vm_type,"docker"))
					get_docker_network_details(doc, cur, hp);
				else
					get_network_details(doc, cur, hp);
			}
			else{
				printf("  Virtual Machine Not found!\n");
				return 1;
			}
		}

		//retrieving Profile
		if((!xmlStrcmp(cur->name,(const xmlChar *)"Profile"))){
			printf("\n%s\n",xmlNodeListGetString(doc,cur->xmlChildrenNode,1));
			hp->hp_profile = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
		}

		//retrieving service related info and command
		else if((!xmlStrcmp(cur->name,(const xmlChar *)"ServiceInfo"))){
			if(hp->vm_name!=NULL){
				if(!strcmp(hp->vm_type,"docker"))
					get_docker_service_details(doc, cur);
				else
					get_service_details(doc, cur);
			}
			else{
				printf("  Virtual Machine Not found!\n");
				return 1;
			}
		}
		cur = cur->next;
	}

	printf("uconf_id: %d\n",hp->uconf_id);

	if(!strcmp(hp->vm_type,"docker")){
		configure_container(hp);
	}
	else{
		configure_vbox(hp);
	}
	printf(" ---------------------------------------------------------------\n");
	return 0;
}


/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: Function for parsing honeypot.conf.
----------------------------------------------------------------------------*/
int configure_manager(char *docname){
	xmlDocPtr doc;
	xmlNodePtr cur;

	logger("Configuration Manager Started.","Configuration-Module");
	//xml file parsing docname is honeypot.conf
	doc = xmlParseFile(docname);

	if (doc == NULL ) {
		fprintf(stderr,"%s Configuration file not Found.\n",docname);
		return 0;
	}

	cur = xmlDocGetRootElement(doc);

	if (cur == NULL) {
		fprintf(stderr,"empty document\n");
		xmlFreeDoc(doc);
		return 0;
	}

	//DHS is root node element of honeypot conf XML file.
	if (xmlStrcmp(cur->name, (const xmlChar *) "DHS")) {
		fprintf(stderr,"document of the wrong type, root node != DHS");
		xmlFreeDoc(doc);
		return 0;
	}

	cur = cur->xmlChildrenNode;

	while(cur !=NULL){
		//retrieving individual honeypt details & configure the honeypot.
		if((!xmlStrcmp(cur->name,(const xmlChar *)"Honeypot"))){
			if(configure_hp(doc, cur)){
				printf("  Error in configuration Module\n");
				printf(" ---------------------------------------------------------------\n");
				xmlFreeDoc(doc);
				return 0;
			}
		}
		cur = cur->next;
	}
	xmlFreeDoc(doc);
	logger("Configuration Manager Ended.","Configuration-Module");
	return 1;
}
