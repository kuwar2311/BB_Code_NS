#include "common.h"

/*************************************************************************
* Author      :  Kuwar Singh
* Date        :  2018-02-14
* Description :  Function to create a docker container
**************************************************************************/
int create_container(struct Honeypot *hp){
	int flag =0;
	char *cmd;
	cmd = (char*)calloc(1,512);
	stop_container(hp);
	rm_container(hp);

	sprintf(cmd,"sudo rm -rf /DockerShared/%s_%d",hp->hp_name,hp->hp_id);
	system(cmd);

	flag = check_docker_image_status(hp->vm_name);
	if(flag == 0){
		if(hp->hp_repo_path != NULL){
			printf("Repo Path: '%s'\n",hp->hp_repo_path);
			flag = pull_docker_image(hp->hp_repo_path,hp->vm_name);
		}
		else
			printf("Docker Repository does not found");
	}

	if(flag==1){
		if(!strcmp(hp->network_type,"DHCP")){
			sprintf(cmd,"sudo docker create -it --name %s_%d --privileged --cap-add NET_ADMIN %s bash /bin/Config.sh",hp->hp_name,hp->hp_id, hp->vm_name);
		}
		else if(!strcmp(hp->network_type,"host")){
			sprintf(cmd,"sudo docker create -it -v /DockerShared/%s_%d:/var/log/ --name %s_%d -p 80:80 --cap-add NET_ADMIN %s bash /bin/Config.sh",hp->hp_name,hp->hp_id,hp->hp_name,hp->hp_id, hp->vm_name);
		}
		else{
			sprintf(cmd,"sudo docker create -it -v /DockerShared/%s_%d:/var/log/ --name %s_%d --ip %s --dns %s  --net my_bridge --cap-add NET_ADMIN %s bash /bin/Config.sh",hp->hp_name,hp->hp_id,hp->hp_name,hp->hp_id, hp->ip,hp->dns, hp->vm_name);
			//sprintf(cmd,"sudo docker create -it -v /DockerShared/%s_%d:/var/log/ --name %s_%d --ip %s --dns %s  --net my_bridge --cap-add NET_ADMIN %s bash",hp->hp_name,hp->hp_id,hp->hp_name,hp->hp_id, hp->ip,hp->dns, hp->vm_name);
		}
		//printf("docker create: %s\n",cmd);
		if(system(cmd)==-1){
			printf("\n Faliure in creating docker container\n");
			free(cmd);
			return(0);
		}
		else
			printf("Container Created\n");
		free(cmd);
		return(1);
	}
	else{
		printf("Image Does not Found.");
		return(0);
	}
}


/*************************************************************************
* Author      :  Kuwar Singh
* Date        :  2018-02-14
* Description :  Function to copy data from base to container.
**************************************************************************/
int copy_base_to_container(struct Honeypot *hp, char *src, char *dest){
	char *cmd;
	cmd = (char*)calloc(1,512);
	sprintf(cmd,"sudo docker cp %s %s_%d:%s",src,hp->hp_name,hp->hp_id,dest);
	//printf("docker copy: %s\n",cmd);
	if(system(cmd)==-1){
		printf("Faliure in copying to docker container \n");
		free(cmd);
		return(0);
	}
	free(cmd);
	return(1);
}


/*************************************************************************
* Author      :  Kuwar Singh
* Date        :  2018-02-14
* Description :  Function to copy data from container to base.
**************************************************************************/
int copy_container_to_base(struct Honeypot *hp, char *src, char *dest){
	char *cmd;
	cmd = (char*)calloc(1,512);
	sprintf(cmd,"sudo docker cp %s_%d:%s %s",hp->hp_name,hp->hp_id,src,dest);
	//        printf("docker copy:%s\n",cmd);
	if(system(cmd)==-1){
		printf("Faliure in copying from docker container\n");
		free(cmd);
		return(0);
	}
	free(cmd);
	return(1);
}


/*************************************************************************
* Author      :  Kuwar Singh
* Date        :  2018-02-15
* Description :  Function to start container.
**************************************************************************/
int start_container(struct Honeypot *hp){
	char *cmd;
	cmd = (char*)calloc(1,512);
	sprintf(cmd,"sudo docker start %s_%d ",hp->hp_name,hp->hp_id);
	//printf("Start Docker:%s\n",cmd);

	if(system(cmd)==-1){
		printf("Faliure in starting the container\n");
		free(cmd);
		return(0);
	}
	else
		printf("docker container %s_%d started\n",hp->hp_name,hp->hp_id);
	free(cmd);
	return(1);
}

/*************************************************************************
* Author      :  Kuwar Singh
* Date        :  2018-02-15
* Description :  Function to check container.
**************************************************************************/
int check_existance_of_container(struct Honeypot *hp){
	char *cmd;
	cmd = (char*)calloc(1,512);
	sprintf(cmd,"sudo docker ps -a | grep '%s_%d' > container.log",hp->hp_name,hp->hp_id);
	//      printf("Start Docker:%s\n",cmd);

	if(system(cmd)==-1){
		printf("Error generated while executing command:%s\n",cmd);
		free(cmd);
		return 0;
	}
	else{
		free(cmd);
		long size=-1;
		FILE *fp = fopen("container.log","r");
		if(fp != NULL){
			fseek(fp,0,SEEK_END);
			size = ftell(fp);
			fclose(fp);
		}

		if(size<=0){
			//printf("Container does not exist.\n");
			return 2;
		}
	}

	return 1;
}

/*************************************************************************
* Author      :  Kuwar Singh
* Date        :  2018-02-15
* Description :  Function to stop container.
**************************************************************************/
int stop_container(struct Honeypot *hp){
	int i;
	i = check_existance_of_container(hp);

	if(i==2)
		return 0;

	char *cmd;
	cmd = (char*)calloc(1,512);

	sprintf(cmd,"sudo docker stop %s_%d ",hp->hp_name,hp->hp_id);
	//      printf("Start Docker:%s\n",cmd);

	if(system(cmd)==-1){
		printf("Error generated while executing command:%s\n",cmd);
		free(cmd);
		return 0;
	}
	else
		printf("docker container %s_%d stoped\n",hp->hp_name,hp->hp_id);
	free(cmd);
	return 1;
}

/*************************************************************************
* Author      :  Kuwar Singh
* Date        :  2018-02-15
* Description :  Function to remove container.
**************************************************************************/
int rm_container(struct Honeypot *hp){
	int i;
	i = stop_container(hp);

	if(i!=1)
		return 0;
	char *cmd;
	cmd = (char*)calloc(1,512);
	sprintf(cmd,"sudo docker rm %s_%d ",hp->hp_name,hp->hp_id);
	//      printf("Start Docker:%s\n",cmd);

	if(system(cmd)==-1){
		printf("Error generated while executing command:%s\n",cmd);
		free(cmd);
		return(0);
	}
	else
		printf("docker container %s_%d removed\n",hp->hp_name,hp->hp_id);			
	free(cmd);
	return(1);
}

/*************************************************************************
* Author      :  Kuwar Singh
* Date        :  2018-02-15
* Description :  Function to remove container files.
**************************************************************************/
int rm_container_data(struct Honeypot *hp,char *path){
	char *cmd;
	cmd = (char*)calloc(1,512);
	sprintf(cmd,"sudo docker rm %s_%d:%s",hp->hp_name,hp->hp_id,path);
	//      printf("Start Docker:%s\n",cmd);

	if(system(cmd)==-1){
		printf("Error generated while executing command:%s\n",cmd);
		free(cmd);
		return(0);
	}
	free(cmd);
	return(1);
}


/*************************************************************************
* Author      :  Kuwar Singh
* Date        :  2018-02-15
* Description :  Function to execute command inside container.
**************************************************************************/
int execute_container_command(struct Honeypot *hp, char *command){
	char *cmd;
	cmd = (char*)calloc(1,512);
	sprintf(cmd,"sudo docker exec %s_%d bash %s",hp->hp_name,hp->hp_id,command);
	//      printf("Start Docker:%s\n",cmd);

	if(system(cmd)==-1){
		printf("Error generated while executing command:%s\n",cmd);
		free(cmd);
		return(0);
	}
	free(cmd);
	return(1);
}

/*************************************************************************
* Author      :  Kuwar Singh
* Date        :  2022-06-03
* Description :  Function to Check docker image existance in locka repo.
**************************************************************************/
int check_docker_image_status(char *image_id){
	int flag =0;
	char *cmd,*cid;
	cid = (char*)calloc(1,128);
	cmd = (char*)calloc(1,512);
	sprintf(cmd,"sudo docker image inspect '%s' > /dev/null 2>&1 && echo 1 > image_status.log || echo 0 >image_status.log",image_id);

	if(system(cmd)==-1){
		printf("Error generated while executing command:%s\n",cmd);
		flag=0;
	}
	else{		
		FILE *fp = fopen("image_status.log","r");
		if(fp != NULL){
			fgets(cid,50,fp);
			fclose(fp);
			size_t ln = strlen(cid)-1;
			if(*cid && cid[ln]=='\n')
				cid[ln]='\0';
			flag = atoi(cid);
		}
	}
	if(flag == 0)
		printf("'%s' Image does not found..\n",image_id);
	bzero(cmd,strlen(cmd));
	bzero(cid,strlen(cid));
	free(cid);
	free(cmd);
	return flag;
}


/*************************************************************************
* Author      :  Kuwar Singh
* Date        :  2022-06-03
* Description :  Function to Pull docker image from docker registry.
**************************************************************************/
int pull_docker_image(char *image,char *image_id){
	int status =0;
	char *cmd;
	cmd = (char*)calloc(1,512);
	sprintf(cmd,"sudo docker pull '%s'",image);

	if(system(cmd)==-1){
		printf("Error generated while executing command:%s\n",cmd);
		status=0;
	}
	else{		
		status = check_docker_image_status(image_id);
		if(status == 0)
			printf("Failed to Pull '%s' Image from repository..\n",image);
		else
			printf("'%s' Image Pulled sucessfully...\n",image);
	}
	free(cmd);
	return status;
}

int remove_all_containers(){
	char *cmd,*cid;
	cid = (char*)calloc(1,128);
	cmd = (char*)calloc(1,128);
	logger("Removing All Containers.","Docker-Module");
	sprintf(cmd,"sudo docker ps -aq > container.log");
	if(system(cmd)==-1){
		printf("Error generated while executing command:%s\n",cmd);
		free(cmd);
		free(cid);
		return 0;
	}
	else{
		FILE *fp = fopen("container.log","r");
		if(fp != NULL){
			while(fgets(cid,50,fp)){
				size_t ln = strlen(cid)-1;
				if(*cid && cid[ln]=='\n')
					cid[ln]='\0';
				bzero(cmd,strlen(cmd));
				sprintf(cmd,"sudo docker stop '%s'",cid);
				system(cmd);
				bzero(cmd,strlen(cmd));
				sprintf(cmd,"sudo docker rm '%s'",cid);
				system(cmd);
			}
			fclose(fp);
		}
	}
	free(cmd);
	free(cid);
}
