#!/usr/bin/python3
from pyzabbix import ZabbixAPI

zapi = ZabbixAPI("http://192.168.31.102:8080")
#zapi.login("zabbix user", "zabbix pass")
# You can also authenticate using an API token instead of user/pass with Zabbix >= 5.4
zapi.login(api_token='57a82bbe1b3abf2c0da1ad01fd764ec142c014041855a01d83946b2f9cba65e3')
print("Connected to Zabbix API Version %s" % zapi.api_version())

for h in zapi.host.get(output="extend"):
    print(h['hostid'])
    print(h['name'])