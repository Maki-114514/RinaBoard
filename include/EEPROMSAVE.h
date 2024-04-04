#ifndef EEPROMSAVE_H
#define EEPROMSAVE_H


#include <EEPROM.h>

#define COLOR_ADDR 0x00
#define BOARDBRIGHTNESS_ADDR 0x05
#define LIGHTSTATE_ADDR 0x07
#define LIGHTBRIGHTNESS_ADDR 0x09
#define DEVICENAME_ADDR 0x0B
#define WIFISSID_ADDR 0x34
#define WIFIPASSWORD_ADDR 0x5D

void EEPROMInit();

uint32_t readColor();
uint8_t readBoardBrightness();
uint8_t readLightstate();
uint8_t readLightBrightness();
String readDeviceName();
String readWifiSSID();
String readWifiPassword();

void writeColor(uint32_t color);
void writeBoardBrightness(uint8_t brightness);
void writeLightState(uint8_t lightIsOn);
void writeLightBrightness(uint8_t brightness);
void writeDeviceName(String *deviceName);
void writeWifiSSID(String *ssid);
void writeWifiPassword(String *password);

#endif //EEPROMSAVE_H
