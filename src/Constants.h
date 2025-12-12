//
// Created by vad on 23.04.2023.
//

#ifndef ICONIUX_MUNIN_ATOM_CONSTANTS_H
#define ICONIUX_MUNIN_ATOM_CONSTANTS_H

bool serial_enabled = false;
#define DBUG_PRINT(...) if (serial_enabled) { Serial.print( __VA_ARGS__ ); }
#define DBUG_PRINTLN(...) if (serial_enabled) { Serial.println( __VA_ARGS__ ); }
#define DBUG_PRINTF(...) if (serial_enabled) { Serial.printf( __VA_ARGS__ ); }
//#define DEBUG_PRINTLN(struct tm * timeinfo, const char * format) if (serial_enabled) { Serial.println( __VA_ARGS__ ); }
#define DBUG_FLUSH() if (serial_enabled) { Serial.flush(); }

const char *roman_month[] = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X", "XI", "XII"};
const char *en_month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

bool ext0_wakeup = false;
bool ext1_wakeup = false;

/**
 *
*/
bool initSerial() {
	Serial.begin(115200);
	int i = 0;
	while (!Serial) {
		delay(100);
		i++;
		if (i > 100) {
			return false;
		}
	};
	return true;
}

/**
 *
 */
bool print_wakeup_reason() {
	esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

	switch (wakeup_reason) {
		case ESP_SLEEP_WAKEUP_UNDEFINED :
			DBUG_PRINTLN("In case of deep sleep, reset was not caused by exit from deep sleep");
			return false;
		case ESP_SLEEP_WAKEUP_ALL :
			DBUG_PRINTLN("Not a wakeup cause, used to disable all wakeup sources with esp_sleep_disable_wakeup_source");
			return false;
		case ESP_SLEEP_WAKEUP_EXT0 :
			//  "Пробуждение от внешнего сигнала с помощью RTC_IO"
			DBUG_PRINTLN("Wakeup by external signal using RTC_IO");
			ext0_wakeup = true;
			return false;
		case ESP_SLEEP_WAKEUP_EXT1 :
			//  "Пробуждение от внешнего сигнала с помощью RTC_IO"
			DBUG_PRINTLN("Wakeup by external signal using RTC_CNTL");
			ext1_wakeup = true;
			return false;
		case ESP_SLEEP_WAKEUP_TIMER :
			//  "Пробуждение от таймера"
			DBUG_PRINTLN("Wakeup by internal timer");
			return true;
		case ESP_SLEEP_WAKEUP_TOUCHPAD :
			//  "Пробуждение от сенсорного контакта"
			DBUG_PRINTLN("Wakeup by touchpad");
			return false;
		case ESP_SLEEP_WAKEUP_ULP :
			//  "Пробуждение от ULP-программы"
			DBUG_PRINTLN("Wakeup by ULP program");
			return false;
		case ESP_SLEEP_WAKEUP_GPIO:
			DBUG_PRINTLN("Wakeup by GPIO");
			return false;
		case ESP_SLEEP_WAKEUP_UART:
			DBUG_PRINTLN("Wakeup by UART");
			return false;
		case ESP_SLEEP_WAKEUP_WIFI:
			DBUG_PRINTLN("Wakeup caused by WIFI (light sleep only)");
			return false;
		case ESP_SLEEP_WAKEUP_COCPU:
			DBUG_PRINTLN("Wakeup caused by COCPU int");
			return false;
		case ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG:
			DBUG_PRINTLN("Wakeup caused by COCPU crash");
			return false;
		case ESP_SLEEP_WAKEUP_BT:
			DBUG_PRINTLN("Wakeup caused by BT (light sleep only)");
			return false;
		default :
			DBUG_PRINTF("Wakeup was not caused by deep sleep %d \r\n", wakeup_reason);
			return false;
	}
}

enum alignment {
	LEFT, RIGHT, CENTER
};

/**
 * @brief An area on the display.
 */
typedef struct
{
	int32_t bx0;
	int32_t bx1;
	int32_t by0;
	int32_t by1;
} Box_t;

/**
 *
 * @param box .
 * @return  .
 */
int32_t getHeight(Box_t box) {
	return box.by1 - box.by0;
}
/**
 *
 * @param box .
 * @return  .
 */
int32_t getWidth(Box_t box) {
	return box.bx1 - box.bx0;
}


#endif //ICONIUX_MUNIN_ATOM_CONSTANTS_H
