/**************************************************************
	file   : common.h
	author : Kuwar Singh
	date   : 2018-02-08
**************************************************************/
#ifndef HEADER_FILE
#define HEAER_FILE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <stdlib.h>
#include <pthread.h>

// maximum number of threads 
#define MAX_THREAD 50

pthread_t threads[MAX_THREAD];

/*Structure for node details*/
typedef struct Node{
	int node_id;
	int conf_timer;
	int data_timer;
	int pcap_timer;
	int hp_update_timer;
	float longitude;
	float latitude;
	char *server_ip;
	char *network_type;
	char *interface;
	char *node_ip;
	char *subnet;
	char *netmask;
	char *gateway;
	char *dns;
	char *virtual_tech;
	char *image_path;
	char *organization;
	char *organization_sector;
	char *organization_region;
}Node;
	
/*Structre for honeypot configuration*/
struct Honeypot{
	int hp_id;
	int dc_count;
	int uconf_id;
	pthread_t tid;
	char *hp_name;
	char *vm_type;
	char *vm_name;
	char *os_type;
	char *hp_type;
	char *hp_category;
	char *vm_snapshot;
	char *network_type;
	char *ip;
	char *netmask;
	char *gateway;
	char *dns;
	char *hp_profile;
	char *hp_repo_path;
	int start_time;
	struct Honeypot *next_hp;
};
struct Honeypot *ahp_config, *hp_list;

typedef struct
{
    char *conf_name;
    char *conf_path;
    char *blueprint_name;
    char *blueprint_path;
}conf_files_st;

struct service{
	char *service_name;
	struct service *next;
};
struct service *service_list;

struct IP{
	char *ip_address;
	struct IP *next;
};
struct IP *ip_list;

struct Node node;
int ahp_data_flag;
int hp_id;
char *startupFile;
char *servCmdFile;
char *hp_type;

void die(char *msg);
int get_node_id_from_response(char *data);
int get_initial_details(char *file,Node *node);
void node_details(struct Node node);

//main.c
int display_hp();
int main();


//network.c
void configure_network(struct Node *node);
void docker_network(struct Node *node);
void virtual_network(struct Node *node);
int public_network(char *subnet, char *ip, char *netmask, char *gw,char *interface);
int dhcp_network(char *interface);
int remove_network();
int remove_docker_network();
int remove_docker_container();

//registration.c
int register_node(struct Node *node);
void write_nodeid(int node_id);
int read_nodeid(struct Node *node);

//httpcomm.c
int register_Node_api(conf_files_st *p_conf_st);
int get_ahp_url();
int get_lahp_url();
int send_compressed_data(char *filename);
int get_conf_file();

//configuration.c
int configure(char *honeypot_conf_file);
int configure_manager(char *docname);
void set_blank(struct Honeypot *h_config);
void memory_allocate(struct Honeypot *h_config);
void set_value(struct Honeypot *source, struct Honeypot *dest);
void *ahp_honeypot(void *args);
int set_network_value(struct Honeypot *hpNet,char *ncmd);
void get_network_command(struct Honeypot *hpNet, xmlChar *service);
void get_network_details(xmlDocPtr doc, xmlNodePtr cur, struct Honeypot *hpNet);
void write_service_command(xmlChar *service);
int get_vm_details(xmlDocPtr doc, xmlNodePtr cur, struct Honeypot *hpConfig);
void get_web_server_details(xmlDocPtr doc, xmlNodePtr cur);
void get_service_details(xmlDocPtr doc, xmlNodePtr cur);
int configure_hp(xmlDocPtr doc, xmlNodePtr cur);
int configure_vbox(struct Honeypot *hpConfig);

//vbox.c
int vm_power_on(char *vm_name);
int vm_power_off(char *vm_name);
int check_vm(char *vm_name);
int check_vm_status(char *vm_name);
int check_vbox_status();
int vm_snapshot_restore(char *vm_name, char *vm_snapshot);


//dockerconfiguration.c
void get_docker_network_details(xmlDocPtr doc, xmlNodePtr cur, struct Honeypot *hpNet);
void get_docker_service_details(xmlDocPtr doc, xmlNodePtr cur);
int docker_configure_hp(xmlDocPtr doc, xmlNodePtr cur);
int docker_configure_manager(char *docname);
int configure_container(struct Honeypot *hp);


//docker.c
int start_container(struct Honeypot *hp);
int stop_container(struct Honeypot *hp);
int rm_container(struct Honeypot *hp);
int create_container(struct Honeypot *hp);
int copy_base_to_container(struct Honeypot *hp, char *src, char *dest);
int copy_container_to_base(struct Honeypot *hp, char *src, char *dest);
int rm_container_data(struct Honeypot *hp,char *path);
int execute_container_command(struct Honeypot *hp, char *command);
int remove_all_containers();
int pull_docker_image(char *image,char *image_id);
int check_docker_image_status(char *image_id);


//data_collector.c
int data_collector(int config_flag);
int data_collect_from_docker(struct Honeypot *hpConfig);
int data_collect_send(struct Honeypot *hpConfig);

//pcap.c
int pcap_data_collection();
int start_pcap_capturing();
int create_path(char *path,char *hp_name,int hp_id);
int check_path(char *path);
void logger(char *log,char *module);

//date_time.c
int time_sync(char *server_ip);
int time_sync_with_server(char *server_ip);
void* data_read(void* arg);

//iptables.c
int set_data_control();


#endif
