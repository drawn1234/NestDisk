#ifndef COMMON_H
#define COMMON_H

#include <QString>
////////////////////文件信息/////////////////
struct FileInfo
{

    FileInfo():fileid(0) , size(0),pos(0)
    , isPause(0) , pFile( nullptr ){

    }

    int fileid;//文件id
    QString name;//文件名
    QString dir;//网盘路径
    QString time;//时间戳
    int size;//文件大小 32位 文件最大2GB
    QString md5;
    QString type;//文件类型
    QString absolutePath;//文件的本地绝对路径

    int pos; //上传或下载到什么位置

    int isPause; //暂停  0 1
    int timestamp;

    //文件指针
    FILE* pFile;
};




#endif // COMMON_H
