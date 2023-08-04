#include "common.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>


/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: Function for retrieving network related info from
honeypot.conf.
----------------------------------------------------------------------------*/
void write_data_infile(char *file, char *data){
	FILE *fd = fopen(file,"a+");
	fputs(data,fd);
	fclose(fd);
}


/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: set docker image conf path for 
new docker image pulled from docker registry.
----------------------------------------------------------------------------*/
int set_image_conf_path(char *path, char *hp_name){
	printf("File Path: %s, hp_name: %s\n",path,hp_name);
	char *found,*temp;
	found = strrchr(path,'/');
	if(found){
		size_t len = found -path;
		temp=malloc(len+2);
		memcpy(temp,path,len);
		temp[len]='\0';
		printf("path: %s\n",temp);
		check_path(temp);
		char *cmd;
		cmd =(char*)calloc(1,512);

		sprintf(cmd,"cp '../%s/Service.conf' '%s/'",hp_name,temp);
		printf("Command: %s\n",cmd);
		system(cmd);

		bzero(cmd,strlen(cmd));
		sprintf(cmd,"%s/%s",temp,hp_name);
		check_path(cmd);

		bzero(cmd,strlen(cmd));
		sprintf(cmd,"cp '../%s/Data_collection.xml' '%s/%s/'",hp_name,temp,hp_name);
		printf("Command: %s\n",cmd);
		system(cmd);

		bzero(temp,strlen(temp));
		bzero(cmd,strlen(cmd));

		free(cmd);
		free(temp);
	}
}



/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: Function for retrieving network related info from
honeypot.conf.
----------------------------------------------------------------------------*/
int get_data_from_xmlfile(char *docname,char *service, char *file, char *network_type){
	printf("Service conf file path: %s\n",docname);
	char *path;
	xmlDocPtr doc;
	xmlNodePtr cur,cur1;

	if(! access( docname, F_OK ) == 0 ) {
		printf("'%s' File does not exists.\n",docname);
		set_image_conf_path(docname, service);
	}
	doc = xmlParseFile(docname);
	if(doc == NULL){
		fprintf(stderr,"%s Configuration file not Found.\n",docname);
		free(docname);
		return 0;
	}
	cur = xmlDocGetRootElement(doc);
	if(cur == NULL){
		fprintf(stderr,"empty document\n");
		xmlFreeDoc(doc);
		free(docname);
		return 0;
	}
	//service is root node element of Services.conf XML file.
	if(xmlStrcmp(cur->name, (const xmlChar *) "service")){
		fprintf(stderr,"document of the wrong type, root node != DHS");
		xmlFreeDoc(doc);
		free(docname);
		return 0;
	}
	cur = cur->xmlChildrenNode;
	FILE *fd = fopen(file,"wb");
	if(!strcmp(network_type,"DHCP")){
		fputs("sleep 30s\n",fd);
	}
	while(cur !=NULL){
		//retrieving individual honeypt details & configure the honeypot.
		if((!xmlStrcmp(cur->name,(const xmlChar *)service))){
			path = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
			fputs(path,fd);
			fputs("\n",fd);
		}
		cur=cur->next;
	}
	fclose(fd);
	xmlFreeDoc(doc);
	return 0;
}


/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: Function for retrieving network related info from
honeypot.conf.
----------------------------------------------------------------------------*/
void get_docker_network_details(xmlDocPtr doc, xmlNodePtr cur, struct Honeypot *hpNet){
	xmlChar *ncmd;
	hpNet->ip=NULL;
	hpNet->netmask=NULL;
	hpNet->gateway=NULL;
	hpNet->dns=NULL;
	hpNet->network_type=NULL;

	ip_list=NULL;

	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		//Retrieving network ip values related info from honeypot.conf.
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"NetworkType"))) {
			hpNet->network_type= xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
		}
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"IPAddress"))) {
			struct IP *ip = (struct IP*) malloc(sizeof(struct IP));
			ip->ip_address = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
			ip->next=ip_list;
			ip_list=ip;
			//hpNet->ip= xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
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
	//	printf("\tIP Address: %s\n",hpNet->ip);
	printf("\tNetmask Address: %s\n",hpNet->netmask);
	printf("\tGateway Address: %s\n",hpNet->gateway);
	printf("\tDNS Address: %s\n",hpNet->dns);
	printf(" --------------------------------------------------------------\n");
	return;
}

/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: Function for retrieving service related info from 
honeypot.conf.
----------------------------------------------------------------------------*/
void get_docker_service_details(xmlDocPtr doc, xmlNodePtr cur){
	xmlChar *serviceName;
	service_list=NULL;

	cur = cur->xmlChildrenNode;
	printf(" ---------------- Configured Service Details ------------------\n");
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"ServiceName"))) {
			serviceName = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
			struct service *sr = (struct service*) malloc(sizeof(struct service));
			sr->service_name=serviceName;
			sr->next=service_list;
			service_list=sr;
		}
		cur = cur->next;
	}
	printf("service list\n");
	struct service *ptr = service_list;
	while(ptr!=NULL){
		printf("Service Name: %s\n",ptr->service_name);
		ptr=ptr->next;
	}

	printf(" --------------------------------------------------------------\n");
	return;
}


int configure_container(struct Honeypot *hp){
	char *buffer, *hpFile, *cmd;
	buffer = (char*)calloc(1,512);
	hpFile = (char*)calloc(1,128);
	cmd    = (char*)calloc(1,128);

	if(!strcmp(hp->network_type,"DHCP")){
		int i = hp->dc_count;
		while(i>0){
			i--;
			printf("=================================================================\n");
			struct Honeypot *hpConfig = (struct Honeypot*) malloc(sizeof(struct Honeypot));
			hp_id++;
			hpConfig->hp_id        = hp_id;
			hpConfig->hp_name      = hp->hp_name;
			hpConfig->vm_type      = hp->vm_type;
			hpConfig->vm_name      = hp->vm_name;
			hpConfig->hp_type      = hp->hp_type;
			hpConfig->network_type = hp->network_type;
			hpConfig->ip           = hp->ip;
			hpConfig->netmask      = hp->netmask;
			hpConfig->gateway      = hp->gateway;
			hpConfig->dns          = hp->dns;
			hpConfig->uconf_id     = hp->uconf_id;
			hpConfig->hp_profile   = hp->hp_profile;
			hpConfig->hp_repo_path = hp->hp_repo_path;

			create_container(hpConfig);

			FILE *fp;
			fp = fopen("server_ip","wb");
			if(fp!=NULL){
				fprintf(fp,"%s",node.server_ip);
				fclose(fp);
				copy_base_to_container(hpConfig,"server_ip","/root/");	
			}

			fp = fopen("node_details","wb");
			if(fp!=NULL){
				if(!strcmp(hpConfig->hp_name,"dionaea"))
					fprintf(fp,"%d\\/%s\\/%f\\/%f", node.node_id, node.organization, node.latitude, node.longitude);
				else if(!strcmp(hpConfig->hp_name,"HoneyPy"))
					fprintf(fp,"%d\\\\/%s\\\\/%f\\\\/%f", node.node_id, node.organization, node.latitude, node.longitude);
				fclose(fp);
				copy_base_to_container(hpConfig,"node_details","/root/");
			}


			fp = fopen("health_config.conf","wb");
			if(fp!=NULL){
				fprintf(fp,"{\"node_id\": \"%d\", \"hp_name\": \"%s\", \"u_conf_id\": \"%d\", \"data_timer\": 600, \"log_file_path\": \"/var/log/healthData.log\"}", node.node_id, hpConfig->hp_name, hpConfig->uconf_id);
				fclose(fp);
				copy_base_to_container(hpConfig,"health_config.conf","/root/");
			}


			copy_base_to_container(hpConfig,"../Code/start_healh_monitor.sh","/bin/");
			copy_base_to_container(hpConfig,"../Code/HoneypotHealth.py","/root/");
			if(!strcmp(hpConfig->hp_name,"dionaea")){
				system("sudo rm -rf ../Dionaea/services-enabled/;mkdir ../Dionaea/services-enabled/");
				struct service *serv_ptr = service_list;
				while(serv_ptr!=NULL){
					bzero(buffer,strlen(buffer));
					sprintf(buffer,"sudo cp -r ../Dionaea/services-available/%s.yaml ../Dionaea/services-enabled/",serv_ptr->service_name);
					printf("Command: %s\n",buffer);
					system(buffer);
					serv_ptr=serv_ptr->next;
				}
				bzero(buffer,strlen(buffer));
				sprintf(buffer,"%s/%s/Service.conf",node.image_path,hpConfig->vm_name);
				get_data_from_xmlfile(buffer,hpConfig->hp_name,"Config.sh",hpConfig->network_type);

				copy_base_to_container(hpConfig,"Config.sh","/bin/");
				//copy_base_to_container(hpConfig,"../Code/node_detail.sh","/bin/");
				copy_base_to_container(hpConfig,"../Dionaea/dionaea_start.sh","/bin/");
				copy_base_to_container(hpConfig,"../Dionaea/services-enabled","/root/");
				copy_base_to_container(hpConfig,"../Dionaea/dionaea.sh","/bin/");
				copy_base_to_container(hpConfig,"../Dionaea/sqllite.sh","/opt/dionaea/var/dionaea/");
				copy_base_to_container(hpConfig,"../Dionaea/logsql.py","/opt/dionaea/lib/dionaea/python/dionaea/");
			}
			else if(!strcmp(hpConfig->hp_name,"HoneyPy")){
				struct service *ptr = service_list;
				bzero(buffer,strlen(buffer));		
				sprintf(buffer,"sudo /usr/bin/python ../HoneyPy/file_edit.py %s/%s/%s/services.cfg",node.image_path,hpConfig->vm_name,hpConfig->hp_name);

				while(ptr!=NULL){
					sprintf(buffer,"%s %s",buffer, ptr->service_name);
					ptr=ptr->next;
				}

				system(buffer);

				bzero(buffer,strlen(buffer));
				sprintf(buffer,"%s/%s/Service.conf",node.image_path,hpConfig->vm_name);
				get_data_from_xmlfile(buffer,hpConfig->hp_name,"Config.sh",hpConfig->network_type);

				bzero(hpFile,strlen(hpFile));
				sprintf(hpFile,"%s/%s/%s/services.cfg",node.image_path,hpConfig->vm_name,hpConfig->hp_name);
				copy_base_to_container(hpConfig,"../HoneyPy/honeypy_commands.sh","/root/");
				copy_base_to_container(hpConfig,"../HoneyPy/HoneyPy-master","/root/");
				copy_base_to_container(hpConfig,"../HoneyPy/ipt-kit","/root/");
				copy_base_to_container(hpConfig,hpFile,"/root/HoneyPy-master/etc/");
				copy_base_to_container(hpConfig,"Config.sh","/bin/");
			}
			else if(!strcmp(hpConfig->hp_name,"cdac_hp")){
				bzero(buffer,strlen(buffer));
				sprintf(buffer,"cd /root/Code/HoneyPot_v0.3.1/;python main.py %s nccc %d",hpConfig->hp_profile,node.node_id);
				FILE *fd = fopen("../cdac_hp/cdac_hp_start.sh","w");
				fputs(buffer,fd);
				fclose(fd);

				bzero(buffer,strlen(buffer));
				sprintf(buffer,"%s/%s/Service.conf",node.image_path,hpConfig->vm_name);
				get_data_from_xmlfile(buffer,hpConfig->hp_name,"Config.sh",hpConfig->network_type);

				bzero(hpFile,strlen(hpFile));
				copy_base_to_container(hpConfig,"Config.sh","/bin/");
				copy_base_to_container(hpConfig,"../cdac_hp/Code/","/root/");
				copy_base_to_container(hpConfig,"../cdac_hp/cdac_hp_data_collector.sh","/bin/");
				copy_base_to_container(hpConfig,"../cdac_hp/cdac_hp_start.sh","/bin/");
				copy_base_to_container(hpConfig,"../cdac_hp/database/data_log.db","/root/");
				copy_base_to_container(hpConfig,"../cdac_hp/database/data_log.db","/var/log/");
			}
			else if(!strcmp(hpConfig->hp_name,"conpot")){
				bzero(buffer,strlen(buffer));
				sprintf(buffer,"rm -rf /var/log/conpot/;\nmkdir /var/log/conpot/;\nsudo chown -R conpot:conpot /var/log/conpot/;\nsudo -H -u conpot bash -c '/home/conpot/.local/bin/conpot --template %s --config /home/conpot/conpot.cfg --logfile /var/log/conpot/conpot.log -f --temp_dir /tmp';\nvim kk",hpConfig->hp_profile);
				FILE *fd = fopen("../conpot/conpot_start.sh","w");
				fputs(buffer,fd);
				fclose(fd);

				bzero(buffer,strlen(buffer));
				sprintf(buffer,"%s/%s/Service.conf",node.image_path,hpConfig->vm_name);
				get_data_from_xmlfile(buffer,hpConfig->hp_name,"Config.sh",hpConfig->network_type);

				bzero(hpFile,strlen(hpFile));
				copy_base_to_container(hpConfig,"Config.sh","/bin/");
				copy_base_to_container(hpConfig,"../conpot/conpot.sh","/bin/");
				copy_base_to_container(hpConfig,"../conpot/conpot_start.sh","/bin/");
				copy_base_to_container(hpConfig,"../conpot/conpot.cfg","/home/conpot/.local/lib/python3.6/site-packages/conpot-0.6.0-py3.6.egg/conpot/testing.cfg");
				//copy_base_to_container(hpConfig,"../conpot/templates","/home/conpot/.local/lib/python3.6/site-packages/conpot-0.6.0-py3.6.egg/conpot/");
			}
			else if(!strcmp(hpConfig->hp_name,"cowrie")){
				bzero(buffer,strlen(buffer));
				sprintf(buffer,"%s/%s/Service.conf",node.image_path,hpConfig->vm_name);
				get_data_from_xmlfile(buffer,hpConfig->hp_name,"Config.sh",hpConfig->network_type);

				copy_base_to_container(hpConfig,"../cowrie/cowrie.cfg","/home/cowrie/cowrie/etc/cowrie.cfg");
				copy_base_to_container(hpConfig,"../cowrie/cowrie_start.sh","/bin/");
				copy_base_to_container(hpConfig,"../cowrie/cowrie.sh","/bin/");
				copy_base_to_container(hpConfig,"Config.sh","/bin/");
			}
			else if(!strcmp(hpConfig->hp_name,"telnet_hpot")){
				bzero(buffer,strlen(buffer));
				sprintf(buffer,"%s/%s/Service.conf",node.image_path,hpConfig->vm_name);
				get_data_from_xmlfile(buffer,hpConfig->hp_name,"Config.sh",hpConfig->network_type);

				copy_base_to_container(hpConfig,"../telnet_hpot/config.dist.yaml","/telnet-iot-honeypot-master/config.dist.yaml");
				copy_base_to_container(hpConfig,"../telnet_hpot/telnet_hpot_start.sh","/bin/");
				copy_base_to_container(hpConfig,"../telnet_hpot/telnet_hpot.sh","/bin/");
				copy_base_to_container(hpConfig,"Config.sh","/bin/");
			}
			else{
				bzero(buffer,strlen(buffer));
				sprintf(buffer,"%s/%s/Service.conf",node.image_path,hpConfig->vm_name);
				get_data_from_xmlfile(buffer,hpConfig->hp_name,"Config.sh",hpConfig->network_type);

				char *hp_file_name;
				hp_file_name = (char *)calloc(1,128);

				sprintf(hp_file_name,"../%s/%s.sh",hpConfig->hp_name,hpConfig->hp_name);
				copy_base_to_container(hpConfig,hp_file_name,"/bin/");
				sprintf(hp_file_name,"../%s/%s_start.sh",hpConfig->hp_name,hpConfig->hp_name);
				copy_base_to_container(hpConfig,hp_file_name,"/bin/");
				copy_base_to_container(hpConfig,"Config.sh","/bin/");
				bzero(hp_file_name,strlen(hp_file_name));
				free(hp_file_name);
			}
			hpConfig->start_time=(int)time(NULL);
			start_container(hpConfig);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"sudo nsenter -t $(docker-pid %s_%d) -n -- dhclient eth0",hpConfig->hp_name,hpConfig->hp_id);
			system(buffer);

			bzero(buffer,strlen(buffer));
			sprintf(buffer,"sudo docker-ip %s_%d > docker_ip",hpConfig->hp_name,hpConfig->hp_id);
			system(buffer);

			FILE *fp_ip;
			fp_ip=fopen("docker_ip","r");
			char *docker_ip = malloc(16);
			fgets(docker_ip,15,fp_ip);
			if(docker_ip[strlen(docker_ip)-1]== '\n')
				docker_ip[strlen(docker_ip)-1]='\0';			
			fclose(fp_ip);

			hpConfig->ip = docker_ip;		
			hpConfig->next_hp=hp_list;
			hp_list=hpConfig;
			printf("=================================================================\n");
		}
	}
	else{
		struct IP *ptr = ip_list;
		while(ptr!=NULL){
			printf("=================================================================\n");
			struct Honeypot *hpConfig = (struct Honeypot*) malloc(sizeof(struct Honeypot));
			hp_id++;
			hpConfig->hp_id        = hp_id;
			hpConfig->hp_name      = hp->hp_name;
			hpConfig->vm_type      = hp->vm_type;
			hpConfig->vm_name      = hp->vm_name;
			hpConfig->hp_type      = hp->hp_type;
			hpConfig->network_type = hp->network_type;
			hpConfig->ip           = ptr->ip_address;
			hpConfig->netmask      = hp->netmask;
			hpConfig->gateway      = hp->gateway;
			hpConfig->dns          = hp->dns;
			hpConfig->uconf_id     = hp->uconf_id;
			hpConfig->hp_profile   = hp->hp_profile;
			hpConfig->hp_repo_path = hp->hp_repo_path;

			ptr=ptr->next;	

			create_container(hpConfig);

			FILE *fp;
			fp = fopen("server_ip","wb");
			if(fp!=NULL){
				fprintf(fp,"%s",node.server_ip);
				fclose(fp);
				copy_base_to_container(hpConfig,"server_ip","/root/");	
			}

			fp = fopen("node_details","wb");
			if(fp!=NULL){
				if(!strcmp(hpConfig->hp_name,"dionaea"))
					fprintf(fp,"%d\\/%s\\/%f\\/%f", node.node_id, node.organization, node.latitude, node.longitude);
				else if(!strcmp(hpConfig->hp_name,"HoneyPy"))
					fprintf(fp,"%d\\\\/%s\\\\/%f\\\\/%f", node.node_id, node.organization, node.latitude, node.longitude);
				fclose(fp);
				copy_base_to_container(hpConfig,"node_details","/root/");
			}


			fp = fopen("health_config.conf","wb");
			if(fp!=NULL){
				fprintf(fp,"{\"node_id\": \"%d\", \"hp_name\": \"%s\", \"u_conf_id\": \"%d\", \"data_timer\": 600, \"log_file_path\": \"/var/log/healthData.log\"}", node.node_id, hpConfig->hp_name, hpConfig->uconf_id);
				fclose(fp);
				copy_base_to_container(hpConfig,"health_config.conf","/root/");
			}

			copy_base_to_container(hpConfig,"../Code/HoneypotHealth.py","/root/");
			copy_base_to_container(hpConfig,"../Code/start_healh_monitor.sh","/bin/");
			if(!strcmp(hpConfig->hp_name,"dionaea")){
				system("sudo rm -rf ../Dionaea/services-enabled/;mkdir ../Dionaea/services-enabled/");
				struct service *ptr = service_list;
				while(ptr!=NULL){
					bzero(buffer,strlen(buffer));
					sprintf(buffer,"sudo cp -r ../Dionaea/services-available/%s.yaml ../Dionaea/services-enabled/",ptr->service_name);
					system(buffer);
					ptr=ptr->next;
				}

				bzero(buffer,strlen(buffer));
				sprintf(buffer,"%s/%s/Service.conf",node.image_path,hpConfig->vm_name);
				get_data_from_xmlfile(buffer,hpConfig->hp_name,"Config.sh",hpConfig->network_type);

				copy_base_to_container(hpConfig,"Config.sh","/bin/");
				//copy_base_to_container(hpConfig,"../Code/node_detail.sh","/bin/");
				copy_base_to_container(hpConfig,"../Dionaea/dionaea_start.sh","/bin/");
				copy_base_to_container(hpConfig,"../Dionaea/services-enabled","/root/");
				copy_base_to_container(hpConfig,"../Dionaea/dionaea.sh","/bin/");
				copy_base_to_container(hpConfig,"../Dionaea/sqllite.sh","/opt/dionaea/var/dionaea/");
				copy_base_to_container(hpConfig,"../Dionaea/logsql.py","/opt/dionaea/lib/dionaea/python/dionaea/");
			}
			else if(!strcmp(hpConfig->hp_name,"HoneyPy")){
				struct service *ptr = service_list;
				bzero(buffer,strlen(buffer));		
				sprintf(buffer,"sudo /usr/bin/python ../HoneyPy/file_edit.py %s/%s/%s/services.cfg",node.image_path,hpConfig->vm_name,hpConfig->hp_name);

				while(ptr!=NULL){
					sprintf(buffer,"%s %s",buffer, ptr->service_name);
					ptr=ptr->next;
				}

				system(buffer);

				bzero(buffer,strlen(buffer));
				sprintf(buffer,"%s/%s/Service.conf",node.image_path,hpConfig->vm_name);
				get_data_from_xmlfile(buffer,hpConfig->hp_name,"Config.sh",hpConfig->network_type);

				bzero(hpFile,strlen(hpFile));
				sprintf(hpFile,"%s/%s/%s/services.cfg",node.image_path,hpConfig->vm_name,hpConfig->hp_name);
				copy_base_to_container(hpConfig,"../HoneyPy/honeypy_commands.sh","/root/");
				copy_base_to_container(hpConfig,"../HoneyPy/HoneyPy-master","/root/");
				copy_base_to_container(hpConfig,"../HoneyPy/ipt-kit","/root/");
				copy_base_to_container(hpConfig,hpFile,"/root/HoneyPy-master/etc/");
				copy_base_to_container(hpConfig,"Config.sh","/bin/");
			}		
			else if(!strcmp(hpConfig->hp_name,"cdac_hp")){
				bzero(buffer,strlen(buffer));
				sprintf(buffer,"cd /root/Code/HoneyPot_v0.3.1/;python main.py %s nccc %d",hpConfig->hp_profile,node.node_id);
				FILE *fd = fopen("../cdac_hp/cdac_hp_start.sh","w");
				fputs(buffer,fd);
				fclose(fd);

				bzero(buffer,strlen(buffer));
				sprintf(buffer,"%s/%s/Service.conf",node.image_path,hpConfig->vm_name);
				get_data_from_xmlfile(buffer,hpConfig->hp_name,"Config.sh",hpConfig->network_type);

				bzero(hpFile,strlen(hpFile));
				copy_base_to_container(hpConfig,"Config.sh","/bin/");
				copy_base_to_container(hpConfig,"../cdac_hp/Code/","/root/");
				copy_base_to_container(hpConfig,"../cdac_hp/cdac_hp_start.sh","/bin/");
				copy_base_to_container(hpConfig,"../cdac_hp/cdac_hp_data_collector.sh","/bin/");
				copy_base_to_container(hpConfig,"../cdac_hp/database/data_log.db","/root/");
				copy_base_to_container(hpConfig,"../cdac_hp/database/data_log.db","/var/log/");
			}
			else if(!strcmp(hpConfig->hp_name,"conpot")){
				bzero(buffer,strlen(buffer));
				sprintf(buffer,"rm -rf /var/log/conpot/;\nmkdir /var/log/conpot/;\nsudo chown -R conpot:conpot /var/log/conpot/;\nsudo -H -u conpot bash -c '/home/conpot/.local/bin/conpot --template %s --config /home/conpot/conpot.cfg --logfile /var/log/conpot/conpot.log -f --temp_dir /tmp';\nvim kk",hpConfig->hp_profile);
				FILE *fd = fopen("../conpot/conpot_start.sh","w");
				fputs(buffer,fd);
				fclose(fd);

				bzero(buffer,strlen(buffer));
				sprintf(buffer,"%s/%s/Service.conf",node.image_path,hpConfig->vm_name);
				get_data_from_xmlfile(buffer,hpConfig->hp_name,"Config.sh",hpConfig->network_type);

				bzero(hpFile,strlen(hpFile));
				copy_base_to_container(hpConfig,"Config.sh","/bin/");
				copy_base_to_container(hpConfig,"../conpot/conpot.sh","/bin/");
				copy_base_to_container(hpConfig,"../conpot/conpot_start.sh","/bin/");
				copy_base_to_container(hpConfig,"../conpot/conpot.cfg","/home/conpot/");
				copy_base_to_container(hpConfig,"../conpot/conpot.cfg","/home/conpot/.local/lib/python3.6/site-packages/conpot-0.6.0-py3.6.egg/conpot/testing.cfg");
				//copy_base_to_container(hpConfig,"../conpot/templates","/home/conpot/.local/lib/python3.6/site-packages/conpot-0.6.0-py3.6.egg/conpot/");
			}
			else if(!strcmp(hpConfig->hp_name,"cowrie")){
				bzero(buffer,strlen(buffer));
				sprintf(buffer,"%s/%s/Service.conf",node.image_path,hpConfig->vm_name);
				get_data_from_xmlfile(buffer,hpConfig->hp_name,"Config.sh",hpConfig->network_type);

				copy_base_to_container(hpConfig,"../cowrie/cowrie.cfg","/home/cowrie/cowrie/etc/cowrie.cfg");
				copy_base_to_container(hpConfig,"../cowrie/cowrie.sh","/bin/");
				copy_base_to_container(hpConfig,"../cowrie/cowrie_start.sh","/bin/");
				copy_base_to_container(hpConfig,"Config.sh","/bin/");
			}
			else if(!strcmp(hpConfig->hp_name,"telnet_hpot")){
				bzero(buffer,strlen(buffer));
				sprintf(buffer,"%s/%s/Service.conf",node.image_path,hpConfig->vm_name);
				get_data_from_xmlfile(buffer,hpConfig->hp_name,"Config.sh",hpConfig->network_type);

				copy_base_to_container(hpConfig,"../telnet_hpot/config.dist.yaml","/telnet-iot-honeypot-master/config.dist.yaml");
				copy_base_to_container(hpConfig,"../telnet_hpot/telnet_hpot_start.sh","/bin/");
				copy_base_to_container(hpConfig,"../telnet_hpot/telnet_hpot.sh","/bin/");
				copy_base_to_container(hpConfig,"Config.sh","/bin/");
			}
			else{
				bzero(buffer,strlen(buffer));
				sprintf(buffer,"%s/%s/Service.conf",node.image_path,hpConfig->vm_name);
				get_data_from_xmlfile(buffer,hpConfig->hp_name,"Config.sh",hpConfig->network_type);

				char *hp_file_name;
				hp_file_name = (char *)calloc(1,128);

				sprintf(hp_file_name,"../%s/%s.sh",hpConfig->hp_name,hpConfig->hp_name);
				copy_base_to_container(hpConfig,hp_file_name,"/bin/");
				sprintf(hp_file_name,"../%s/%s_start.sh",hpConfig->hp_name,hpConfig->hp_name);
				copy_base_to_container(hpConfig,hp_file_name,"/bin/");
				copy_base_to_container(hpConfig,"Config.sh","/bin/");
				bzero(hp_file_name,strlen(hp_file_name));
				free(hp_file_name);

			}
			hpConfig->start_time=(int)time(NULL);
			start_container(hpConfig);
			hpConfig->next_hp=hp_list;
			hp_list=hpConfig;

			//if(!strcmp(hpConfig->hp_name,"cdac_hp")){
			pthread_create(&threads[hp_id],NULL,data_read,(void*) hpConfig);
			pthread_detach(threads[hp_id]);
			//}
			printf("=================================================================\n");
		}
	}
	free(hpFile);
	free(buffer);
	free(cmd);
}
