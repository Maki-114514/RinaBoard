#ifndef SDSAVE_H
#define SDSAVE_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

//文件读取相关
extern SPIClass spi;

bool SDInit();
void startAnime(void (*showImagin)(uint8_t*));
void saveExpression(const String& name, uint8_t *bitmap);

#endif //SDSAVE_H
