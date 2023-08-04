/***************************************************************************
* @file        xmlReader.c							         				    *	
* @author      Simranjeet Singh							 						*
* @date        28/05/2015								 						*
* @Edited By										 								*
* S.No.  |   Name    |  Date      |  Line No  |  Comments				 	*
* 1.	                            	 						 					*
* 2.											 									*
**************************************************************************/ 
#include "common.h"

void die(char *msg)
{
	printf("%s", msg);
	return;
}

int get_node_id_from_response(char *data)
{
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseMemory(data,strlen(data));
	if (doc == NULL )
		die("Document parsing failed. \n");

	//Gets the root element of the XML Doc
	cur = xmlDocGetRootElement(doc); 
	if (cur == NULL)
	{
		xmlFreeDoc(doc);
		die("Document is Empty!!!\n");
	}


	while (cur != NULL)
	{
		if ((!xmlStrcmp(cur->name, "html")))
		{
			xmlNodePtr cur_temp = cur;
			cur_temp = cur_temp->xmlChildrenNode;
			while (cur_temp != NULL)
			{
				if ((!xmlStrcmp(cur_temp->name, "body")))
				{
					xmlNodePtr temp = cur_temp;
					temp = temp->xmlChildrenNode;
					while(temp != NULL)
					{
						if ((!xmlStrcmp(temp->name, "NODEID")))
							return atoi(xmlNodeListGetString(doc, temp->xmlChildrenNode, 1));
						temp = temp->next;
					}
				}
				cur_temp = cur_temp->next;
			}
		}
		cur = cur->next;
	}
	xmlFreeDoc(doc);

	return -1;
}





int get_initial_details(char *file,struct Node *node)
{
	printf("\n==================== Node Details ===========================\n");
	printf("Node details File: %s\n",file);
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile(file);
	if (doc == NULL ){
		die("Document parsing failed. \n");
		return 0;
	}

	//Gets the root element of the XML Doc
	cur = xmlDocGetRootElement(doc); 
	if (cur == NULL){
		xmlFreeDoc(doc);
		die("Document is Empty!!!\n");
		return 0;
	}

	while (cur != NULL){
		if ((!xmlStrcmp(cur->name, "INIT"))){
			xmlNodePtr cur_temp = cur;
			cur_temp = cur_temp->xmlChildrenNode;
			while (cur_temp != NULL){
				if ((!xmlStrcmp(cur_temp->name, "server_ip"))){
					node->server_ip=xmlNodeListGetString(doc, cur_temp->xmlChildrenNode, 1);
					printf("Central Server IP: %s\n",node->server_ip);
				}
				else if ((!xmlStrcmp(cur_temp->name, "node_ip"))){
					node->node_ip=xmlNodeListGetString(doc, cur_temp->xmlChildrenNode, 1);			
					printf("Node IP: %s\n",node->node_ip);
				}
				else if ((!xmlStrcmp(cur_temp->name, "virtual_tech"))){
					node->virtual_tech=xmlNodeListGetString(doc, cur_temp->xmlChildrenNode, 1);			
					printf("Virtualization Technology: %s\n",node->virtual_tech);
				}
				else if ((!xmlStrcmp(cur_temp->name, "image_path"))){
					node->image_path=xmlNodeListGetString(doc, cur_temp->xmlChildrenNode, 1);			
					printf("Virtual Image: %s\n",node->image_path);
				}
				else if ((!xmlStrcmp(cur_temp->name, "network_type"))){
					node->network_type=xmlNodeListGetString(doc, cur_temp->xmlChildrenNode, 1);			
					printf("Network IP Type: %s\n",node->network_type);
				}
				else if ((!xmlStrcmp(cur_temp->name, "interface"))){
					node->interface=xmlNodeListGetString(doc, cur_temp->xmlChildrenNode, 1);			
					printf("Network Interface Name: %s\n",node->interface);
				}
				else if ((!xmlStrcmp(cur_temp->name, "subnet"))){
					node->subnet=xmlNodeListGetString(doc, cur_temp->xmlChildrenNode, 1);
					printf("Network Subnet: %s\n",node->subnet);
				}
				else if ((!xmlStrcmp(cur_temp->name, "netmask"))){
					node->netmask=xmlNodeListGetString(doc, cur_temp->xmlChildrenNode, 1);
					printf("Network Netmask: %s\n",node->netmask);
				}
				else if ((!xmlStrcmp(cur_temp->name, "gateway"))){
					node->gateway=xmlNodeListGetString(doc, cur_temp->xmlChildrenNode, 1);
					printf("Network Gateway IP: %s\n",node->gateway);
				}
				else if ((!xmlStrcmp(cur_temp->name, "dns"))){
					node->dns=xmlNodeListGetString(doc, cur_temp->xmlChildrenNode, 1);			
					printf("Network DNS IP: %s\n",node->dns);
				}
				else if ((!xmlStrcmp(cur_temp->name, "organization"))){
					node->organization=xmlNodeListGetString(doc, cur_temp->xmlChildrenNode, 1);
					printf("Node Location: %s\n",node->organization);
				}
				else if ((!xmlStrcmp(cur_temp->name, "longitude"))){
					node->longitude=atof(xmlNodeListGetString(doc, cur_temp->xmlChildrenNode, 1));			
					printf("Location Longitude: %lf\n",node->longitude);
				}
				else if ((!xmlStrcmp(cur_temp->name, "latitude"))){
					node->latitude=atof(xmlNodeListGetString(doc, cur_temp->xmlChildrenNode, 1));
					printf("Location Lattitude: %lf\n",node->latitude);
				}
				else if ((!xmlStrcmp(cur_temp->name, "conf_timer"))){
					node->conf_timer=atoi(xmlNodeListGetString(doc, cur_temp->xmlChildrenNode, 1));
					printf("Configuration Timer: %d\n",node->conf_timer);
				}
				else if ((!xmlStrcmp(cur_temp->name, "data_timer"))){
					node->data_timer=atoi(xmlNodeListGetString(doc, cur_temp->xmlChildrenNode, 1));
					printf("Data Timer: %d\n",node->data_timer);
				}
				else if ((!xmlStrcmp(cur_temp->name, "pcap_timer"))){
					node->pcap_timer=atoi(xmlNodeListGetString(doc, cur_temp->xmlChildrenNode, 1));
					printf("Pcap Timer: %d\n",node->pcap_timer);
				}
				else if ((!xmlStrcmp(cur_temp->name, "organization_sector"))){
					node->organization_sector=xmlNodeListGetString(doc, cur_temp->xmlChildrenNode, 1);
					printf("Organization sector: %s\n",node->organization_sector);
				}
				else if ((!xmlStrcmp(cur_temp->name, "organization_region"))){
					node->organization_region=xmlNodeListGetString(doc, cur_temp->xmlChildrenNode, 1);
					printf("Organization Region: %s\n",node->organization_region);
				}

				cur_temp = cur_temp->next;
			}
		}
		cur = cur->next;
	}
	xmlFreeDoc(doc);
	return 1;
}

void node_details(struct Node node){
	printf("Node Details.\n");
	printf("\tNetwork Type: %s\n",node.network_type);
	printf("\tNetwork Interface : %s\n",node.interface);
	printf("\tServer IP: %s\n",node.server_ip);
	printf("\tNode IP: %s\n",node.node_ip);
	printf("\tNetwork Subnet %s\n",node.subnet);
	printf("\tNetmask: %s\n",node.netmask);
	printf("\tGateway: %s\n",node.gateway);
	printf("\tDNS IP: %s\n",node.dns);
	printf("\tVirtual Technology : %s\n",node.virtual_tech);
	printf("\tImage Path: %s\n",node.image_path);
	printf("\tNode Organization Name: %s\n",node.organization);
	printf("\tNode Organization Sector: %s\n",node.organization_sector);
	printf("\tNode Organization Region: %s\n",node.organization_region);
	printf("\tNode Longitude: %f\n",node.longitude);
	printf("\tNode Latitude: %f\n",node.latitude);
	printf("\tConf Timer: %d Seconds\n",node.conf_timer);
	printf("\tData Timer: %d seconds\n",node.data_timer);
	printf("\tPcap Timer: %d Seconds\n",node.pcap_timer);
}

