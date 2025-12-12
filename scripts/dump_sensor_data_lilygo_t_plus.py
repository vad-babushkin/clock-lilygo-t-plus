#!/usr/bin/python3
import redis
import sys
import json

from pyzabbix import ZabbixAPI, ZabbixAPIException

version = "231001.3"
# 10573   m5stack-fire
# 10567   Arharius
# 10586   xiaomi-router
# 10585   esp32-pico-d4-clock
# 10592   lilygo-t5-47-plus
# 10591   m5stack-atom-s3
# 10568   esp32-devkit-c-v4
# 10084   Zabbix server
# 10574   m5stack-stick
# 10577   esp32-s3-devkit-c-1
# 10572   m5stack-atom
# 10578   esp32-s3-devkit-m-1
# 10581   esp32-pico-d4

M5STACK_ATOM = 10572
m5stack_stick = 10574
ESP32_PICO_D4_CLOCK = 10585
ESP32_S3_DEVKIT_M_1 = 10578

#
def getSingleItemLastValue(zapi, key, hostid):
    try:
        items = zapi.item.get(search={"key_": key}, hostids=hostid, output=['lastvalue'])
        if len(items) == 0:
            print(f"item key: {key} not found in hostname: {hostid}")
            return None
        else:
            return items[0]["lastvalue"]

    except ZabbixAPIException as e:
        print(e)
        return None


#
zapi = ZabbixAPI("http://192.168.31.102:8080")
zapi.login(api_token='57a82bbe1b3abf2c0da1ad01fd764ec142c014041855a01d83946b2f9cba65e3')
# print("Connected to Zabbix API Version %s" % zapi.api_version())

tempValue = getSingleItemLastValue(zapi, "mhz19b.temp", M5STACK_ATOM)
#tempValue = getSingleItemLastValue(zapi, "mhz19b.temp", m5stack_stick)
co2Value = getSingleItemLastValue(zapi, "mhz19b.co2", M5STACK_ATOM)
#co2Value = getSingleItemLastValue(zapi, "mhz19b.co2", m5stack_stick)
humValue = getSingleItemLastValue(zapi, "aht10.humidity", ESP32_PICO_D4_CLOCK)
lux = getSingleItemLastValue(zapi, "veml7700.lux", ESP32_S3_DEVKIT_M_1)
if tempValue is None:
    tempValue = -1
if co2Value is None:
    co2Value = -1
if humValue is None:
    humValue = -1
if lux is None:
    lux = 10


d = {'temp': int(tempValue), 'co2': int(co2Value), 'hummidity': int(float(humValue)), 'lux': lux}
print(d)

r = redis.Redis(host='192.168.31.102', port=6379, decode_responses=True)
key = "192.168.31.63/sensors"
r.set(key, json.dumps(d))

sys.exit()
