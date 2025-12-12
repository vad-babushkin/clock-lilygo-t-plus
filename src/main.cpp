#ifndef BOARD_HAS_PSRAM
#error "Please enable PSRAM !!!"
#endif

#include <Arduino.h>
#include "EEPROM.h"
// #include "esp_sleep.h"
#include <esp_wifi.h>
#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Constants.h>
#include "time.h"
#include "sntp.h"
//#include <ESPAsyncWebServer.h>
//#include <AsyncElegantOTA.h>
//#include <ESP32ZabbixSender.h>
//#include <Redis.h>
//#include <ArduinoJson.h>
#include "epd_driver.h"
#include "libjpeg/libjpeg.h"
#include "font/firasans.h"
//#include "font/ZCOOLQingKeHuangYouRegular15b.h"
//#include "font/ZCOOLQingKeHuangYouRegular24b.h"
#include "font/ZCOOLQingKeHuangYouRegular48b.h"
// #include "font/ZCOOLQingKeHuangYouRegular72b.h"
#include "font/ZCOOLQingKeHuangYouRegular88b.h"
//#include "font/DynaPuffCondensedMedium15b.h"
//#include "font/DynaPuffCondensedMedium24b.h"
#include "font/DynaPuffCondensedMedium48b.h"
#include "font/DynaPuffCondensedMedium72b.h"
//#include "font/DynaPuffCondensedMedium88b.h"
//#include "font/GelloBold15b.h"
//#include "font/GelloBold24b.h"
#include "font/GelloBold48b.h"
#include "font/GelloBold72b.h"
//#include "font/GelloBold88b.h"
//#include "font/Meteocons96.h"
#include "esp_adc_cal.h"
#include <FS.h>
#include <SD.h>
#include "pins.h"
#include <FFat.h>
//#include "pcf8563.h"
#include <Wire.h>
#include "icons/batt_100.h"
#include "icons/batt_75.h"
#include "icons/batt_50.h"
#include "icons/batt_25.h"
#include "icons/batt_0.h"

/**
 * Sleep
 */
#define uS_TO_S_FACTOR 1000000 /* коэффициент пересчета микросекунд в секунды */
#define TIME_TO_SLEEP 60       /* время, в течение которого будет спать ESP32 (в секундах) */
#define START_TICK_TIME_WINDOW 60

/**
 * WiFi
 */
const int TRUST_WIFI_COUNT = 6;
const char *TRUST_WIFI_SSID[6] = {"dami", "Xiao", "MC", "MC1", "FB", "Finbox"};
const char *TRUST_WIFI_PASSPHRASE[6] = {"mama11mama11", "mama11mama11", "MCforever", "MCforever", "FBforever", "FB63258505"};
int selectedTrustWifiSSID = -1;

/**
 * nntp
*/
const char* ntpServer1 = "ru.pool.ntp.org";
// const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = 3600 * 5;
// const int   daylightOffset_sec = 3600;
const int   daylightOffset_sec = 0;

//const char* time_zone = "CET-1CEST,M3.5.0,M10.5.0/3";  // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)
const char *time_zone = "YEKT-5YEKST,M3.5.0,M10.5.0/3 ";  // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)

/**
 * lilygo-t5-47-plus
 */
int FROM_FILE = 1;
int FILES_COUNT = 336;
const char *files[336] = {
		"/AIR.JPG", "/ALIEN.JPG", "/ARTEMIDA.JPG", "/CHARLES.JPG", "/CLEONS.JPG", "/COSMOS.JPG",        // 5
		"/DEEP.JPG", "/ESCIMOS.JPG", "/FOX.JPG", "/FUTURE.JPG", "/GALAKSI.JPG", "/JOKER.JPG",           // 11
		"/MACON.JPG", "/MEDUZA.JPG", "/OKO.JPG", "/OLDMAN.JPG", "/PINK.JPG", "/ROBOT.JPG",              // 17
		"/SOBEK.JPG", "/START.JPG", "/SUNMOON.JPG", "/TRUTH.JPG", "/WAR.JPG", "/SAURON.JPG",            // 22
		"/ROCKETS2.JPG", "/ROCKETS3.JPG", "/NAUTILUS.JPG", "/LZ130.JPG", "/SUN.JPG", "/GALAXY.JPG",     //29
		"/AIR2.JPG", "/BAUHAUS.JPG", "/COBRA.JPG", "/FACEPALM.JPG", "/FISH1.JPG", "/FOX2.JPG",          // 35
		"/JOKER2.JPG", "/LAKE.JPG", "/MASON.JPG", "/RAM.JPG", "/ROBOCOP.JPG", "/SEA.JPG",               // 41
		"/SPACE.JPG", "/WARIOR.JPG", "/ZERO.JPG", "/AIR3.JPG", "/BELRIOSE.JPG", "/ELLO.JPG",            // 47
		"/FIRE.JPG", "/FISH2.JPG", "/FOX3.JPG", "/JOKER3.JPG", "/MANDALOR.JPG", "/PAST.JPG",            // 53
		"/RICH.JPG", "/SEA2.JPG", "/SHAMAN.JPG", "/TREE.JPG", "/YAGA.JPG", "/ZODIAC.JPG",               // 59
		"/AIR4.JPG", "/ALIEN2.JPG", "/AUTO2.JPG", "/AUTO.JPG", "/FSB.JPG", "/HAMSTER.JPG",              // 65
		"/JOKER4.JPG", "/LOKI.JPG", "/MAPPET.JPG", "/SHIP.JPG", "/ZERO2.JPG", "/FLAMMA.JPG",            // 71
		"/AGENT.JPG", "/BEAR.JPG", "/CHET.JPG", "/DEPP.JPG", "/GOA.JPG", "/RING.JPG",                   // 77
		"/ROCKETS4.JPG", "/SHIP2.JPG", "/SKY.JPG", "/SPACE2.JPG", "/STUPA.JPG", "/VADER.JPG",           // 83
		"/BEAR2.JPG", "/HOLE.JPG", "/IAM2.JPG", "/IAM.JPG", "/IND.JPG", "/SPHERA2.JPG",                 // 89
		"/SPHERA.JPG", "/SUN2.JPG", "/SUNMOON2.JPG", "/SUNMOON3.JPG", "/SUNMOON4.JPG", "/TREE2.JPG",    // 95
		"/AIR5.JPG", "/ANIMALS2.JPG", "/ANIMALS3.JPG", "/ANIMALS4.JPG", "/ANIMALS5.JPG", "/ANIMALS.JPG",// 101
		"/ASTRO.JPG", "/BEAR3.JPG", "/BEAR4.JPG", "/BEAR5.JPG", "/CROCO.JPG", "/LIZARD.JPG",            // 107
		"/AIR6.JPG", "/ALIEN3.JPG", "/ANIMALS7.JPG", "/ATOM.JPG", "/BEAR6.JPG", "/BOT.JPG",             // 113
		"/GERALT.JPG", "/MAUL.JPG", "/MOUSE.JPG", "/RABBIT.JPG", "/SNOWDEN.JPG", "/THANOS.JPG",         // 119
		"/ALFRED.JPG", "/ANIMALS6.JPG", "/ANIME.JPG", "/BATMAN.JPG", "/BOT2.JPG", "/CROCO2.JPG",        // 125
		"/JOKER5.JPG", "/MEZGIR.JPG", "/PRISON.JPG", "/SNAKE.JPG", "/TARKIN.JPG", "/WIND.JPG",          // 131
		"/ANIMALS8.JPG", "/ARCHER.JPG", "/BEAR7.JPG", "/BIRD.JPG", "/CAT.JPG", "/CROCO4.JPG",           // 137
		"/CROCO6.JPG", "/EVOLUT.JPG", "/FOX4.JPG", "/FOX6.JPG", "/MAN.JPG", "/PHANTOM.JPG",             // 143
		"/RACCOON.JPG", "/SHIP3.JPG", "/SHIP5.JPG", "/SKY2.JPG", "/WAGNER.JPG", "/WARIOR2.JPG",         // 149
		"/ARCHER2.JPG", "/BATMAN2.JPG", "/BEAR8.JPG", "/BUDANOV.JPG", "/CROCO3.JPG", "/CROCO5.JPG",     // 155
		"/DINNER.JPG", "/FOREST.JPG", "/FOX5.JPG", "/KREML.JPG", "/MERA.JPG", "/RABBIT2.JPG",           // 161
		"/SHAMAN2.JPG", "/SHIP4.JPG", "/SHIP6.JPG", "/WAGNER2.JPG", "/WALE.JPG", "/WORLD.JPG",          // 167
		"/BEAR9.JPG", "/BELKA.JPG", "/FOX7.JPG", "/PHOENIX.JPG", "/WARIOR3.JPG", "/WOMAN.JPG",          // 173
		"/BATERFLY.JPG", "/BIRD2.JPG", "/FUTURE2.JPG", "/IKAR.JPG", "/JOKER6.JPG", "/MAN3.JPG",         // 179
		"/MAN5.JPG", "/MILL.JPG", "/NOTE.JPG", "/SHAMAN3.JPG", "/SORROW.JPG", "/WOMAN2.JPG",            // 185
		"/BELKA2.JPG", "/DOG.JPG", "/HORSE.JPG", "/IND2.JPG", "/MAN2.JPG", "/MAN4.JPG",                 // 191
		"/MILL2.JPG", "/NOTE2.JPG", "/ROBOT2.JPG", "/SHIP7.JPG", "/SUN3.JPG", "/YAGA2.JPG",             // 197
		"/ALIEN4.JPG", "/ARTDECO.JPG", "/CROCO7.JPG", "/GHOST.JPG", "/JOKER7.JPG", "/JOKER8.JPG",       // 203
		"/MASK.JPG", "/MAUL2.JPG", "/PREDATOR.JPG", "/RAM2.JPG", "/RAM3.JPG", "/SPACE3.JPG",            // 209
		"/STAR.JPG", "/TREE3.JPG", "/VADER2.JPG", "/VADER3.JPG", "/VADER4.JPG", "/WARIOR4.JPG",         // 215
		"/ANONYM2.JPG", "/ANONYM.JPG", "/ARIES.JPG", "/BIRD3.JPG", "/FROG2.JPG", "/FROG3.JPG",          // 221
		"/FROG.JPG", "/KALMAR.JPG", "/MEDUZA2.JPG", "/OWL.JPG", "/RAM4.JPG", "/SUNMOON5.JPG",           // 227
		"/SUNMOON6.JPG", "/THANOS2.JPG", "/WARIOR5.JPG", "/WATCHMEN2.JPG", "/WATCHMEN3.JPG", "/WATCHMEN.JPG", // 233
		"/AZOG.JPG", "/FLOWER.JPG", "/JACKSON.JPG", "/MAN6.JPG", "/MASK2.JPG", "/PINKFLOYD2.JPG",            // 239
		"/PREDATOR2.JPG", "/PREDATOR3.JPG", "/QUEEN.JPG", "/ROBOCOP2.JPG", "/STATHEM.JPG", "/WARIOR6.JPG",   // 245
		"/BIRD4.JPG", "/BUDDA.JPG", "/CAT2.JPG", "/CHINA.JPG", "/DOM.JPG", "/FREEDOM2.JPG",                  // 251
		"/FREEDOM3.JPG", "/FREEDOM4.JPG", "/FREEDOM.JPG", "/GHOST2.JPG", "/JACKSON2.JPG", "/MAN10.JPG",      // 257
		"/MAN11.JPG", "/MAN7.JPG", "/MAN8.JPG", "/MAN9.JPG", "/WALE2.JPG", "/WARIOR7.JPG",                   // 263
		"/BAZHOV.JPG", "/BELKA3.JPG", "/BIRD5.JPG", "/BIRD6.JPG", "/BLADERN2.JPG", "/BLADERN.JPG",           // 269
		"/BOBER.JPG", "/DM.JPG", "/KORSAR.JPG", "/LEGION.JPG", "/MASK3.JPG", "/PO.JPG",                      // 275
		"/QUEEN2.JPG", "/RABBIT3.JPG", "/SHAMAN4.JPG", "/SKY3.JPG", "/T1000.JPG", "/WARIOR8.JPG",            // 281
		"/AIR7.JPG", "/ALIEN5.JPG", "/BIRD7.JPG", "/BLADERN3.JPG", "/CHEM.JPG", "/FROG4.JPG",                // 287
		"/FROG5.JPG", "/FROG6.JPG", "/FROG7.JPG", "/MAN12.JPG", "/MOUSE2.JPG", "/MUSIC.JPG",                 // 293
		"/PEOPLE.JPG", "/RABBIT4.JPG", "/SHIP8.JPG", "/TD2.JPG", "/TD.JPG", "/THING.JPG",                    // 299
		"/AIR8.JPG", "/BUER.JPG", "/DM2.JPG", "/FIGURE.JPG", "/FISH3.JPG", "/FOLK2.JPG",                     // 305
		"/FOLK3.JPG", "/FOLK4.JPG", "/FOLK.JPG", "/JOKER9.JPG", "/KINO.JPG", "/MAN13.JPG",                   // 311
		"/MATRIX.JPG", "/ROBOT3.JPG", "/SUN4.JPG", "/TOWN.JPG", "/TURTLE.JPG", "/WICK.JPG",                  // 317
		"/BEAR10.JPG", "/BEAR11.JPG", "/BIRD10.JPG", "/BIRD8.JPG", "/BIRD9.JPG", "/CLOWN.JPG",               // 323
		"/FOLK10.JPG", "/FOLK11.JPG", "/FOLK5.JPG", "/FOLK6.JPG", "/FOLK7.JPG", "/FOLK8.JPG",                // 329
		"/FOLK9.JPG", "/ROD2.JPG", "/ROD.JPG", "/SHAMAN5.JPG", "/SIRIN2.JPG", "/SIRIN.JPG"                   // 335
};

const char *WARNING = "/WARNING.JPG";
const char *DANGER = "/DANGER.JPG";

char old_buf[128] = {};
char buf[128] = {};
char sensor_buf[128] = {};

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 540
#define SCREEN_MARGIN 5

RTC_DATA_ATTR int old_month = 1;
RTC_DATA_ATTR int old_day = 1;
RTC_DATA_ATTR int old_hour = 0;
RTC_DATA_ATTR int old_minute = 0;

RTC_DATA_ATTR int month = 1;
RTC_DATA_ATTR int day = 1;
RTC_DATA_ATTR int hour = 0;
RTC_DATA_ATTR int minute = 0;

Box_t hourBox;

typedef struct {
	GFXfont hourFont;
	GFXfont dateFont;
	alignment hourAlignment;
	alignment dateAlignment;
	alignment co2Alignment;
	int32_t hourFontHeight;
	int32_t dateFontHeight;
	int32_t co2FontHeight;
} Box_Font_Param_t;

// W 393 H 133
const Box_Font_Param_t bigCenterBoxParam = {.hourFont = ZCOOLQingKeHuangYouRegular88B, .dateFont = ZCOOLQingKeHuangYouRegular48B,
		.hourAlignment = CENTER, .dateAlignment = CENTER, .co2Alignment = CENTER, .hourFontHeight= 133, .dateFontHeight = 73, .co2FontHeight = 41};
// W 274 H 107 MAX picture
const Box_Font_Param_t middleCenterBoxParam = {.hourFont = DynaPuffCondensedMedium72B, .dateFont = DynaPuffCondensedMedium48B,
		.hourAlignment = CENTER, .dateAlignment = CENTER, .co2Alignment = CENTER, .hourFontHeight= 108, .dateFontHeight = 82, .co2FontHeight = 41};
// W 195 H 135  MAX picture 765
const Box_Font_Param_t smallCenterBoxParam = {.hourFont = GelloBold72B, .dateFont = GelloBold48B,
		.hourAlignment = CENTER, .dateAlignment = CENTER, .co2Alignment = CENTER, .hourFontHeight= 135, .dateFontHeight = 100, .co2FontHeight = 41};

const Box_Font_Param_t manBoxParam = {.hourFont = ZCOOLQingKeHuangYouRegular88B, .dateFont = ZCOOLQingKeHuangYouRegular48B,
		.hourAlignment = RIGHT, .dateAlignment = CENTER, .co2Alignment = LEFT, .hourFontHeight= 133, .dateFontHeight = 73, .co2FontHeight = 41};
const Box_Font_Param_t airBoxParam = {.hourFont = ZCOOLQingKeHuangYouRegular88B, .dateFont = ZCOOLQingKeHuangYouRegular48B,
		.hourAlignment = RIGHT, .dateAlignment = LEFT, .co2Alignment = CENTER, .hourFontHeight= 133, .dateFontHeight = 73, .co2FontHeight = 41};
const Box_Font_Param_t macconBoxParam = {.hourFont = ZCOOLQingKeHuangYouRegular88B, .dateFont = ZCOOLQingKeHuangYouRegular48B,
		.hourAlignment = RIGHT, .dateAlignment = LEFT, .co2Alignment = CENTER, .hourFontHeight= 133, .dateFontHeight = 73, .co2FontHeight = 41};
const Box_Font_Param_t charleyBoxParam = {.hourFont = ZCOOLQingKeHuangYouRegular88B, .dateFont = ZCOOLQingKeHuangYouRegular48B,
		.hourAlignment = CENTER, .dateAlignment = CENTER, .co2Alignment = RIGHT, .hourFontHeight= 133, .dateFontHeight = 73, .co2FontHeight = 41};
const Box_Font_Param_t nautilusBoxParam = {.hourFont = ZCOOLQingKeHuangYouRegular88B, .dateFont = ZCOOLQingKeHuangYouRegular48B,
		.hourAlignment = LEFT, .dateAlignment = RIGHT, .co2Alignment = CENTER, .hourFontHeight= 133, .dateFontHeight = 73, .co2FontHeight = 41};
const Box_Font_Param_t frogBoxParam = {.hourFont = ZCOOLQingKeHuangYouRegular88B, .dateFont = ZCOOLQingKeHuangYouRegular48B,
		.hourAlignment = LEFT, .dateAlignment = CENTER, .co2Alignment = RIGHT, .hourFontHeight= 133, .dateFontHeight = 73, .co2FontHeight = 41};

//typedef struct {
//	int id;
//	char *name;
//} employee;
//#define INIT_EMPLOYEE(X) employee X = {.id = 0, .name ="none"}

GFXfont currentFont;

/**
 * RTC
 */
//PCF8563_Class rtc;
int vref = 1100;

/**
 * BATT
 */
const uint BATT_X = EPD_WIDTH - batt_100_width;
const uint BATT_Y = 0;
const Rect_t BATT_AREA = {
		.x = BATT_X,
		.y = BATT_Y,
		.width = batt_100_width,
		.height = batt_100_height,
};

//float old_battery_voltage;
uint8_t batt_icon = 0;

/**
 * SENSORS
 */
RTC_DATA_ATTR int old_co2 = 0;

/**
 * App
 */
RTC_DATA_ATTR long bootCount = 0;
RTC_DATA_ATTR long tick = 0;
RTC_DATA_ATTR int theme_id = 0;

bool wifi_enabled = false;
bool time_enabled = false;
bool redis_enabled = false;
bool sleep_need = false;
bool update_need = false;
bool rtc_set = false;
bool need_refresh = false;
bool firstRun = true;
bool initSd = false;

RTC_DATA_ATTR short step = 30;

// *************************************************************************************
// SPECIAL
// *************************************************************************************

String transEncryptionType(wifi_auth_mode_t encryptionType) {
	// 对比出该wifi网络加密类型并返回相应的String值
	switch (encryptionType) {
		case (WIFI_AUTH_OPEN):
			return "Open";
		case (WIFI_AUTH_WEP):
			return "WEP";
		case (WIFI_AUTH_WPA_PSK):
			return "WPA_PSK";
		case (WIFI_AUTH_WPA2_PSK):
			return "WPA2_PSK";
		case (WIFI_AUTH_WPA_WPA2_PSK):
			return "WPA_WPA2_PSK";
		case (WIFI_AUTH_WPA2_ENTERPRISE):
			return "WPA2_ENTERPRISE";
		default:
			return ("Unkonwn EncryptionType");
	}
}

/**
 *
 * @return .
 */
int scanNetworks() {
	// 扫描周边wifi网络，并显示wifi数量，打印它们的属性（ssid，信号强度，加密方式，mac地址）
	int numberOfNetworks = WiFi.scanNetworks();
	DBUG_PRINT("The number of networks found is:");
	DBUG_PRINTLN(numberOfNetworks);
	int selectedNum = -1;
	for (int i = 0; i < numberOfNetworks; i++) {
		DBUG_PRINT("Networkname: ");
		DBUG_PRINTLN(WiFi.SSID(i));

		for (int j = 0; j < TRUST_WIFI_COUNT; j++) {
			if (WiFi.SSID(i).equals(TRUST_WIFI_SSID[j])) {
				selectedNum = j;
			}
		}

		DBUG_PRINT("Signalstrength: ");
		DBUG_PRINTLN(WiFi.RSSI(i));
		DBUG_PRINT("MACaddress: ");
		DBUG_PRINTLN(WiFi.BSSIDstr(i));
		DBUG_PRINT("Encryptiontype: ");
		String encryptionTypeDescription = transEncryptionType(WiFi.encryptionType(i));
		DBUG_PRINTLN(encryptionTypeDescription);
		DBUG_PRINTLN("-----------------------");
	}
	DBUG_PRINT("selectedNum: ");
	DBUG_PRINTLN(selectedNum);

	return selectedNum;
}

/**
 *
*/
void printLocalTime() {
	struct tm timeinfo;
	if (!getLocalTime(&timeinfo)) {
		DBUG_PRINTLN("No time available (yet)");
		return;
	}
	DBUG_PRINTLN(&timeinfo, "%A, %B %d %Y %H:%M:%S");
	hour = timeinfo.tm_hour;
	minute = timeinfo.tm_min;
	day = timeinfo.tm_mday;
	month = timeinfo.tm_mon + 1;

}

/**
 * Callback function (get's called when time adjusts via NTP)
*/
void timeavailable(struct timeval *t) {
	DBUG_PRINTLN("Got time adjustment from NTP!");
	printLocalTime();
}


void refreshTheme();

/**
 *
 * @param font .
 */
void setFont(GFXfont const &font) {
	currentFont = font;
}

/**
 *
 * @param x .
 * @param y .
 * @param text .
 * @param align .
 */
void clearString(int x, int y, String text, alignment align, int right_x) {
	char *data = const_cast<char *>(text.c_str());
	int x1, y1;
	int w, h;
	int xx = x, yy = y;
	get_text_bounds(&currentFont, data, &xx, &yy, &x1, &y1, &w, &h, NULL);
	if (align == RIGHT) x = right_x - w;
	if (align == CENTER) x = x + (right_x - x - w) / 2;
	if (x > 0) {
		x--;
	}
	if (y > 0) {
		y--;
	}
	w += 2;
	h += 2;
	Rect_t area = {
			.x = x,
			.y = y - h,
			.width = w,
			.height = h,
	};
	epd_clear_area(area);
}

/**
 *
 * @param x .
 * @param y .
 * @param text .
 * @param align .
 */
void drawString(int x, int y, String text, alignment align, int right_x) {
	char *data = const_cast<char *>(text.c_str());
	int x1, y1;
	int w, h;
	int xx = x, yy = y;
	get_text_bounds(&currentFont, data, &xx, &yy, &x1, &y1, &w, &h, NULL);
//	DBUG_PRINTF("bounds x %d y %d right_x %d   xx %d yy %d x1 %d y1 %d  W %d H %d\r\n",
//	            x, y, right_x, xx, yy, x1, y1, w, h)

	if (align == RIGHT) x = right_x - w;
	if (align == CENTER) x = x + (right_x - x - w) / 2;
//	DBUG_PRINTF("align x %d\r\n", x)
	writeln((GFXfont * ) & currentFont, data, &x, &y, NULL);
}

/**
 *
 * @param x .
 * @param y .
 * @param text .
 * @param old_text .
 * @param align .
 */
void drawString(int x, int y, String text, String old_text, alignment align, int right_x) {
	if (!firstRun) {
		clearString(x, y, old_text, align, right_x);
	}
	drawString(x, y, text, align, right_x);
}

/**
 *
 */
void initializeSd() {
	if (!initSd) {
		bool rlst = SD.begin(SD_CS, SPI);
		if (!rlst) {
			DBUG_PRINTLN("SD init failed");
		} else {
			DBUG_PRINTLN("SD init success");
		}
		initSd = true;
	}
}

/**
 *
 */
void draw_file_to_screen(const char *path, Rect_t rect) {
	initializeSd();

	File jpg = SD.open(path);
//	DBUG_PRINT("  FILE: ");
//	DBUG_PRINT(jpg.name());
//	DBUG_PRINT("  SIZE: ");
//	DBUG_PRINTLN(jpg.size());

	String jpg_p;
	while (jpg.available()) {
		jpg_p += jpg.readString();
	}
//	DBUG_PRINTf("jpg w:%d,h:%d l:%d\r\n", rect.width, rect.height, jpg_p.length());
	show_jpg_from_buff((uint8_t *) jpg_p.c_str(), jpg_p.length(), rect);
}

/**
 *
 */
int generateNewThemeId() {
	int oldThemeId = theme_id;
	int cnt = 0;
	while (cnt < 10) {
		theme_id = random(FROM_FILE, FILES_COUNT + 1);
		if (theme_id != oldThemeId) {
			return theme_id;
		}
		cnt++;
	}
	return theme_id;
}

/**
 *
 */
void initTheme() {
	DBUG_PRINTF("change theme from %d\r\n", theme_id)
	theme_id = generateNewThemeId();
	DBUG_PRINTF("change theme to %d\r\n", theme_id)

	refreshTheme();
}

/**
 *
 */
void refreshTheme() {
	DBUG_PRINTF("refresh theme %d\r\n", theme_id)

	Rect_t rect = {
			.x = 0,
			.y = 0,
			.width = EPD_WIDTH,
			.height = EPD_HEIGHT,
	};
	if (theme_id > 0 && theme_id < FILES_COUNT + 1) {
		draw_file_to_screen(files[theme_id - 1], rect);
//	} else if (theme_id == 1001) {
//		draw_file_to_screen(DANGER, rect);
//	} else if (theme_id == 1002) {
//		draw_file_to_screen(WARNING, rect);
	} else {
		epd_clear();
	}

	need_refresh = true;
}

/**
 * общая
 */
void draw_common(Box_t box, Box_Font_Param_t boxFontParam) {
	// epd_draw_hline(10, random(10, EPD_HEIGHT), EPD_WIDTH - 20, 0, NULL);
	int cursor_x = box.bx0;
	int cursor_y = getHeight(box) / 4 + boxFontParam.hourFontHeight / 2;
	sprintf(old_buf, "%02d:%02d", old_hour, old_minute);
	sprintf(buf, "%02d:%02d", hour, minute);
	setFont(boxFontParam.hourFont);
	drawString(cursor_x, cursor_y, buf, old_buf, boxFontParam.hourAlignment, box.bx1);

	if (old_day != day || old_month != month || need_refresh) {
		sprintf(buf, "%02d %s", old_day, en_month[old_month - 1]);
		sprintf(buf, "%02d %s", day, en_month[month - 1]);
		cursor_y = 3 * getHeight(box) / 4 + boxFontParam.dateFontHeight / 2;
		setFont(boxFontParam.dateFont);
		drawString(cursor_x, cursor_y, buf, old_buf, boxFontParam.dateAlignment, box.bx1);
	}

//	sprintf(buf, "%02dC %02d%% %d", 28, 30, 500);
//	sprintf(buf, "%02dC %02d%% %d", 28, 30, 500);
	cursor_y = getHeight(box);
	setFont(FiraSans);
//	DBUG_PRINTLN(sensor_buf)
	drawString(cursor_x, cursor_y, sensor_buf, sensor_buf, boxFontParam.co2Alignment, box.bx1);

	need_refresh = false;
}

/**
 *
 */
void draw() {
//	DBUG_PRINTLN("= DRAW")

	DBUG_PRINTF("time %d %d %d %d \r\n", month, day, hour, minute);

	// ночью не обновляем
	if (hour < 10 || hour > 22) {
		return;
	}

	epd_poweron();
 	if (firstRun) {
 		epd_clear();
 		initTheme();
 		firstRun = false;
 	}
	if (minute == 0) {
		DBUG_PRINTLN("refresh theme every hour")
		initTheme();
	}
	if (minute != 0 && minute % 20 == 0) {
		DBUG_PRINTLN("refresh theme every 10 min")
		refreshTheme();
	}

	DBUG_PRINTLN(theme_id);
	switch (theme_id) {
		case 0:
			// clear
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 1:
			// air
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, airBoxParam);
			break;
		case 2:
			// alien
			hourBox = {.bx0 = 390 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 3:
			// artemida
			hourBox = {.bx0 = 430 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 4:
			// charlie
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 520 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, charleyBoxParam);
			break;
		case 5:
			// cleon
			hourBox = {.bx0 = 541 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 6:
			// cosmos
			hourBox = {.bx0 = 517 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 7:
			// deep
			hourBox = {.bx0 = 382 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 8:
			// escimos
			hourBox = {.bx0 = 557 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 9:
			// fox
			hourBox = {.bx0 = 541 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 10:
			// future
			hourBox = {.bx0 = 385 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 11:
			// galaxy
			hourBox = {.bx0 = 380 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 12:
			// joker
			hourBox = {.bx0 = 371 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 13:
			// macon
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, macconBoxParam);
			break;
		case 14:
			// MEDUZA
			hourBox = {.bx0 = 447 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 15:
			// OKO
			hourBox = {.bx0 = 541 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 16:
			// OLDMAN
			hourBox = {.bx0 = 509 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 17:
			// PINK
			hourBox = {.bx0 = 590 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 18:
			// ROBOT
			hourBox = {.bx0 = 446 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 19:
			// SOBEK
			hourBox = {.bx0 = 361 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 20:
			// START
			hourBox = {.bx0 = 444 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 21:
			// SUNMOON
			hourBox = {.bx0 = 412 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 22:
			// TRUTH
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 430 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 23:
			// WAR
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 24:
			// SAURON
			hourBox = {.bx0 = 700 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 25:
			// ROCKETS2
			hourBox = {.bx0 = 720 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 26:
			// ROCKETS3
			hourBox = {.bx0 = 540 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 27:
			// NAUTILUS
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, nautilusBoxParam);
			break;
		case 28:
			// LZ130
			hourBox = {.bx0 = 390 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 29:
			// SUN
			hourBox = {.bx0 = 460 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 30:
			// GALAXY
			hourBox = {.bx0 = 720 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 31:
			// AIR2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 640 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 32:
			// BAUHAUS
			hourBox = {.bx0 = 360 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 33:
			// COBRA
			hourBox = {.bx0 = 675 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 34:
			// FACEPALM
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 455 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 35:
			// FISH1
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 430 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 36:
			// FOX2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 575 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 37:
			// JOKER2
			hourBox = {.bx0 = 675 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 38:
			// LAKE
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 39:
			// MASON
			hourBox = {.bx0 = 420 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 40:
			// RAM
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 450 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 41:
			// ROBOCOP
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 505 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 42:
			// SEA
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 320 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 43:
			// SPACE
			hourBox = {.bx0 = 400 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 44:
			// WARIOR
			hourBox = {.bx0 = 480 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 45:
			// ZERO
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 590 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 46:
			// AIR3
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 47:
			// BELRIOSE
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 295 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 48:
			// ELLO
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 49:
			// FIRE
			hourBox = {.bx0 = 435 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 50:
			// FISH2
			hourBox = {.bx0 = 590 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 51:
			// FOX3
			hourBox = {.bx0 = 540 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 52:
			// JOKER3
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 300 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 53:
			// MANDALOR
			hourBox = {.bx0 = 525 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 54:
			// PAST
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 330 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 55:
			// RICH
			hourBox = {.bx0 = 390 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 56:
			// SEA2
			hourBox = {.bx0 = 540 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 57:
			// SHAMAN
			hourBox = {.bx0 = 435 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 58:
			// TREE
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 420 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 59:
			// YAGA
			hourBox = {.bx0 = 420 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 60:
			// ZODIAC
			hourBox = {.bx0 = 395 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 61:
			// AIR4
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 475 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 62:
			// ALIEN2
			hourBox = {.bx0 = 380 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 63:
			// AUTO2
			hourBox = {.bx0 = 365 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 64:
			// AUTO
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 530 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 65:
			// FSB
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 520 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 66:
			// HAMSTER
			hourBox = {.bx0 = 310 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 67:
			// JOKER4
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 380 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 68:
			// LOKI
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 420 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 69:
			// MAPPET
			hourBox = {.bx0 = 445 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 70:
			// SHIP
			hourBox = {.bx0 = 435 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 71:
			// ZERO2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 535 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 72:
			// FLAMMA
			hourBox = {.bx0 = 645 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 73:
			// AGENT
			hourBox = {.bx0 = 425 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 74:
			// BEAR
			hourBox = {.bx0 = 415 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 75:
			// CHET
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 530 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 76:
			// DEPP
			hourBox = {.bx0 = 500 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 77:
			// GOA
			hourBox = {.bx0 = 540 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 78:
			// RING
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 79:
			// ROCKETS4
			hourBox = {.bx0 = 535 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 80:
			// SHIP2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 525 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 81:
			// SKY
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 335 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 82:
			// SPACE2
			hourBox = {.bx0 = 475 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 83:
			// STUPA
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 465 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 84:
			// VADER
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 565 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 85:
			// BEAR2
			hourBox = {.bx0 = 550 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 86:
			// HOLE
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 550 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 87:
			// IAM2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 550 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 88:
			// IAM
			hourBox = {.bx0 = 450 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 89:
			// IND
			hourBox = {.bx0 = 335 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 90:
			// SPHERA2
			hourBox = {.bx0 = 440 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 91:
			// SPHERA
			hourBox = {.bx0 = 380 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 92:
			// SUN2
			hourBox = {.bx0 = 450 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 93:
			// SUNMOON2
			hourBox = {.bx0 = 540 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 94:
			// SUNMOON3
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 95:
			// SUNMOON4
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 525 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 96:
			// TREE2
			hourBox = {.bx0 = 405 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 97:
			// AIR5
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 98:
			// ANIMALS2
			hourBox = {.bx0 = 380 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 99:
			// ANIMALS3
			hourBox = {.bx0 = 400 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 100:
			// ANIMALS4
			hourBox = {.bx0 = 360 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 101:
			// ANIMALS5
			hourBox = {.bx0 = 440 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 102:
			// ANIMALS
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 440 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 103:
			// ASTRO
			hourBox = {.bx0 = 350 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 104:
			// BEAR3
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 300 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 105:
			// BEAR4
			hourBox = {.bx0 = 375 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 106:
			// BEAR5
			hourBox = {.bx0 = 385 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 107:
			// CROCO
			hourBox = {.bx0 = 645 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 108:
			// LIZARD
			hourBox = {.bx0 = 350 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 109:
			// AIR6
			hourBox = {.bx0 = 650 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 110:
			// ALIEN3
			hourBox = {.bx0 = 540 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 111:
			// ANIMALS7
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 420 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 112:
			// ATOM
			hourBox = {.bx0 = 470 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 113:
			// BEAR6
			hourBox = {.bx0 = 380 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 114:
			// BOT
			hourBox = {.bx0 = 520 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 115:
			// GERALT
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 560 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 116:
			// MAUL
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 565 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 117:
			// MOUSE
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 320 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 118:
			// RABBIT
			hourBox = {.bx0 = 510 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 119:
			// SNOWDEN
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 575 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 120:
			// THANOS
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 530 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 121:
			// ALFRED
			hourBox = {.bx0 = 425 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 122:
			// ANIMALS6
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 550 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 123:
			// ANIME
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 560 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 124:
			// BATMAN
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 550 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 125:
			// BOT2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 570 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 126:
			// CROCO2
			hourBox = {.bx0 = 370 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 127:
			// JOKER5
			hourBox = {.bx0 = 380 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 128:
			// MEZGIR
			hourBox = {.bx0 = 480 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 129:
			// PRISON
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 260 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 130:
			// SNAKE
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 380 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 131:
			// TARKIN
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 420 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 132:
			// WIND
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 300 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 133:
			// ANIMALS8
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 525 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 134:
			// ARCHER
			hourBox = {.bx0 = 300 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 135:
			// BEAR7
			hourBox = {.bx0 = 300 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 136:
			// BIRD
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 600 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 137:
			// CAT
			hourBox = {.bx0 = 730 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 138:
			// CROCO4
			hourBox = {.bx0 = 630 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 139:
			// CROCO6
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 540 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 140:
			// EVOLUT
			hourBox = {.bx0 = 245 + SCREEN_MARGIN, .bx1 = 725 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 141:
			// FOX4
			hourBox = {.bx0 = 400 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 142:
			// FOX6
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 550 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 143:
			// MAN
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 535 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 144:
			// PHANTOM
			hourBox = {.bx0 = 650 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 145:
			// RACCOON
			hourBox = {.bx0 = 480 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 146:
			// SHIP3
			hourBox = {.bx0 = 360 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 147:
			// SHIP5
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 500 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 148:
			// SKY2
			hourBox = {.bx0 = 520 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 149:
			// WAGNER
			hourBox = {.bx0 = 670 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 150:
			// WARIOR2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 540 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 151:
			// ARCHER2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 600 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 152:
			// BATMAN2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 535 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 153:
			// BEAR8
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 630 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 154:
			// BUDANOV
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 250 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 155:
			// CROCO3
			hourBox = {.bx0 = 520 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 156:
			// CROCO5
			hourBox = {.bx0 = 440 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 157:
			// DINNER
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 340 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 158:
			// FOREST
			hourBox = {.bx0 = 325 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 159:
			// FOX5
			hourBox = {.bx0 = 300 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 160:
			// KREML
			hourBox = {.bx0 = 400 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 161:
			// MERA
			hourBox = {.bx0 = 540 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 162:
			// RABBIT2
			hourBox = {.bx0 = 540 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 163:
			// SHAMAN2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 600 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 164:
			// SHIP4
			hourBox = {.bx0 = 440 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 165:
			// SHIP6
			hourBox = {.bx0 = 540 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 166:
			// WAGNER2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 360 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 167:
			// WALE
			hourBox = {.bx0 = 405 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 168:
			// WORLD
			hourBox = {.bx0 = 430 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 169:
			// BEAR9
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 610 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 170:
			// BELKA
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 650 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 171:
			// FOX7
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 600 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 172:
			// PHOENIX
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 570 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 173:
			// WARIOR3
			hourBox = {.bx0 = 540 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 174:
			// WOMAN
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 660 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 175:
			// BATERFLY
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 580 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 176:
			// BIRD2
			hourBox = {.bx0 = 400 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 177:
			// FUTURE2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 500 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 178:
			// IKAR
			hourBox = {.bx0 = 380 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 179:
			// JOKER6
			hourBox = {.bx0 = 460 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 180:
			// MAN3
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 420 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 181:
			// MAN5
			hourBox = {.bx0 = 375 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 182:
			// MILL
			hourBox = {.bx0 = 570 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 183:
			// NOTE
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 540 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 184:
			// SHAMAN3
			hourBox = {.bx0 = 370 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 185:
			// SORROW
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 600 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 186:
			// WOMAN2
			hourBox = {.bx0 = 300 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 187:
			// BELKA2
			hourBox = {.bx0 = 380 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 188:
			// DOG
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 600 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 189:
			// HORSE
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 400 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 190:
			// IND2
			hourBox = {.bx0 = 455 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 191:
			// MAN2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 580 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 192:
			// MAN4
			hourBox = {.bx0 = 500 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 193:
			// MILL2
			hourBox = {.bx0 = 540 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 194:
			// NOTE2
			hourBox = {.bx0 = 260 + SCREEN_MARGIN, .bx1 = 710 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 195:
			// ROBOT2
			hourBox = {.bx0 = 435 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 196:
			// SHIP7
			hourBox = {.bx0 = 435 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 197:
			// SUN3
			hourBox = {.bx0 = 530 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 198:
			// YAGA2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 600 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 199:
			// ALIEN4
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 560 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 200:
			// ARTDECO
			hourBox = {.bx0 = 375 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 201:
			// CROCO7
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 600 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 202:
			// GHOST
			hourBox = {.bx0 = 375 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 203:
			// JOKER7
			hourBox = {.bx0 = 470 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 204:
			// JOKER8
			hourBox = {.bx0 = 235 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 205:
			// MASK
			hourBox = {.bx0 = 480 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 206:
			// MAUL2
			hourBox = {.bx0 = 410 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 207:
			// PREDATOR
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 525 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 208:
			// RAM2
			hourBox = {.bx0 = 415 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 209:
			// RAM3
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 354 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 210:
			// SPACE3
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 580 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 211:
			// STAR
			hourBox = {.bx0 = 550 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 212:
			// TREE3
			hourBox = {.bx0 = 520 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 213:
			// VADER2
			hourBox = {.bx0 = 320 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 214:
			// VADER3
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 650 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 215:
			// VADER4
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 570 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 216:
			// WARIOR4
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 400 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 217:
			// ANONYM2
			hourBox = {.bx0 = 600 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 218:
			// ANONYM
			hourBox = {.bx0 = 500 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 219:
			// ARIES
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 500 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 220:
			// BIRD3
			hourBox = {.bx0 = 310 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 221:
			// FROG2
			hourBox = {.bx0 = 560 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 222:
			// FROG3
			hourBox = {.bx0 = 490 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 223:
			// FROG
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 410 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 224:
			// KALMAR
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 580 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 225:
			// MEDUZA2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 620 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 226:
			// OWL
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 620 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 227:
			// RAM4
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 500 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 228:
			// SUNMOON5
			hourBox = {.bx0 = 530 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 229:
			// SUNMOON6
			hourBox = {.bx0 = 440 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 230:
			// THANOS2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 550 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 231:
			// WARIOR5
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 760 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 232:
			// WATCHMEN2
			hourBox = {.bx0 = 380 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 233:
			// WATCHMEN3
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 520 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 234:
			// WATCHMEN
			hourBox = {.bx0 = 410 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 235:
			// AZOG
			hourBox = {.bx0 = 400 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 236:
			// FLOWER
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 600 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 237:
			// JACKSON
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 460 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 238:
			// MAN6
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 645 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 239:
			// MASK2
			hourBox = {.bx0 = 400 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 240:
			// PINKFLOYD2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 395 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 241:
			// PREDATOR2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 510 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 242:
			// PREDATOR3
			hourBox = {.bx0 = 400 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 243:
			// QUEEN
			hourBox = {.bx0 = 400 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 244:
			// ROBOCOP2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 380 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 245:
			// STATHEM
			hourBox = {.bx0 = 400 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 246:
			// WARIOR6
			hourBox = {.bx0 = 565 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 247:
			// BIRD4
			hourBox = {.bx0 = 520 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 248:
			// BUDDA
			hourBox = {.bx0 = 520 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 249:
			// CAT2
			hourBox = {.bx0 = 510 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 250:
			// CHINA
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 400 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 251:
			// DOM
			hourBox = {.bx0 = 600 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 252:
			// FREEDOM2
			hourBox = {.bx0 = 400 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 253:
			// FREEDOM3
			hourBox = {.bx0 = 420 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 254:
			// FREEDOM4
			hourBox = {.bx0 = 505 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 255:
			// FREEDOM
			hourBox = {.bx0 = 400 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 256:
			// GHOST2
			hourBox = {.bx0 = 580 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 257:
			// JACKSON2
			hourBox = {.bx0 = 500 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 258:
			// MAN10
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 540 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 259:
			// MAN11
			hourBox = {.bx0 = 425 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 260:
			// MAN7
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 590 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 261:
			// MAN8
			hourBox = {.bx0 = 360 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 262:
			// MAN9
			hourBox = {.bx0 = 600 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 263:
			// WALE2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 550 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 264:
			// WARIOR7
			hourBox = {.bx0 = 440 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 265:
			// BAZHOV
			hourBox = {.bx0 = 540 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 266:
			// BELKA3
			hourBox = {.bx0 = 440 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 267:
			// BIRD5
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 635 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 268:
			// BIRD6
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 400 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 269:
			// BLADERN2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 550 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 270:
			// BLADERN
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 560 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 271:
			// BOBER
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 550 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 272:
			// DM
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 570 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 273:
			// KORSAR
			hourBox = {.bx0 = 600 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 274:
			// LEGION
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 505 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 275:
			// MASK3
			hourBox = {.bx0 = 440 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 276:
			// PO
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 530 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 277:
			// QUEEN2
			hourBox = {.bx0 = 400 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 278:
			// RABBIT3
			hourBox = {.bx0 = 405 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 279:
			// SHAMAN4
			hourBox = {.bx0 = 540 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 280:
			// SKY3
			hourBox = {.bx0 = 550 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 281:
			// T1000
			hourBox = {.bx0 = 440 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 282:
			// WARIOR8
			hourBox = {.bx0 = 400 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 283:
			// AIR7
			hourBox = {.bx0 = 425 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 284:
			// ALIEN5
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 530 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 285:
			// BIRD7
			hourBox = {.bx0 = 365 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 286:
			// BLADERN3
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 420 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 287:
			// CHEM
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 570 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 288:
			// FROG4
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, frogBoxParam);
			break;
		case 289:
			// FROG5
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 380 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 290:
			// FROG6
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 555 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 291:
			// FROG7
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 540 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 292:
			// MAN12
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 500 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, manBoxParam);
			break;
		case 293:
			// MOUSE2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 445 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 294:
			// MUSIC
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 510 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 295:
			// PEOPLE
			hourBox = {.bx0 = 300 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 296:
			// RABBIT4
			hourBox = {.bx0 = 465 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 297:
			// SHIP8
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 600 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 298:
			// TD2
			hourBox = {.bx0 = 400 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 299:
			// TD
			hourBox = {.bx0 = 415 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 300:
			// THING
			hourBox = {.bx0 = 350 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 301:
			// AIR8
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 395 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 302:
			// BUER
			hourBox = {.bx0 = 550 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 303:
			// DM2
			hourBox = {.bx0 = 665 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 304:
			// FIGURE
			hourBox = {.bx0 = 600 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 305:
			// FISH3
			hourBox = {.bx0 = 330 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 306:
			// FOLK2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 560 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 307:
			// FOLK3
			hourBox = {.bx0 = 360 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 308:
			// FOLK4
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 380 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 309:
			// FOLK
			hourBox = {.bx0 = 300 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 310:
			// JOKER9
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 375 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 311:
			// KINO
			hourBox = {.bx0 = 570 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 312:
			// MAN13
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 580 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 313:
			// MATRIX
			hourBox = {.bx0 = 660 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		case 314:
			// ROBOT3
			hourBox = {.bx0 = 540 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 315:
			// SUN4
			hourBox = {.bx0 = 520 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 316:
			// TOWN
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 420 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 317:
			// TURTLE
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 435 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 318:
			// WICK
			hourBox = {.bx0 = 375 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 319:
			// BEAR10
			hourBox = {.bx0 = 260 + SCREEN_MARGIN, .bx1 = 695 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 320:
			// BEAR11
			hourBox = {.bx0 = 380 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 321:
			// BIRD10
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 610 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 322:
			// BIRD8
			hourBox = {.bx0 = 335 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 323:
			// BIRD9
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 605 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 324:
			// CLOWN
			hourBox = {.bx0 = 435 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 325:
			// FOLK10
			hourBox = {.bx0 = 400 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 326:
			// FOLK11
			hourBox = {.bx0 = 450 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 327:
			// FOLK5
			hourBox = {.bx0 = 395 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 328:
			// FOLK6
			hourBox = {.bx0 = 255 + SCREEN_MARGIN, .bx1 = 700 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 329:
			// FOLK7
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 565 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 330:
			// FOLK8
			hourBox = {.bx0 = 260 + SCREEN_MARGIN, .bx1 = 685 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 331:
			// FOLK9
			hourBox = {.bx0 = 330 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 332:
			// ROD2
			hourBox = {.bx0 = 395 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 333:
			// ROD
			hourBox = {.bx0 = 380 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 334:
			// SHAMAN5
			hourBox = {.bx0 = 535 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, middleCenterBoxParam);
			break;
		case 335:
			// SIRIN2
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = 590 - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 336:
			// SIRIN
			hourBox = {.bx0 = 445 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;

		case 1001:
			// WARNING
			hourBox = {.bx0 = 540 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
		case 1002:
			// DANGER
			hourBox = {.bx0 = 735 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, smallCenterBoxParam);
			break;
		default:
			DBUG_PRINTLN("DEFAULT");
			hourBox = {.bx0 = 0 + SCREEN_MARGIN, .bx1 = SCREEN_WIDTH - SCREEN_MARGIN,
					.by0 = 0 + SCREEN_MARGIN, .by1 = SCREEN_HEIGHT - SCREEN_MARGIN};
			draw_common(hourBox, bigCenterBoxParam);
			break;
	}
	// epd_poweroff_all();
	epd_poweroff();
}

/**
 *
 */
void display_pictures() {
	for (int i = 0; i < 72; i++) {
		DBUG_PRINTLN(i);
		DBUG_PRINTLN(files[i]);
		File jpg = SD.open(files[i]);
		DBUG_PRINT("  FILE: ");
		DBUG_PRINT(jpg.name());
		DBUG_PRINT("  SIZE: ");
		DBUG_PRINTLN(jpg.size());

		String jpg_p;
		while (jpg.available()) {
			jpg_p += jpg.readString();
		}

		Rect_t rect = {
				.x = 0,
				.y = 0,
				.width = EPD_WIDTH,
				.height = EPD_HEIGHT,
		};
		DBUG_PRINTF("jpg w:%d,h:%d l:%d\r\n", rect.width, rect.height, jpg_p.length());
		epd_poweron();
		show_jpg_from_buff((uint8_t *) jpg_p.c_str(), jpg_p.length(), rect);
		epd_poweroff();
		delay(10000);
	}
}

// *************************************************************************************
// INIT
// *************************************************************************************

/**
 *
 */
void initWifi() {
	if (selectedTrustWifiSSID < 0) {
		DBUG_PRINTLN("SSID do not selected!!!");
		return;
	}

	DBUG_PRINTLN();
	DBUG_PRINTLN();
	DBUG_PRINT("Connecting to ");
	DBUG_PRINTLN(TRUST_WIFI_SSID[selectedTrustWifiSSID]);

	// setup WiFi
	WiFi.mode(WIFI_STA);
	WiFi.begin(TRUST_WIFI_SSID[selectedTrustWifiSSID], TRUST_WIFI_PASSPHRASE[selectedTrustWifiSSID]);

	int cnt = 0;
	while (WiFi.status() != WL_CONNECTED && cnt < 100) {
		delay(200);
		DBUG_PRINT(".");
		cnt++;
	}

	wifi_enabled = WiFi.status() == WL_CONNECTED;

	if (wifi_enabled) {
		DBUG_PRINTLN("WiFi connected.");
		DBUG_PRINTLN("IP address: ");
		DBUG_PRINTLN(WiFi.localIP());
		DBUG_PRINTLN(WiFi.RSSI());
	} else {
		DBUG_PRINTLN("WiFi NOT connected!!!");
	}
	DBUG_PRINT("wifi_enabled ");
	DBUG_PRINTLN(wifi_enabled);
}

/**
 *
 */
void reInitWifi() {
	if (selectedTrustWifiSSID < 0) {
		DBUG_PRINTLN("SSID do not selected!!!");
		return;
	}
	DBUG_PRINTLN();
	DBUG_PRINTLN();
	DBUG_PRINT("Connecting to ");
	DBUG_PRINTLN(TRUST_WIFI_SSID[selectedTrustWifiSSID]);

	// setup WiFi
	WiFi.mode(WIFI_STA);
	WiFi.begin(TRUST_WIFI_SSID[selectedTrustWifiSSID], TRUST_WIFI_PASSPHRASE[selectedTrustWifiSSID]);

	int cnt = 0;
	while (WiFi.status() != WL_CONNECTED && cnt < 100) {
		delay(200);
		DBUG_PRINT(".");
		cnt++;
	}

	wifi_enabled = WiFi.status() == WL_CONNECTED;

	if (wifi_enabled) {
		DBUG_PRINTLN("WiFi connected.");
		DBUG_PRINTLN("IP address: ");
		DBUG_PRINTLN(WiFi.localIP());
		DBUG_PRINTLN(WiFi.RSSI());
	} else {
		DBUG_PRINTLN("WiFi NOT connected!!!");
	}
	DBUG_PRINT("wifi_enabled ");
	DBUG_PRINTLN(wifi_enabled);
}

/**
 *
*/
void initTime() {
	// set notification call-back function
	sntp_set_time_sync_notification_cb(timeavailable);

	/**
	 * NTP server address could be aquired via DHCP,
	 *
	 * NOTE: This call should be made BEFORE esp32 aquires IP address via DHCP,
	 * otherwise SNTP option 42 would be rejected by default.
	 * NOTE: configTime() function call if made AFTER DHCP-client run
	 * will OVERRIDE aquired NTP server address
	 */
	sntp_servermode_dhcp(1);    // (optional)

	/**
	 * This will set configured ntp servers and constant TimeZone/daylightOffset
	 * should be OK if your time zone does not need to adjust daylightOffset twice a year,
	 * in such a case time adjustment won't be handled automagicaly.
	 */
	configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
	// configTzTime(time_zone, ntpServer1, ntpServer2);
}

/**
 *
 */
void initLilygo() {
	SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
//	DBUG_PRINTLN("GET VREF")
	// Correct the ADC reference voltage
	esp_adc_cal_characteristics_t adc_chars;
	esp_adc_cal_value_t val_type = esp_adc_cal_characterize(
			ADC_UNIT_2,
			ADC_ATTEN_DB_11,
			ADC_WIDTH_BIT_12,
			1100,
			&adc_chars);
	if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
		DBUG_PRINTF("eFuse Vref: %umV\r\n", adc_chars.vref);
		vref = adc_chars.vref;
	}

//	Wire.begin(TOUCH_SDA, TOUCH_SCL);
//	rtc.begin();
//	DBUG_PRINTLN(rtc.formatDateTime(PCF_TIMEFORMAT_YYYY_MM_DD_H_M_S));

//	DBUG_PRINTLN("INIT EPD")
	/** Initialize the screen */
	epd_init();
	libjpeg_init();

//	esp_sleep_enable_ext1_wakeup(GPIO_SEL_21, ESP_EXT1_WAKEUP_ALL_LOW);
}

// *************************************************************************************
// REFRESH
// *************************************************************************************

// *************************************************************************************
// ARDUINO
// *************************************************************************************

/**
 *
 * @return .
 */
void setup() {
//		delay(10000);
	serial_enabled = initSerial();
	DBUG_PRINTLN("")
	DBUG_PRINTLN("=== ON SETUP ===")
	DBUG_PRINTLN("")

	disableCore0WDT(); // Network requests may block long enough to trigger watchdog

	DBUG_PRINTLN("--- initWifi ---");
	selectedTrustWifiSSID = scanNetworks();

	initWifi();
	initTime();

	initLilygo();
}

/**
 *
 * @return .
 */
void loop() {
	DBUG_PRINTLN("")
	DBUG_PRINTLN("=== ON LOOP ===")
	DBUG_PRINTLN("")

	if (WiFi.status() != WL_CONNECTED) {
		reInitWifi();
	}

	printLocalTime();
	draw();

	DBUG_PRINTF("tick %d. do simple sleep \r\n", tick);
	delay(TIME_TO_SLEEP * 1000);
	tick += TIME_TO_SLEEP;

	old_hour = hour;
	old_minute = minute;
	old_day = day;
	old_month = month;
}
