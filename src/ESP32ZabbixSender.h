#ifndef _ESP32ZabbixSender_H_
#define _ESP32ZabbixSender_H_

#include "Arduino.h"
#include <WiFi.h>
#define ZABBIXMAXLEN 300 // max 256byte packet
#define ZABBIXMAXITEM 50 // max 50 items
#define ZABBIXTIMEOUT 1000 // 1000ms

#define SILENT

class ESP32ZabbixSender {
public:
	ESP32ZabbixSender(void);
	void Init(IPAddress ZabbixServerAddr, uint16_t ZabbixServerPort, String ZabbixItemHostName);
	int Send(void);
	void ClearItem(void);
	void AddItem(String key, double value);

private:
	int createZabbixPacket(void);
	struct zabbixCell {
		String key;
		double val;
	};
	WiFiClient zClient;
	IPAddress zAddr;
	uint16_t zPort;
	String zItemHost;
	uint8_t zabbixPacket[ZABBIXMAXLEN];
	zabbixCell zabbixItemList[ZABBIXMAXITEM];
	int zabbixItemSize;
};

#endif
