#include "EEPROMSAVE.h"

//璃奈板亮度和颜色
const uint8_t defaultBoardBrightness = 100;
const uint32_t defaultColor = 0x00FF1493;

//灯条亮度和开关
const uint8_t defaultLightBrightness = 100;
const uint8_t defaultLightIsOn = 1;

//WIFI通讯相关
const String defaultSSID = "RinaBoard";
const String defaultPassword = "12345678";

const String defaultDeviceName = "璃奈板";

void EEPROMInit()
{
    EEPROM.begin(4096); //申请操作到地址4095（比如你只需要读写地址为100上的一个字节，该处也需输入参数101）
}

//———————————————————————————————————————————————————————— Read —————————————————————————————————————————————————————————————————
uint32_t readColor()
{
    if (EEPROM.read(COLOR_ADDR) != 0)
    {
        return EEPROM.readUInt(COLOR_ADDR + 1);
    } else
    {
        return defaultColor;
    }
}

uint8_t readBoardBrightness()
{
    if (EEPROM.read(BOARDBRIGHTNESS_ADDR) != 0)
    {
        return EEPROM.read(BOARDBRIGHTNESS_ADDR + 1);
    } else
    {
        return defaultLightBrightness;
    }
}

uint8_t readLightstate()
{
    if (EEPROM.read(LIGHTSTATE_ADDR) != 0)
    {
        return EEPROM.read(LIGHTSTATE_ADDR + 1);
    } else
    {
        return defaultLightIsOn;
    }
}

uint8_t readLightBrightness()
{
    if (EEPROM.read(LIGHTBRIGHTNESS_ADDR) != 0)
    {
        return EEPROM.read(LIGHTBRIGHTNESS_ADDR + 1);
    } else
    {
        return defaultLightBrightness;
    }
}

String readDeviceName()
{
    return defaultDeviceName;
}

String readWifiSSID()
{
    return defaultSSID;
}

String readWifiPassword()
{
    return defaultPassword;
}

//———————————————————————————————————————————————————————— Write —————————————————————————————————————————————————————————————————
void writeColor(uint32_t color)
{
    if (EEPROM.read(COLOR_ADDR) == 0)//如果第一位是0，说明color值未设置
    {
        EEPROM.write(COLOR_ADDR, 1);
    }
    EEPROM.writeUInt(COLOR_ADDR + 1, color);
    EEPROM.commit();
    vTaskDelay(1);
}

void writeBoardBrightness(uint8_t brightness)
{
    if (EEPROM.read(BOARDBRIGHTNESS_ADDR) == 0)
    {
        EEPROM.write(BOARDBRIGHTNESS_ADDR, 1);
    }
    EEPROM.write(BOARDBRIGHTNESS_ADDR + 1, brightness);
    EEPROM.commit();
    vTaskDelay(1);
}

void writeLightState(uint8_t lightIsOn)
{
    if (EEPROM.read(LIGHTSTATE_ADDR) == 0)
    {
        EEPROM.write(LIGHTSTATE_ADDR, 1);
    }
    EEPROM.write(LIGHTSTATE_ADDR + 1, lightIsOn);
    EEPROM.commit();
    vTaskDelay(1);
}

void writeLightBrightness(uint8_t brightness)
{
    if (EEPROM.read(LIGHTBRIGHTNESS_ADDR) == 0)
    {
        EEPROM.write(LIGHTBRIGHTNESS_ADDR, 1);
    }
    EEPROM.write(LIGHTBRIGHTNESS_ADDR + 1, brightness);
    EEPROM.commit();
    vTaskDelay(1);
}

void writeDeviceName(String deviceName)
{

}

void writeWifiSSID(String ssid)
{

}

void writeWifiPassword(String password)
{

}