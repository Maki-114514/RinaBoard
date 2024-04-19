#include "SDUtils.h"
#include "DoublyLinkedList.h"

//文件读取相关
SPIClass spi = SPIClass(HSPI);
File myFile;
DoublyLinkedList expressionList;
DoublyLinkedList::Node *expression;
const String startExpreesionName = "start.bin";
const String startFilePath = "/" + startExpreesionName;

/**
 * @brief: SD驱动初始化
 * */
bool SDInit()
{
    // SD卡初始化
    spi.begin(12, 13, 11, 10);
    if (!SD.begin(10, spi))
    {
        if (PORT.available())
        {
#ifdef DEBUG
            PORT.println("存储卡挂载失败");
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
        PORT.println("Failed to open root directory");
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
            PORT.printf("%s is inserted into expressionList\r\n", filename.c_str());
#endif
        }
        entry.close();
    }
    root.close(); // 关闭根目录
}

String getStringFromList()
{
    String str = "";
    DoublyLinkedList::Node *fileNode = expressionList.getHead();
    if (fileNode)
    {
        str += getNameFromPath(fileNode->data);
        fileNode = fileNode->next;
        while (fileNode != expressionList.getHead())
        {
            str += "|";
            str += getNameFromPath(fileNode->data);
            fileNode = fileNode->next;
        }
    }
#ifdef DEBUG
    PORT.println("The String is :" + str);
#endif
    return str;
}

String getNameFromPath(const String &path)
{
    // 查找最后一个斜杠的位置
    int lastSlashIndex = path.lastIndexOf('/');
    if (lastSlashIndex == -1)
    {
        // 没有斜杠，直接返回原字符串
        return path;
    }

    // 查找最后一个点的位置（文件扩展名的起始位置）
    int lastDotIndex = path.lastIndexOf('.');
    if (lastDotIndex == -1 || lastDotIndex < lastSlashIndex)
    {
        // 没有点，或者点在斜杠之前，直接返回文件名
        return path.substring(lastSlashIndex + 1);
    }

    // 截取文件名部分（不包括斜杠和扩展名）
    return path.substring(lastSlashIndex + 1, lastDotIndex);
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
        PORT.printf("%s is inserted into expressionList\r\n", filename.c_str());
#endif
    }
}

void removeExpression(const String &name)
{
    String filename = "/" + name + ".bin";
    if (SD.remove(filename))
    {
        if (expressionList.search(filename))
        {
            expressionList.remove(filename);
#ifdef DEBUG
            PORT.printf("%s is removed from expressionList\r\n", filename.c_str());
#endif
        }
    }
}

void getExpreesion(const String &name, uint8_t *bitmap)
{
    String filename = "/" + name + ".bin";
    if (expressionList.findNode(filename) != nullptr)
    {
#ifdef DEBUG
        PORT.println("Change to bitmap named " + filename);
#endif
        expression = expressionList.findNode(filename);
        myFile = SD.open(expression->data, FILE_READ);
        if (myFile)
        {
            myFile.read(bitmap, 48);
        }
        myFile.close();
    }
}

void getLastExpression(uint8_t *bitmap)
{
    if (expression == nullptr)
    {
        if (expressionList.getHead() != nullptr)
        {
            expression = expressionList.getHead();
            String bitmapname = expression->data;
            myFile = SD.open(bitmapname, FILE_READ);
            if (myFile)
            {
                myFile.read(bitmap, 48);
            }
            myFile.close();
        }
        return;
    }
    if (expression->prev != nullptr)
    {
        expression = expression->prev;
        myFile = SD.open(expression->data, FILE_READ);
        if (myFile)
        {
            myFile.read(bitmap, 48);
        }
        myFile.close();
    }
}

void getNextExpression(uint8_t *bitmap)
{
    if (expression == nullptr)
    {
        if (expressionList.getHead() != nullptr)
        {
            expression = expressionList.getHead();
            String bitmapname = expression->data;
            myFile = SD.open(bitmapname, FILE_READ);
            if (myFile)
            {
                myFile.read(bitmap, 48);
            }
            myFile.close();
        }
        return;
    }
    if (expression->next != nullptr)
    {
        expression = expression->next;
        myFile = SD.open(expression->data, FILE_READ);
        if (myFile)
        {
            myFile.read(bitmap, 48);
        }
        myFile.close();
    }
}

void clearStartBitmap()
{
    SD.remove(startFilePath);
    myFile = SD.open(startFilePath, FILE_WRITE);
    myFile.close();
}

void appendBitmap(const uint8_t *img)
{
    myFile = SD.open(startFilePath, FILE_APPEND);
#ifdef DEBUG
    PORT.println("Append bitmap to start anime");
#endif
    if (myFile)
    {
        myFile.write(0x00);
        myFile.write(img, 48);
    }
    myFile.close();
}

void appendBitmapOnBoard(const String &name)
{
    String filename = "/" + name + ".bin";
    uint8_t bitmap[48];
    myFile = SD.open(filename, FILE_READ);
    if (myFile)
    {
#ifdef DEBUG
        PORT.println("Append bitmap to start anime named " + filename);
#endif
        myFile.read(bitmap, 48);
        myFile.close();
        appendBitmap(bitmap);
        return;
    }
    myFile.close();
}

void appendMicroSeconds(const uint8_t *ms)
{
    myFile = SD.open(startFilePath, FILE_APPEND);
    if (myFile)
    {
        uint16_t data = (ms[0] << 8) | ms[1];
#ifdef DEBUG
        PORT.println("Append delay to start anime: " + (String) data + "ms");
#endif
        myFile.write(0x80);
        myFile.write(ms, 2);
    }
    myFile.close();
}

void appendSeconds(uint8_t s)
{
    myFile = SD.open(startFilePath, FILE_APPEND);
    if (myFile)
    {
#ifdef DEBUG
        PORT.println("Append delay to start anime: " + (String) s + "s");
#endif
        myFile.write(0xC0);
        myFile.write(s);
    }
    myFile.close();
}