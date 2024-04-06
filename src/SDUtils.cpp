#include "SDUtils.h"
#include "DoublyLinkedList.h"

//文件读取相关
SPIClass spi = SPIClass(HSPI);
File myFile;
DoublyLinkedList expressionList;
DoublyLinkedList::Node *expression;
const String startExpreesionName = "start.bin";

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
#ifdef DEBUG
            USBSerial.println("存储卡挂载失败");
#endif
        }
        return false;
    } else
    {
        getExpressionListFromSD();
        return true;
    }
    // SD卡初始化结束
}

void getExpressionListFromSD()
{
    File root = SD.open("/");
    if (!root)
    {
#ifdef DEBUG
        USBSerial.println("Failed to open root directory");
#endif
        return;
    }

    while (File entry = root.openNextFile())
    {
        if (!entry.isDirectory() && !String(entry.name()).equals(startExpreesionName))
        {
            String filename = "/" + String(entry.name());
            expressionList.insert(filename);
#ifdef DEBUG
            USBSerial.printf("%s is inserted into expressionList\r\n", filename.c_str());
#endif
        }
        entry.close();
    }
    root.close(); // 关闭根目录
}

/**
 * @brief 开机动画
 * @param showImagin 屏幕显示驱动函数
 * @param bitmap 主程序的位图，开机动画播放结束后，主程序的位图会转变成开机动画的最后一帧
 * */
void startAnime(void (*showImage)(), uint8_t *bitmap)
{
    uint16_t time = 0;
    myFile = SD.open("/start.bin");
    uint8_t buf[4];
    bool endOfFile = false; // 添加文件结束标志

    while (myFile && !endOfFile) // 更新循环条件
    {
        switch (myFile.read())
        {
            case 0x00:
                myFile.read(bitmap, 48);
                showImage();
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
            case -1: // 文件末尾
                endOfFile = true;
                break;
            default:
                break;
        }
    }
    myFile.close();

    if (expressionList.getHead() != nullptr)
    {
#ifdef DEBUG
        USBSerial.println("There is a expression file");
#endif
        expression = expressionList.getHead();
        String bitmapname = expression->data;
        myFile = SD.open(bitmapname, FILE_READ);
        if (myFile)
        {
#ifdef DEBUG
            USBSerial.println("Show the expression named " + bitmapname);
#endif
            myFile.read(bitmap, 48);
            showImage();
        }
        myFile.close();
    }
#ifdef DEBUG
    USBSerial.println("Start anime is over");
#endif
}


/**
 * @brief 保存表情
 * @param name 表情名称
 * @param bitmap 表情的位图数据
 * */
void saveExpression(const String &name, uint8_t *bitmap)
{
    String filename = "/" + name + ".bin";
    myFile = SD.open(filename, FILE_WRITE);
    if (myFile)
    {
        myFile.write(bitmap, 48);
    }
    myFile.close();
    if (!expressionList.search(filename))
    {
        expressionList.insert(filename);
#ifdef DEBUG
        USBSerial.printf("%s is inserted into expressionList\r\n", filename.c_str());
#endif
    }
}

void getLastExpression(uint8_t *bitmap)
{
    if(expression->prev != nullptr)
    {
        expression = expression->prev;
        myFile = SD.open(expression->data, FILE_READ);
        if(myFile)
        {
            myFile.read(bitmap, 48);
        }
    }
}

void getNextExpression(uint8_t *bitmap)
{
    if(expression->next != nullptr)
    {
        expression = expression->next;
        myFile = SD.open(expression->data, FILE_READ);
        if(myFile)
        {
            myFile.read(bitmap, 48);
        }
    }
}
