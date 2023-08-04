#include "common.h"

void* data_read(void* arg)
{
	struct Honeypot *hp = (struct Honeypot*)arg;
	char *str,*log_path;
	str      = (char*)calloc(1,256);
	log_path = (char*)calloc(1,128);


	if(!strcmp(hp->hp_name,"cdac_hp")){
		sprintf(str,"python ../Code/db.py /DockerShared/%s_%d/ '%s' %d '%s' '%s' '%s' '%s' &",hp->hp_name,hp->hp_id,node.server_ip,node.node_id,hp->hp_name, node.organization, node.organization_sector, node.organization_region);
		system(str);
	}

	sprintf(log_path,"/DockerShared/%s_%d/healthData.log",hp->hp_name,hp->hp_id);
	while(1){
		bzero(str,strlen(str));
		sprintf(str,"python ../Code/HealthDataToRest.py '%s' '%s'",node.server_ip,log_path);
		system(str);
		sleep(60);
	}

	bzero(str,strlen(str));
	bzero(log_path,strlen(log_path));

	free(str);
	free(log_path);
}

int data_collector(int config_flag){
	logger("Data Collector Module Started.","Data-Collection-Module");
	struct Honeypot *ptr = hp_list;
	while(ptr!=NULL){
		if(!strcmp(ptr->vm_type,"docker")){
			pthread_cancel(threads[ptr->hp_id]);
			data_collect_from_docker(ptr);
			stop_container(ptr);
			if(config_flag==1){
				rm_container(ptr);
			}
			else{
				ptr->start_time=(int)time(NULL);
				start_container(ptr);
				pthread_create(&threads[ptr->hp_id],NULL,data_read,(void*) ptr);
				pthread_detach(threads[ptr->hp_id]);
			}
		}
		else{
			data_collect_send(ptr);
			if(config_flag!=1){
				if(check_vm_status(ptr->vm_name))
					vm_power_off(ptr->vm_name);
				vm_snapshot_restore(ptr->vm_name, ptr->vm_snapshot);
				vm_power_on(ptr->vm_name);
			}
		}
		ptr=ptr->next_hp;
	}
	if(config_flag==1){
		hp_id=0;
		hp_list=NULL;
	}
	logger("Data Collector Module Endeed.","Data-Collection-Module");
	return 0;
}


int data_collect_from_docker(struct Honeypot *hpConfig){

	char *command,*end_time ,*doc_file,*data_path,*path;
	doc_file= (char*)calloc(1,256);
	end_time=(char*)calloc(1,64);
	command=(char*)calloc(1,512);
	data_path=(char*)calloc(1,512);
	int flag=0;

	sprintf(data_path,"/DockerShared/%s_%d/flag_close",hpConfig->hp_name,hpConfig->hp_id);

	/* /DockerShared/%s_%d/flag_close file is created
	* python program db.py will check the existance of this file if file exist db.py
	* program will delete the file and terminate itself.*/

	FILE *fp=fopen(data_path,"wb");
	fclose(fp);
	sleep(30);

	bzero(data_path,strlen(data_path));
	sprintf(data_path,"%s/%s/%s/Data_collection.xml",node.image_path,hpConfig->vm_name,hpConfig->hp_name);
	xmlDocPtr doc;
	xmlNodePtr cur;
	doc = xmlParseFile(data_path);
	if(doc == NULL){
		fprintf(stderr,"%s Configuration file not Found.\n",data_path);
		free(data_path);
		free(end_time);
		free(doc_file);
		free(command);
		xmlFreeDoc(doc);
		return 0;
	}

	/* creating the file structure to save the file*/
	/* code for creating the path for stroing the data */
	bzero(end_time,strlen(end_time));
	sprintf(end_time,"%d",(int)time(NULL));
	bzero(data_path,strlen(data_path));
	sprintf(data_path,"%s/%s/%s/DATA_CAPTURED/%d",node.image_path,hpConfig->vm_name,hpConfig->hp_name,node.node_id);
	/* check if the /imagePath/node_list/vm_name/snapshot/Data_CAPTURED/node_id/PHP/exists 
	if it dosenot exits create the dircetory     
	*/

	/* create the directory*/
	bzero(command,strlen(command));
	sprintf(command,"sudo mkdir %s/%s/%s/DATA_CAPTURED/",node.image_path,hpConfig->vm_name,hpConfig->hp_name);
	system(command);

	//sprintf(command,"sudo mkdir %s/%d %s/%d/PHP ",data_path,node_id,data_path,node_id);
	bzero(command,strlen(command));
	sprintf(command,"sudo mkdir %s/ %s/PHP ",data_path,data_path);
	system(command);

	// does not exist
	bzero(command,strlen(command));
	sprintf(command,"sudo mkdir %s/PHP/%d-%s-%s",data_path,hpConfig->start_time,end_time,hpConfig->ip);
	system(command);

	/* path created for data storage DATA_CAPTURE node_id/PHP/strat-end-ip*/
	bzero(doc_file,strlen(doc_file));
	sprintf(doc_file,"sudo mkdir %d_data %d_data/PHP/",node.node_id,node.node_id);
	system(doc_file);

	bzero(doc_file,strlen(doc_file));
	sprintf(doc_file,"sudo mkdir %d_data/PHP/%d-%s-%s",node.node_id,hpConfig->start_time,end_time,hpConfig->ip);
	system(doc_file);

	bzero(doc_file,strlen(doc_file));
	sprintf(doc_file,"%d_data/PHP/%d-%s-%s",node.node_id,hpConfig->start_time,end_time,hpConfig->ip);

	cur = xmlDocGetRootElement(doc);
	if(cur == NULL){
		fprintf(stderr,"empty document\n");
		free(data_path);
		free(end_time);
		free(doc_file);
		xmlFreeDoc(doc);
		return 0;
	}
	//DHS is root node element of honeypot conf XML file.
	if(xmlStrcmp(cur->name, (const xmlChar *) "DHS")){
		fprintf(stderr,"document of the wrong type, root node != DHS");
		free(data_path);
		free(end_time);
		free(doc_file);
		free(command);
		xmlFreeDoc(doc);
		return 0;
	}
	cur = cur->xmlChildrenNode;
	// this module gets the data to be sent in the forlser
	while(cur !=NULL){
		//retrieving individual honeypt details & configure the honeypot.
		if((!xmlStrcmp(cur->name,(const xmlChar *)"cp_1"))){
			path = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
			/* copy the data from container to the data directory*/
			copy_container_to_base(hpConfig, path, doc_file);
			sleep(6);
		}
		else if((!xmlStrcmp(cur->name,(const xmlChar *)"rm_1"))){
			// remove these files from docker to refresh the docker 
			// we remove data files which contains the data and are recreated when honeypot is restarted*/

			path = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
			rm_container_data(hpConfig, path);
		}		
		else if((!xmlStrcmp(cur->name,(const xmlChar *)"execute"))){
			path = xmlNodeListGetString(doc,cur->xmlChildrenNode,1);
			execute_container_command(hpConfig, path);
		}
		cur = cur->next;
	}

	// zip the folder in the doc_file
	// remove the folder 
	// send the zip data 
	// remove the zip folder

	bzero(command,strlen(command));
	bzero(data_path,strlen(data_path));
	sprintf(data_path,"%d_data/uconf_id",node.node_id);
	sprintf(command,"%d",hpConfig->uconf_id);

	FILE *fp_uconf = fopen(data_path,"w");
	if(fp_uconf!=NULL){
		fwrite(command,1,strlen(command),fp_uconf);
		fclose(fp_uconf);
	}

	bzero(data_path,strlen(data_path));
	sprintf(data_path,"%d_data.zip",node.node_id);

	/* move existing data.zip for verification of data*/
	if (access(data_path, F_OK) == 0) {
		bzero(command,strlen(command));
		sprintf(command,"sudo mv %s ../temp/",data_path);
		system(command);
		sleep(5);
	}

	/* Compress the collected data using zip */
	bzero(command,strlen(command));
	sprintf(command,"sudo zip -r %s %d_data;",data_path,node.node_id);	
	flag=system(command);

	sleep(10);

	if(flag==0)
	{
		/* send the compressed data */
		while(1)
		{
			if(send_compressed_data(data_path)==1)
			{
				/* remove collected data */
				bzero(command,strlen(command));
				sprintf(command,"sudo rm -rf %d_data",node.node_id);
				flag=system(command);
				
				/* Remove the zip file */
				/*bzero(command,strlen(command));
				sprintf(command,"sudo rm -rf %s",data_path);
				flag=system(command);
				sleep(5);*/

				break;
			}
			logger("Error during sending pcap data.","PCAP-Data-Module");
			printf("\nRetrying to send collected data.\n");
			sleep(5);
		}		
	}

	free(end_time);
	free(doc_file);
	free(data_path);
	free(command);
	xmlFree(doc);
	return 1;
}


int data_collect_send(struct Honeypot *hpConfig){
	int node_id;
	node_id=node.node_id;
	int flag=0;
	char *cmd,*file_path;

	cmd       = (char*)calloc(1,512);
	file_path = (char*)calloc(1,256);

	printf("######################################## Data Collection ###########################################\n");
	sprintf(cmd,"%s/%s/%s/Shared/Data/",node.image_path,hpConfig->os_type,hpConfig->vm_name);

	if(!strcmp(hpConfig->os_type,"Windows"))
	{
		sprintf(file_path,"%s/%s/%s/Shared/Data",node.image_path,hpConfig->os_type,hpConfig->vm_name);
		check_path(file_path);
		sprintf(file_path,"%s/%s/%s/Shared/Data/captureData.txt",node.image_path,hpConfig->os_type,hpConfig->vm_name);
		FILE *fp;
		fp = fopen(file_path,"w");
		fclose(fp);

		printf("\t\t Data collecting Windows.............\n");
		if(strcmp(hpConfig->hp_type,"AHP"))
		{
			int cvmf=0;
			while(1)
			{
				fp = fopen(file_path,"r");
				if (fp==NULL)
				{
					break;
				}
				else
				{
					fclose(fp);
				}
				if(cvmf==30){
					sprintf(cmd,"sudo rm -rf %s",file_path);
					system(cmd);
					break;
				}
				sleep(60);
				cvmf++;
			}
		}
		sprintf(cmd,"sudo rm -rf %s",file_path);
		system(cmd);

		bzero(file_path,strlen(file_path));
		sprintf(file_path,"%s/%s/%s/Shared/Data/%d/%s/",node.image_path,hpConfig->os_type,hpConfig->vm_name,node_id,hpConfig->hp_type);
		sprintf(cmd,"sudo mkdir %d_data",node_id);
		system(cmd);
		sprintf(cmd,"sudo cp -ruf %s %d_data/",file_path,node_id);
		flag = system(cmd);
		if(flag==0)
		{
			printf("\t\t Data copied successfully\n");
			sprintf(cmd,"sudo rm -rf %s",file_path);
			system(cmd);
		}
		sleep(5);
	}
	else if(!strcmp(hpConfig->os_type,"Linux"))
	{
		sprintf(file_path,"%s/%s/%s/Shared/Data/captureData.txt",node.image_path,hpConfig->os_type,hpConfig->vm_name);
		FILE *fp;
		fp = fopen(file_path,"w");
		fclose(fp);

		printf("\t\t Data collecting.............\n");
		if(strcmp(hpConfig->hp_type,"AHP"))
		{
			while(1)
			{
				fp = fopen(file_path,"r");
				if (fp==NULL)
				{
					break;
				}
				else
				{
					fclose(fp);
				}
			}
			bzero(file_path,strlen(file_path));
			sprintf(file_path,"%s/%s/%s/Shared/Data/%d/%s/",node.image_path,hpConfig->os_type,hpConfig->vm_name,node_id,hpConfig->hp_type);
		}
		else{
			bzero(file_path,strlen(file_path));
			sprintf(file_path,"%s/%s/%s/Shared/Data/AHP/",node.image_path,hpConfig->os_type,hpConfig->vm_name);
		}

		sprintf(cmd,"sudo mkdir %d_data",node_id);
		system(cmd);
		sprintf(cmd,"sudo cp -ruf %s %d_data/",file_path,node_id);
		flag = system(cmd);
		sleep(5);
		if(flag==0)
		{
			printf("\t\t Data copied successfully\n");
			sprintf(cmd,"sudo rm -rf %s",file_path);
			flag=system(cmd);	
		}
		sleep(5);
	}
	if(flag==0)
	{
		bzero(cmd,strlen(cmd));
		bzero(file_path,strlen(file_path));
		sprintf(file_path,"%d_data/uconf_id",node_id);
		sprintf(cmd,"%d",hpConfig->uconf_id);

		FILE *fp_uconf = fopen(file_path,"w");
		if(fp_uconf!=NULL){
			fwrite(cmd,1,strlen(cmd),fp_uconf);
			fclose(fp_uconf);
		}

		sprintf(file_path,"%d_data.zip",node_id);

		/* move existing data.zip for verification of data*/
		if (access(file_path, F_OK) == 0)
		{
			bzero(cmd,strlen(cmd));
			sprintf(cmd,"sudo mv %s ../temp/",file_path);
			system(cmd);
			sleep(5);
		}
		
		sprintf(cmd,"sudo zip -r %s %d_data/",file_path,node_id);
		flag=system(cmd);
		sleep(10);

		if(flag==0)
		{
			/* send compressed pcap Data */
			while(1)
			{
				if(send_compressed_data(file_path)==1)
				{
					/* remove Collected data */
					bzero(cmd,strlen(cmd));
					sprintf(cmd,"sudo rm -rf %d_data",node.node_id);
					flag=system(cmd);
					sleep(5);
					break;
				}
				logger("Error during sending pcap data.","PCAP-Data-Module");
				printf("\nRetrying to send Collected data.\n");
				sleep(5);
			}		
		}
	}
	free(file_path);
	free(cmd);
	printf("####################################################################################################\n");
	return 0;
}

int start_firewall()
{
	char *cmd;
	cmd = (char*)calloc(1,256);

	printf("\n Honeypot Type: %s\n",hp_type);
	if(hp_type!=NULL)
	{
		sprintf(cmd,"sudo ../data_control/dc.sh %s",hp_type);
		system(cmd);
	}
	system("/etc/rc.d/rc.FireWall stop");
	free(cmd);
}
