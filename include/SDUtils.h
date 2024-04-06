#ifndef SDSAVE_H
#define SDSAVE_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#include "DEBUG.h"

//文件读取相关
extern SPIClass spi;

bool SDInit();
void startAnime(void (*showImage)(), uint8_t *bitmap);
void getExpressionListFromSD();
void saveExpression(const String& name, uint8_t *bitmap);
void getLastExpression(uint8_t *bitmap);
void getNextExpression(uint8_t *bitmap);

#endif //SDSAVE_H
