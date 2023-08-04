cp /usr/share/zoneinfo/Asia/Kolkata /etc/localtime
rm -rf /opt/dionaea/etc/dionaea/services-enabled/*
cp -f /root/services-enabled/* /opt/dionaea/etc/dionaea/services-enabled/

server_ip=`cat /root/server_ip`
node_details=`cat /root/node_details`

sed -i "s/SERVER_IP/$server_ip/g" /opt/dionaea/lib/dionaea/python/dionaea/logsql.py
sed -i "s/NODE_DETAILS/$node_details/g" /opt/dionaea/lib/dionaea/python/dionaea/logsql.py

#useradd -ms /bin/bash dionaea_user
#cd /root/ipt-kit/
#bash dionaea-ipt.sh
#chmod 777 /opt/dionaea/var -R
#su - dionaea_user -c "/opt/dionaea/bin/dionaea -c /opt/dionaea/etc/dionaea/dionaea.conf"
#service dionaea restart
#su - dionaea_user -c "/opt/dionaea/bin/dionaea -c /opt/dionaea/etc/dionaea/dionaea.cfg"
/opt/dionaea/bin/dionaea -c /opt/dionaea/etc/dionaea/dionaea.cfg
vim kk
