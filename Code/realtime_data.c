#include "common.h"
void* data_cdac_hp_read(void* arg)
{
	struct Honeypot *hp = (struct Honeypot*)arg;
	char *str;
	str=(char*)calloc(1,512);

	sprintf(str,"python ../Code/db.py /DockerShared/%s_%d/data_log.db %s %d %s %s %s %s",hp->hp_name,hp->hp_id,node.server_ip,node.node_id,hp->hp_name,node.organization,node.organization_sector,node.organization_region);
	system(str);

	free(str);
}