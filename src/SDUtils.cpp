#include "SDUtils.h"

//文件读取相关
SPIClass spi = SPIClass(HSPI);
File myFile;

/**
 * @brief: SD驱动初始化
 * */
bool SDInit()
{
    // SD卡初始化
    spi.begin(12, 13, 11, 10);
    if (!SD.begin(10, spi))
    {
        if (USBSerial.available())
        {
            USBSerial.println("存储卡挂载失败");
        }
        return false;
    } else
    {
        return true;
    }
    // SD卡初始化结束
}

/**
 * @brief: 开机动画
 * @param: showImagin 屏幕显示驱动函数
 * */
void startAnime(void (*showImagin)(uint8_t*))
{
    uint16_t time = 0;
    myFile = SD.open("/start.bin");
    uint8_t buf[48];
    while(myFile)
    {
        switch (myFile.read())
        {
            case 0x00:
                myFile.read(buf, 48);
                showImagin(buf);
                break;
            case 0x80:
                myFile.read(buf, 2);
                time = (buf[0] << 8) | buf[1];
                vTaskDelay(pdMS_TO_TICKS(time));
                break;
            case 0xC0:
                time = myFile.read();
                for (uint16_t i = 0; i < time; ++i)
                {
                    vTaskDelay(pdMS_TO_TICKS(1000));
                }
                break;
            default:
                break;
        }
    }
    myFile.close();
}

void saveExpression(const String& name, uint8_t *bitmap)
{
    String filename = "/" + name + ".bin";
    myFile = SD.open(filename, FILE_WRITE);
    if(myFile)
    {
        myFile.write(bitmap, 48);
    }
    myFile.close();
}