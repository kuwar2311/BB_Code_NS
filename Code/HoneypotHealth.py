#!/usr/bin/python


import datetime
import psutil
import json
import time
import os


class Honeypot:
    def __init__(self, file_name, node_id, hp_name, u_conf_id):
        self.file_name = file_name
        self.node_id = node_id
        self.hp_name = hp_name
        self.u_conf_id = u_conf_id
        self.in_net_data = 0
        self.out_net_data = 0
        
    def data_logger(self, message):
        try:
            date_time = datetime.datetime.now().strftime("%Y-%m-%dT%H:%M:%S")
            file_name = "/var/log/health/error.log"
            if not os.path.exists("/var/log/health/"):
                os.makedirs("/var/log/health/")
            data = date_time + "\t" + str(message)
            self.data_log_file(file_name, data)
        except Exception as e:
            print("Exception in data_logger(): "+str(e))

    def data_log_file(self, log_file, data):
        try:
            f = open(log_file, "a")
            f.write(data+"\n")
            f.close()
        except Exception as e:
            print("Exception in data_write_file(): "+str(e))

    def data_write_file(self, data):
        try:
            f = open(self.file_name, "a")
            f.write(json.dumps(data)+"\n")
            f.close()
        except Exception as e:
            print("Exception in data_write_file(): "+str(e))

    def net_usage(self, inf="eth0"):  # change the inf variable according to the interface
        try:
            net_stat = psutil.net_io_counters(pernic=True, nowrap=True)[inf]
            net_in_1 = net_stat.bytes_recv
            net_out_1 = net_stat.bytes_sent

            net_in = round((net_in_1 - self.in_net_data) / 1024 / 1024, 3)
            net_out = round((net_out_1 - self.out_net_data) / 1024 / 1024, 3)

            #print(f"Current net-usage:\nIN: {net_in} MB/s, OUT: {net_out} MB/s")

            self.in_net_data = net_stat.bytes_recv
            self.out_net_data = net_stat.bytes_sent
            return net_in,net_out

        except Exception as e:
            self.data_logger("Exception in data_write_file(): "+str(e))
            print("Exception in data_write_file(): "+str(e))

    def get_health_data(self):
        try:
            net_in,net_out = self.net_usage("eth0")
            date_time = datetime.datetime.now().strftime("%Y-%m-%dT%H:%M:%S")
            #cpu_utilization = psutil.cpu_percent(interval = 1, percpu = False)
            cpu_utilization = psutil.cpu_percent(percpu = False)
            ram_stats = psutil.virtual_memory()
            available_ram = ram_stats[1] / (1024 * 1024)
            data = {"hp_name": "" + self.hp_name + "", "node_id": "" + self.node_id + "",
                    "u_conf_id": "" + self.u_conf_id + "", "cpu_stats": "" + str(cpu_utilization) + "",
                    "net_data_in": "" + str(net_in) + "", "net_data_out": "" + str(net_out) + "",
                    "mem_stats": "" + str(available_ram) + "",
                    "date_time": "" + date_time + ""}
            self.data_write_file(data)
            self.data_logger("get_health_data: {}".format(data))
        except Exception as e:
            self.data_logger("Exception in get_health_data(): "+str(e))
            print("Exception in get_health_data(): "+str(e))


def read_file(file_name):
    try:
        f = open(file_name, "r")
        line = f.readline()
        f.close()
        return line
    except Exception as e:
        self.data_logger("Exception in read_file(): "+str(e))
        print("Exception in read_file(): "+str(e))


def main():
    try:
        if os.path.exists("/root/health_config.conf"):
            node_data = json.loads(read_file("/root/health_config.conf"));
            node_id = node_data['node_id']
            hp_name = node_data['hp_name']
            u_conf_id = node_data['u_conf_id']
            data_timer = node_data['data_timer']
            log_file_path = node_data['log_file_path']

            honeypot1 = Honeypot(log_file_path, node_id, hp_name, u_conf_id)
            while True:
                honeypot1.get_health_data()
                time.sleep(data_timer)
        else:
            self.data_logger("health_config.conf does not exist")
            print("health_config.conf does not exist")
    except Exception as e:
        self.data_logger("Exception in main(): "+str(e))
        print("Exception in main(): "+str(e))


if __name__ == "__main__":
    main()
