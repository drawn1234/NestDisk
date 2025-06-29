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
    //最大 2G 1024(kb)*1024(mb)*1024(Gb)*2
    static QString getSize(int size){
        QString res;
        int count=0;
        int tmp=size;
        while(tmp!=0){
            tmp/=1024;
            if(tmp!=0)count++;
        }
        switch(count){
        case 0:
            res=QString("0.%1KB").arg((int)(size%1024/1024.0*100),2,10,QChar('0'));
            //arg() 参数，宽度 进制 不够宽度填充字符
            break;
        case 1:
            res=QString("%1.%2KB").arg(size/1024).arg((int)(size%1024/1024.0*100),2,10,QChar('0'));
            break;
        case 2:
            res=QString("%1.%2MB").arg(size/1024/1024).arg((int)(size/1024%1024/1024.0*100),2,10,QChar('0'));
            break;
        case 3:
            res=QString("%1.%GB").arg(size/1024/1024/1024).arg((int)(size/1024/1024%1024/1024.0*100),2,10,QChar('0'));
            break;
        default:
            res="文件过大，无法显示";
            break;
        }
        return res;
    }
};




#endif // COMMON_H
