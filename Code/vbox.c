#include "common.h"

/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: Function to run the virtual machine honeypot
----------------------------------------------------------------------------*/

int vm_power_on(char *vm_name){

	int sys_status;
	char *cmd;
	cmd = (char*)calloc(1,128);

	sleep(5);
	sprintf(cmd, "VBoxManage startvm %s",vm_name);
	sys_status=system(cmd);
	if(sys_status==-1)
	{
		free(cmd);
		printf("Failed to power on the vm");
		sleep(300);
		return 1;
	}
	else
	{
		printf("\t\tVirtual Machine started..........\n");
	}

	return 0;
}

/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: Function to shutdown the virtual machine honeypot
----------------------------------------------------------------------------*/
int vm_power_off(char *vm_name){
	char *cmd;
	cmd = (char*)calloc(1,128);

	sprintf(cmd, "VBoxManage controlvm %s poweroff",vm_name);
	system(cmd);
	printf("\t\tVirtual Machine stopped.\n");

	free(cmd);
	return 0;
}

/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: Function to check the virutal machine honeypot
is exist or not
----------------------------------------------------------------------------*/
int check_vm(char *vm_name)
{
	FILE *fp;
	char *cmd;
	int sys_status;

	cmd	= (char*)calloc(1,128);

	sprintf(cmd,"VBoxManage list vms | grep '%s' | awk '{print $1}'> reg_machine",vm_name);		
	sys_status = system(cmd);
	if(sys_status == -1)
	{
		printf("Error Found in command execution.\n");
		free(cmd);
		return 1;
	}

	fp=fopen("reg_machine", "rb");
	bzero(cmd,strlen(cmd));

	if(fp!=NULL)
	{
		while(fgets(cmd,128,fp))
		{
			int i,j;
			i=strlen(cmd)-3;
			j=strlen(vm_name);

			if(i==j)
			{
				free(cmd);
				return 1;
			}
		}
		fclose(fp);
	}
	free(cmd);
	return 0;
}

/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: Function to check the virutal machine honeypot
is running or not
----------------------------------------------------------------------------*/
int check_vm_status(char *vm_name)
{
	FILE *fp;
	char *cmd;
	int sys_status;

	cmd	= (char*)calloc(1,128);

	sprintf(cmd,"VBoxManage list runningvms | grep '%s' | awk '{print $1}'> reg_machine",vm_name);		
	sys_status = system(cmd);
	if(sys_status == -1)
	{
		printf("Error Found in command execution.\n");
		free(cmd);
		return 1;
	}

	fp=fopen("reg_machine", "rb");
	bzero(cmd,strlen(cmd));

	if(fp!=NULL)
	{
		while(fgets(cmd,128,fp))
		{
			int i,j;
			i=strlen(cmd)-3;
			j=strlen(vm_name);

			if(i==j)
			{
				free(cmd);
				return 1;
			}
		}
		fclose(fp);
	}
	free(cmd);
	return 0;
}

/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: Function to check the virutal machine honeypot
is running or not
----------------------------------------------------------------------------*/
int check_vbox_status()
{
	FILE *fp;
	char *cmd;
	int sys_status;

	cmd	= (char*)calloc(1,128);

	sprintf(cmd,"VBoxManage list runningvms | awk '{print $1}'> reg_machine");		
	sys_status = system(cmd);
	if(sys_status == -1)
	{
		printf("Error Found in command execution.\n");
		free(cmd);
		return 1;
	}


	fp=fopen("reg_machine", "rb");
	bzero(cmd,strlen(cmd));

	if(fp!=NULL)
	{	while(fgets(cmd,128,fp))
	{
		int i,j;
		i=strlen(cmd)-3;

		if(i>1)
		{
			free(cmd);
			return 1;
		}
	}
	fclose(fp);
	}
	free(cmd);
	return 0;
}

/*----------------------------------------------------------------------------
Written By: Kuwar Singh
Description: Function to restore the virtual machine honeypot
to fresh state
----------------------------------------------------------------------------*/
int vm_snapshot_restore(char *vm_name, char *vm_snapshot){
	char *cmd;
	cmd = (char*)calloc(1,128);

	sleep(5);
	sprintf(cmd, "VBoxManage snapshot  %s restore %s",vm_name,vm_snapshot);
	system(cmd);
	sleep(5);
	printf("\t\tSnapshot Reverted\n");

	free(cmd);
	return 0;
}

