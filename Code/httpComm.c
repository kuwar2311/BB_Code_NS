
/***************************************************************************
* @file      : httpComm.c                                                  *	
* @author    : Simranjeet Singh         				   *
* @date      : 2023-01-23                                                  *
* @Edited By : Kuwar Singh                                                 *
* S.No.  |   Name    |  Date      |  Line No  |  Comments		   *
* 1.	                            	 				   *
* 2.									   *
**************************************************************************/ 

#include "common.h"
#include <curl/curl.h>
#include <curl/easy.h>


char *response_headers = NULL;
long curl_time_out_data = 14400;
long curl_time_out_conf = 300;


/****************************************************************************
* Function    : post_response                                               *
* Param       :                                                             *
* Return      : int                                                         *
* Description : This function is called as callback to read the response    *
*               received from server for every POST request sent by client  *
* Author      : Simranjeet Singh                                            *
* Editor      : Kuwar Singh                                                 *
* Date        : 2023-01-16                                                  *
****************************************************************************/
static size_t post_response(char *buffer, size_t size, size_t nitems, void *userdata)
{
	if(userdata != NULL)
	{
		char data[512] = "";
		memcpy(data,buffer,size*nitems);
		//printf("RESPONSE:\n%s\n",data);
	}
	return nitems * size;
}


/****************************************************************************
* Function    : write_data                                                  *
* Param       :                                                             *
* Return      : int                                                         *
* Description : This function is called as callback to read the response    *
*               received from server for every GET request sent by client.  *
* Author      : Simranjeet Singh                                            *
* Editor      : Kuwar Singh                                                 *
* Date        : 2023-01-16                                                  *
****************************************************************************/
static size_t write_data(char *buffer, size_t size, size_t nitems, void *userdata)
{
	if(userdata != NULL)
	{
		char data[512] = "";
		memcpy(data,buffer,size*nitems);
		printf("RESPONSE: %s\n",data);
		node.node_id = get_node_id_from_response(data);
		if(node.node_id != -1)
		{
			//printf("Node_id:%d\n",node_id);
			write_nodeid(node.node_id);
		}
		else{
			printf("\nFailed to register Node.\n");
			printf("%s\n",data);
		}
	}
	return nitems * size;
}


/****************************************************************************
* Function    : send_compressed_data                                        *
* Param       : filename                                                    *
* Return      : int                                                         *
* Description : This function  sends the compressed data file to server.    *
* Author      : Simranjeet Singh 					    *
* Editor      : Kuwar Singh                                                 *
* Date        : 2023-01-16                                                  *
****************************************************************************/
int send_compressed_data(char *filename)
{
	char *log;
	log=(char*)calloc(1, 128);
	bzero(log,strlen(log));
	sprintf(log,"%s File to send.", filename);
	logger(log,"send_compressed_data");

	printf("\n\t\t Data Sending start\n");
	CURL *curl;
	CURLcode res;

	struct curl_httppost *formpost=NULL;
	struct curl_httppost *lastptr=NULL;
	struct curl_slist *headerlist=NULL;
	static const char buf[] = "Expect:";

	curl_global_init(CURL_GLOBAL_ALL);


	/* Fill in the file upload field */ 
	curl_formadd(&formpost, &lastptr,CURLFORM_COPYNAME, "file1", CURLFORM_FILE, filename,CURLFORM_END);

	curl = curl_easy_init();
	/* initalize custom header list (stating that Expect: 100-continue is not   wanted */ 
	headerlist = curl_slist_append(headerlist, buf);
	if(curl) 
	{
		char serv_add[250] = "\0";
		sprintf(serv_add,"https://%s:8443/DATA",node.server_ip);

		/* what URL that receives this POST */ 
		curl_easy_setopt(curl, CURLOPT_URL, serv_add);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, curl_time_out_data);

		/* SSL Options */
		//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS , 1);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER , 1);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST , 1);
		curl_easy_setopt(curl, CURLOPT_CAINFO, "../keys/ca.crt");
		curl_easy_setopt(curl, CURLOPT_CAPATH, "../keys/ca.crt");

		/* only disable 100-continue header if explicitly requested */ 
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, post_response);
		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);
		/* Check for errors */ 
		if(res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));				
			bzero(log,strlen(log));
			sprintf(log,"%s Error during '%s' send.", curl_easy_strerror(res), filename);
			logger(log,"send_compressed_data");
			bzero(log,strlen(log));
			free(log);
			return 0;
		}

		/* cleanup curl handle*/ 
		curl_easy_cleanup(curl);

		/* then cleanup the formpost chain */ 
		curl_formfree(formpost);
		/* free slist */ 
		curl_slist_free_all (headerlist);
	}
	
	bzero(log,strlen(log));
	sprintf(log,"'%s' successfully sent.", filename);
	logger(log,"send_compressed_data");
	printf("\n\t\t Data tranferred.\n");
	
	bzero(log,strlen(log));
        free(log);
	
	return 1;
}


/****************************************************************************
* Function    : register_Node_api                                           *
* Param       : conf_files_st (names of config files)                       *
* Return      : int                                                         *
* Description : This function  prepares the register request & send a POST  *
*               request to server along with the xml config file data.      *
* Author      : Simranjeet Singh                                            *
* Editor      : Kuwar Singh                                  		    *
* Date        : 2023-01-16                                                  *
****************************************************************************/
int register_Node_api(conf_files_st *p_conf_st)
{
	char *log;
        log=(char*)calloc(1, 128);
        bzero(log,strlen(log));
        sprintf(log,"Node Registration.");
        logger(log,"send_compressed_data");

	CURL *curl;
	CURLcode res;


	struct curl_httppost *formpost=NULL;
	struct curl_httppost *lastptr=NULL;
	struct curl_slist *headerlist=NULL;
	static const char buf[] = "Expect:";

	curl_global_init(CURL_GLOBAL_ALL);

	/* Fill in the file upload field */ 
	curl_formadd(&formpost, &lastptr,CURLFORM_COPYNAME, "file1", CURLFORM_FILE, p_conf_st->conf_path,CURLFORM_END);
	curl_formadd(&formpost, &lastptr,CURLFORM_COPYNAME, "FILES_IN_QUEUE", CURLFORM_COPYCONTENTS, "send", CURLFORM_END);
	curl_formadd(&formpost, &lastptr,CURLFORM_COPYNAME, "file2", CURLFORM_FILE, p_conf_st->blueprint_path,CURLFORM_END);

	curl = curl_easy_init();
	
	/* initalize custom header list (stating that Expect: 100-continue is not  wanted */
	headerlist = curl_slist_append(headerlist, buf);
	if(curl) 
	{
		char serv_add[250] = "\0";
		sprintf(serv_add,"https://%s:8443/REGISTER",node.server_ip);
		/* what URL that receives this POST */ 
		curl_easy_setopt(curl, CURLOPT_URL, serv_add);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, curl_time_out_conf);

		/* SSL Options */
		//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS , 1);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER , 1);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST , 1);
		curl_easy_setopt(curl, CURLOPT_CAINFO, "../keys/ca.crt");
		curl_easy_setopt(curl, CURLOPT_CAPATH, "../keys/ca.crt");

		/* only disable 100-continue header if explicitly requested */ 
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);
		/* Check for errors */ 
		if(res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
			
			bzero(log,strlen(log));
                        sprintf(log,"'%s' Error during Node Registration.", curl_easy_strerror(res));
                        logger(log,"send_compressed_data");
                        
			bzero(log,strlen(log));
                        free(log);
			
			return 0;
		}

		/* cleanup curl handle*/ 
		curl_easy_cleanup(curl);

		/* then cleanup the formpost chain */ 
		curl_formfree(formpost);
		/* free slist */ 
		curl_slist_free_all (headerlist);
	}
	
	bzero(log,strlen(log));
	sprintf(log,"Node Registered. ");
	logger(log,"send_compressed_data");

	bzero(log,strlen(log));
	free(log);

	return 1;
}


/****************************************************************************
* Function :    write_data_to_file                                         *
* Param :                                                                  *
* Return :      int                                                        *
* Description: This function is iteratively called as callback to write    *
*               data,received from server, to file on GET request sent by  *
*                 client.                                                  *
* Author :      Simranjeet Singh 					    *
* Editor :                                   							        *
* Date         15/05/2015                                                  *
*                                                                          *
****************************************************************************/
size_t write_data_to_file(void *ptr, size_t size, size_t nmemb, FILE *stream) 
{
	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	return written;
}


/****************************************************************************
* Function :    dl_progress                                                *
* Param :                                                                  *
* Return :      int                                                        *
* Description: This function display the progress of download  any file.   *
* Author :      Simranjeet Singh 											    *
* Editor :                                   							        *
* Date         15/05/2015                                                  *
*                                                                          *
****************************************************************************/
static int dl_progress(void *clientp,double dltotal,double dlnow,double ultotal,double ulnow)
{
	if (dlnow && dltotal)
		printf("dl:%3.0f%%\r",100*dlnow/dltotal);
		fflush(stdout);    
	return 0;
}



/****************************************************************************
* Function    : get_file_from_server                                        *
* Param       : char *file_path, char *request_type                         *
* Return      : int                                                         *
* Description : This function  prepares the GET request to get the honeypot *
*               configuration file from server                              *
* Author      : Simranjeet Singh                                            *
* Editor      : Kuwar Singh                                                 *
* Date        : 2023-01-16                                                  *
****************************************************************************/
long get_file_from_server(char *file_path,char *request_type) 
{
	CURL *curl;
	FILE *fp;
	CURLcode res;
	double dl_size;
	char url[250] = "\0";
	long resp_code;

	if(!strcmp(request_type,"GetHoneypotConf"))
		sprintf(url,"https://%s:8443/%d/honeypot.conf",node.server_ip,node.node_id);
	else if(!strcmp(request_type,"GetAHPUrl"))
		sprintf(url,"https://%s:8443/%d/GetAHPUrl",node.server_ip,node.node_id);
	else if(!strcmp(request_type,"GetLAHPUrl"))
		sprintf(url,"https://%s:8443/%d/GetLAHPUrl",node.server_ip,node.node_id);


	char *outfilename  = file_path;
	curl = curl_easy_init();
	if (curl) 
	{
		fp = fopen(outfilename,"wb");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_to_file);
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, dl_progress);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, curl_time_out_conf);

		/* SSL Options */
		//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS , 1);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER , 1);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST , 1);
		curl_easy_setopt(curl, CURLOPT_CAINFO, "../keys/ca.crt");
		curl_easy_setopt(curl, CURLOPT_CAPATH, "../keys/ca.crt");

		res = curl_easy_perform(curl);

		curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&resp_code);
		if(resp_code == 404||resp_code == 0)
			remove(outfilename);
		/* Check for errors */ 
		if(res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			return 0;
		}


		curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &dl_size);
		//printf("CURLINFO_CONTENT_LENGTH_DOWNLOAD:%f\n", dl_size);
		/* always cleanup */ 
		curl_easy_cleanup(curl);
		fclose(fp);
	}
	return resp_code;
}



/****************************************************************************
* Function    : get_conf_file                                               *
* Param       :                                                             *
* Return      : int                                                         *
* Description : This function  prepares the GET request to get the honeypot *
*               configuration file from server                              *
* Author      : Simranjeet Singh 			                    *
* Editor      : Kuwar Singh                                                 *
* Date        : 2023-01-16                                                   *
****************************************************************************/
int get_conf_file() 
{
	printf("\n#############################################################################\n");
	//printf("\tConfiugration File Request.\n");
	int return_code;
	char *log;
	log=(char*)calloc(1,128);
	bzero(log,strlen(log));
	sprintf(log,"Configuration file Request to server.");
	logger("-----------------------------------","--------------------------------------------------");
	logger(log,"Configuration-Module");
	
	long resp_code = get_file_from_server("../config_files/honeypot.conf","GetHoneypotConf");
	printf("Response code From server: %ld\n",resp_code);	
	
	printf("#############################################################################\n");
	
	bzero(log,strlen(log));

	if(resp_code == 404)
	{
		printf("\n\t\t No new configuration at server side\n");
		sprintf(log,"Info: No new configuration at server side, Response Code: %ld\n",resp_code);
		return_code = 0;
	}
	else if(resp_code == 0)
	{
		printf("\n\t\t HTTP Connection error: response code 0 is caused by an unreachable server.\n");
		sprintf(log,"Error: Due to an unreachable server, Response Code: %ld\n",resp_code);
		return_code = 2;
	}	
	else if(resp_code == 503)
	{
		printf("\n\t\t Connection Error. due to Service Unavailable\n");
		sprintf(log,"Error: Due to Service Unavailable, Response Code: %ld\n",resp_code);
		return_code = 2;
	}	
	else if(resp_code == 504)
	{
		printf("\n\t\t Connection Error due to Gateway Timeout.\n");
		sprintf(log,"Error: Due to Gateway Timeout., Response Code: %ld\n",resp_code);
		return_code = 2;
	}
	else if(resp_code == 200) {
		FILE *fp;
		fp = fopen("../config_files/honeypot.conf","r");

		if(fp!=NULL)
		{
			int size;
			fseek(fp,0,SEEK_END);
			size = ftell(fp);
			fclose(fp);
			if(size<=0)
			{
				printf("\t\t Empty configuration file received.\n");
				sprintf(log,"Error: Empty configuration file received., Response Code: %ld\n",resp_code);
				return_code = 2;
			}
			sprintf(log,"Info: New Configuration file received., Response Code: %ld\n",resp_code);
			return_code = 1;
		}
		else
		{
			printf("\t\t Configuration file does not exist.\n ");
			sprintf(log,"Error: Configuration file does not exist., Response Code: %ld\n",resp_code);
			return_code = 2;
		}

	}	
	else
	{
		printf("\n\t\t HTTP Connection Error.\n");
		sprintf(log,"Error: Unknown Error., Response Code: %ld\n",resp_code);
		return_code = 2;
	}
	logger(log,"Configuration-Module");
	bzero(log,strlen(log));
	free(log);
	return return_code;
}




/****************************************************************************
* Function :    get_conf_file                                              *
* Param :                                                                  *
* Return :      int                                                        *
* Description: This function  prepares the GET request to get the honeypot *
*                 configuration file from server                           *
* Author :      Simranjeet Singh 											    *
* Editor :                                   							        *
* Date         15/06/2015                                                  *
*                                                                          *
****************************************************************************/
int get_ahp_url() 
{
	printf("\n#####################################################################################################\n");
	printf("\t\t URL Request for High Interaction Active Honeypot.\n");	
	long resp_code = get_file_from_server("../url/URLDetails.xml","GetAHPUrl");
	printf("\t\t Response Code:%ld\n",resp_code);
	printf("####################################################################################################\n");
	if(resp_code == 404)
	{
		printf("\n\t\t No new URL left to be processed\n");
		return 0;
	}
	else if(resp_code == 0)
	{
		printf("\n\t\t Connection Error\n");
		return 2;
	}
	return 1;
}

/****************************************************************************
* Function :    get_conf_file                                              *
* Param :                                                                  *
* Return :      int                                                        *
* Description: This function  prepares the GET request to get the honeypot *
*                 configuration file from server                           *
* Author :      Simranjeet Singh 											    *
* Editor :                                   							        *
* Date         15/06/2015                                                  *
*                                                                          *
****************************************************************************/
int get_lahp_url() 
{
	printf("#####################################################################################################\n");
	printf("\t\t URL Request for Low Interaction Active Honeypot (Thug).\n");	
	long resp_code = get_file_from_server("../url/URLDetails.xml","GetLAHPUrl");
	printf("\t\t Response Code:%ld\n",resp_code);
	printf("#####################################################################################################\n");
	if(resp_code == 404)
	{
		printf("No new URL left to be processed\n");
		return 0;
	}
	else if(resp_code == 0)
	{
		printf("\n\t\t Connection Error.");
		return 2;
	}
	return 1;
}
