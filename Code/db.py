#!/usr/bin/python

import datetime
import requests
import sqlite3
import time
import json
import sys
import os


def data_write_file(file_name, data):
    try:
        f = open(file_name, "a")
        f.write(data+"\n")
        f.close()
    except Exception as e:
        print("Exception in data_write_file(): "+str(e))


def data_logger(message):
    date = datetime.datetime.now().strftime("%Y-%m-%d")
    date_time = datetime.datetime.now().strftime("%Y-%m-%dT%H:%M:%S")
    file_name = "../logs/database/%s.log"%(date)
    if not os.path.exists("../logs/database/"):
        os.makedirs("../logs/database/")
    data = date_time + "\t" +str(message)
    data_write_file(file_name, data)
	

def main(file_path, server_ip, node_id, hp_name, index_type, url, organization, organization_sector, organization_region):
    data_logger("Program Started")
    close_flag = file_path + "flag_close"
    record_id = 0
    while True:
        db_file = file_path + "data_log.db"
        temp_db_file = file_path + "temp_data_log.db"
        if os.path.exists(db_file):
            command = "cp '{}' {}".format(db_file, temp_db_file)
            os.system(command)
            if os.path.exists(temp_db_file):
                conn = sqlite3.connect(temp_db_file)
            else:
                data_logger(temp_db_file + " does not found.")
                return 0
        else:
            data_logger(db_file + " does not found.")
            return 0
        
        try:
            query = "SELECT * from shared_events WHERE event_id > {}".format(record_id)
            cursor = conn.execute(query)
            for row in cursor:
                try:
                    #attack_data = eval(row[7])
                    attack_data = row[7]
                    payload = {'organization_sector': organization_sector, 'organization_region': organization_region,
                               'organization': organization, 'node_id': node_id, 'honeypot': hp_name,
                               'type': index_type, 'tag': row[1], 'attack_time': row[2], 'remote_ip': row[3],
                               'remote_port': row[4], 'local_ip': row[5], 'local_port': row[6],
                               'attack_data': attack_data, 'target_profile': row[8], 'stage': row[9],
                               'vul_name': row[10], 'service': row[12]}
                    res = requests.post(url, data=json.dumps(payload))
                    #print(payload)
                    record_id = row[0]
                except Exception as e:
                    data_logger(e)
                    print(e)
            conn.close()
        except Exception as e:
            print(e)
            data_logger(e)
            conn.close()
        time.sleep(10)
        if os.path.exists(close_flag):
            time.sleep(2)
            data_logger("Close Flag found. Program terminated")
            os.remove(close_flag)
            break


if __name__ == "__main__":
    if len(sys.argv) == 8:
        file_path  = sys.argv[1]
        server_ip  = sys.argv[2]
        node_id    = int(sys.argv[3])
        hp_name    = sys.argv[4]
        index_type = hp_name + "_events"
        url        = "http://%s:8082/contentListener"%(server_ip)
        organization = sys.argv[5]
        organization_sector = sys.argv[6]
        organization_region = sys.argv[7]
        main(file_path, server_ip, node_id, hp_name, index_type, url, organization, organization_sector, organization_region)
    else:
        print("---------------------------------------------------------------")
        print("Please execute program with db file path, server_ip, node_id, hp_name, organization name, organization sector, organization region")
        print("Example:")
        print("\tpython db.py /root/ 192.168.8.4 2 cdac_hp CDAC-Mohali-Punjab govt-national North")
        sys.exit()
