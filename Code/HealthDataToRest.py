#!/usr/bin/python

import datetime
import requests
import json
import time
import sys
import os


class Node:
    def __init__(self, url_path, file_path):
        self.url_path = url_path
        self.file_path = file_path

    def read_data_file(self,file_name):
        try:
            if os.path.exists(file_name):
                status_code = 201
                with open(file_name) as openfileobject:
                    for line in openfileobject:
                        status_code = 0
                        status_code = self.send_data_to_rest(self.url_path, line)
                if status_code == 201:
                    os.remove(file_name)
        except Exception as e:
            self.data_logger("Exception in read_data_file(): "+str(e))

    def send_data_to_rest(self, url, health_data):
        try:
            headers = {'content-type': 'application/json'}
            r = requests.post(url, data=json.dumps(json.loads(health_data)), headers=headers)
            return r.status_code
        except Exception as e:
            self.data_logger("Exception in send_data_to_rest(): "+str(e))

    def data_write_file(self,file_name, data):
        try:
            f = open(file_name, "a")
            f.write(data+"\n")
            f.close()
        except Exception as e:
            print("Exception in data_write_file(): "+str(e))

    def data_logger(self,message):
        try:
            date = datetime.datetime.now().strftime("%Y-%m-%d")
            date_time = datetime.datetime.now().strftime("%Y-%m-%dT%H:%M:%S")
            file_name = "../logs/health/%s.log"%(date)
            if not os.path.exists("../logs/health/"):
                os.makedirs("../logs/health/")
            data = date_time + "\t" +str(message)
            self.data_write_file(file_name, data)
        except Exception as e:
            print("Exception in data_logger(): "+str(e))


def main(server_ip, log_file):
    try:
        url_path = "http://"+server_ip+":8080/NodeAttackDataService/rest/attackdata/health_data"
        node = Node(url_path, log_file)
        node.read_data_file(node.file_path)
    except Exception as e:
        print("Exception in main(): "+str(e))


if __name__ == "__main__":
    if len(sys.argv) == 3:
        server_ip = str(sys.argv[1])
        log_file  = str(sys.argv[2])
        main(server_ip, log_file)
    else:
        print("---------------------------------------------------------------")
        print("Please execute program with server_ip & log file path")
        print("Example:")
        print("\tpython HealthDataToRest.py 192.168.8.4 /home/data_file.log")
