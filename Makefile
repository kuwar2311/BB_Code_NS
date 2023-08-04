cc = gcc
CFLAGS = -g
CFLAGS1 = -I/usr/include/libxml2/ -I/usr/local/axis2c/ -I/usr/local/include/
CFLAGS2 = -L/usr/lib/ -lpthread -L/usr/local/axis2c/lib  -L/usr/local/lib/ -lxml2 -lcurl
default:
	$(cc) $(CFLAGS) $(CFLAGS1) -o Release/bb_client Code/main.c Code/date_time.c Code/xmlReader.c Code/network.c Code/registration.c Code/httpComm.c Code/configuration.c Code/vbox.c Code/data_collector.c Code/dockerconfiguration.c Code/docker.c Code/pcap.c  Code/realtime_data.c Code/iptables.c $(CFLAGS2)
clean:
	rm -rf temp Release logs
	mkdir temp Release logs