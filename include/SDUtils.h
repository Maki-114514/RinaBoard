#ifndef SDSAVE_H
#define SDSAVE_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

//开启串口调试
#include "DEBUG.h"

//文件读取相关
extern SPIClass spi;

bool SDInit();
void startAnime(void (*showImage)(), uint8_t *bitmap);
void getExpressionListFromSD();
String getStringFromList();
String getNameFromPath(const String& path);
void saveExpression(const String& name, uint8_t *bitmap);
void removeExpression(const String &name);
void getExpreesion(const String &name, uint8_t* bitmap);
void getLastExpression(uint8_t *bitmap);
void getNextExpression(uint8_t *bitmap);
void clearStartBitmap();
void appendBitmap(const uint8_t *img);
void appendBitmapOnBoard(const String &name);
void appendMicroSeconds(const uint8_t* ms);
void appendSeconds(uint8_t s);

#endif //SDSAVE_H
