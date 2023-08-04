#include "common.h"
#include<dirent.h>

int stop_tcpdump(){
	if(system("sudo pidof tcpdump > tcpdump.log")==-1){
		printf("Error generated while executing command.\n");
		return 0;
	}
	else{
		long size=-1;
		FILE *fp = fopen("tcpdump.log","r");
		if(fp != NULL){
			fseek(fp,0,SEEK_END);
			size = ftell(fp);
			fclose(fp);
		}

		if(size>0){
			system("sudo kill -9 $(pidof tcpdump)");
		}
	}
	return 1;
}

int start_pcap_capturing(){
	printf("\n=================== N/W Data Capturing =======================\n");
	char *cmd,*pcap_path;
	cmd = (char*)calloc(1,256);
	pcap_path = (char*)calloc(1,70);
	struct Honeypot *ptr = hp_list;

	stop_tcpdump();

	bzero(cmd,strlen(cmd));
	bzero(pcap_path,strlen(pcap_path));
	while(ptr!=NULL){
		create_path(pcap_path,ptr->hp_name,ptr->hp_id);
		check_path(pcap_path);
		sprintf(cmd,"sudo tcpdump host %s and not %s -C 30 -n -s 0 -Z root -w %s/log -U > /dev/null 2>&1 &",ptr->ip,node.server_ip,pcap_path);
		printf("Command: %s\n",cmd);
		system(cmd);		
		ptr=ptr->next_hp;
	}
	free(cmd);
	free(pcap_path);
	printf("\n==============================================================\n");
}

int create_path(char *path,char *hp_name,int hp_id){
	time_t t =time(NULL);
	struct tm tm = *localtime(&t);
	//printf("Time: %d:%d\n",tm.tm_hour,tm.tm_min);
	bzero(path,strlen(path));
	sprintf(path,"%d_pcap/pcap/%d",node.node_id,tm.tm_year+1900);
	if((tm.tm_mon+1)<10)
		sprintf(path,"%s/0%d",path,tm.tm_mon+1);
	else
		sprintf(path,"%s/%d",path,tm.tm_mon+1);

	if(tm.tm_mday < 10)
		sprintf(path,"%s/0%d",path,tm.tm_mday);
	else
		sprintf(path,"%s/%d",path,tm.tm_mday);

	if(tm.tm_hour < 10)
		sprintf(path,"%s/0%d",path,tm.tm_hour);
	else
		sprintf(path,"%s/%d",path,tm.tm_hour);

	if(tm.tm_min < 10)
		sprintf(path,"%s.0%d",path,tm.tm_min);
	else
		sprintf(path,"%s.%d",path,tm.tm_min);

	sprintf(path,"%s/%s_%d",path,hp_name,hp_id);
	printf("path: %s\n",path);
}

int check_path(char *path){
	DIR * dir = opendir(path);
	if(!dir){
		char *found,*temp;
		found = strrchr(path,'/');
		if(found){
			size_t len = found -path;
			temp=malloc(len+1);
			memcpy(temp,path,len);
			temp[len]='\0';
			check_path(temp);
			free(temp);
		}
		struct stat st ={0};
		if(stat(path,&st) == -1)
			mkdir(path,0700);
	}
	else{
		closedir(dir);
	}
}

int pcap_data_collection()
{
	logger("Pcap Data Collection Started.","PCAP-Data-Module");
	char *cmd,*file_path;
	file_path = (char *)calloc(1,256);
	cmd = (char *)calloc(1,256);
	sprintf(file_path,"%d_pcap.zip",node.node_id);
	int flag=1;

	/* move existing pcap.zip for verification of data*/
	if (access(file_path, F_OK) == 0) {
		bzero(cmd,strlen(cmd));
		sprintf(cmd,"sudo mv %s ../temp/",file_path);
		system(cmd);
		sleep(5);
	}

	/*Compress pcap data*/
	bzero(cmd,strlen(cmd));
	sprintf(cmd,"sudo zip -r %s %d_pcap/",file_path,node.node_id);
	flag=system(cmd);
	sleep(10);
	
	if(flag==0)
	{
		/* send compressed pcap Data */
		while(1)
		{
			if(send_compressed_data(file_path)==1)
			{
				/* remove pcap data */
				bzero(cmd,strlen(cmd));
				sprintf(cmd,"sudo rm -rf %d_pcap",node.node_id);
				flag=system(cmd);
				
				/* remove pcap zip file */
				/*bzero(cmd,strlen(cmd));		
				sprintf(cmd,"sudo rm -rf %s",file_path);
				system(cmd);*/
				sleep(5);
				break;
			}
			logger("Error during sending pcap data.","PCAP-Data-Module");
			printf("\nRetrying to send pcap data.\n");
			sleep(5);
		}		
	}
	free(cmd);
	free(file_path);
	start_pcap_capturing();
	logger("Pcap Data Collection Ended.","PCAP-Data-Module");
	return 0;
}

void logger(char *log,char *module)
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char *sentence,*file_name;
	sentence=(char*)calloc(1,500);
	file_name=(char*)calloc(1,50);
	bzero(sentence,strlen(sentence));
	bzero(file_name,strlen(file_name));
	sprintf(file_name,"../logs/%d-%02d-%02d.log", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	sprintf(sentence,"\n%d-%02d-%02d %02d:%02d:%02d, %s, %s",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, module,log);
	FILE *fp = fopen(file_name,"a+");
	if(fp != NULL){
		fprintf(fp, "%s", sentence);
		fclose(fp);
	}
	bzero(sentence,strlen(sentence));
	free(sentence);
	free(file_name);
}

