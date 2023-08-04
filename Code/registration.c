
/***************************************************************************
* @file        registeration.c                                            *
* @author      Simranjeet Singh				           *
* @date        15/05/2015						   *
* @Edited By								   *
* S.No.  |   Name    |  Date      |  Line No  |  Comments		   *
* 1.	                            	 				   *
* 2.									   *
**************************************************************************/ 

#include "common.h"


/****************************************************************************
* Function :    register_node                                              *
* Param :                                                                  *
* Return :      int                                                        *
* Description: This function is used to register the node get the regisered*
*              node_id in response.                                        *
* Author :      Simranjeet Singh 					    *
* Editor :                                   				    *
* Date         15/05/2015                                                  *
*                                                                          *
****************************************************************************/
int register_node(struct Node *node)
{
	conf_files_st conf_st;
	printf("Registration Process...\n");

	conf_st.conf_name = (char *) calloc(1,50);
	conf_st.conf_path = (char *) calloc(1,255);
	conf_st.blueprint_name = (char *) calloc(1,50);
	conf_st.blueprint_path = (char *) calloc(1,255);

	strcpy(conf_st.conf_name,"config.conf");
	strcpy(conf_st.conf_path,"../node_confs/config.conf");
	strcpy(conf_st.blueprint_name,"blueprint.xml");
	strcpy(conf_st.blueprint_path,"../node_confs/blueprint.xml");

	node->node_id=-1;

	register_Node_api(&conf_st);
	if(node->node_id == -1)
		return 0;
#if 1
	free(conf_st.conf_name);
	free(conf_st.conf_path);
	free(conf_st.blueprint_name);
	free(conf_st.blueprint_path);
#endif
	return 1;
}

/****************************************************************************
* Function :    read_nodeid                                                *
* Param :                                                                  *
* Return :      int                                                        *
* Description: This function reads the client_reg file and returns back id *
* Author :      Simranjeet Singh                                           *
* Editor :                                                                 *
* Date         15/05/2015                                                  *
*                                                                          *
****************************************************************************/
int read_nodeid(struct Node *node)
{
	FILE *fp1;
	char nodeID[20];
	bzero(nodeID,strlen(nodeID));
	fp1=fopen("../config_files/client_reg","rb");
	//fread(nodeID,1,9,fp1);
	fgets(nodeID,20,fp1);
	fclose(fp1);

	int len = strlen(nodeID);
	if( nodeID[len-1] == '\n' )
		nodeID[len-1] = 0;


	if(strlen(nodeID)>0)
	{
		char cmd[50];
		sprintf(cmd,"mkdir ../logs/node_%s",nodeID);
		printf("\n %s \n",cmd);
		system(cmd);
		node->node_id = atoi(nodeID);
		return 1;
	}
	else
	{
		return -1;
	}
}


/****************************************************************************
* Function :    write_nodeid                                               *
* Param :                                                                  *
* Return :      int                                                        *
* Description: This function writes the node_id to client_reg file.        *
* Author :      Simranjeet Singh                                           *
* Editor :                                                                 *
* Date         15/05/2015                                                  *
*                                                                          *
****************************************************************************/
void write_nodeid(int node_id)
{
	FILE *fp;
	fp=fopen("../config_files/client_reg","w");
	if(fp != NULL){
		fprintf(fp,"%d",node_id);
		fclose(fp);
	}
	return;
}

