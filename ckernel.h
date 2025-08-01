#ifndef CKERNEL_H
#define CKERNEL_H

#include <QObject>
#include "maindialog.h"
#include <qdebug.h>
#include <TcpClientMediator.h>
#include <TcpServerMediator.h>
#include <QMessageBox>
#include<packdef.h>
#include"logindialog.h"
#include "common.h"
#include "csqlite.h"
#include "playerdialog.h"
//协议映射表
//类成员函数指针
class CKernel;
typedef void (CKernel::*PFUN)(uint from,char* data,int len);


//#define USE_SERVER 1
class INetMediator;
class CKernel : public QObject
{
    Q_OBJECT
private:
    explicit CKernel(QObject *parent = nullptr);
    //私有拷贝构造
    explicit CKernel(const CKernel & kernel){}
    ~CKernel();

signals:
    void sig_updateUploadFileProgress(int timestamp,int pos);
    void sig_updateDownloadFileProgress(int timestamp,int pos);
    void sig_insertFileInfo(FileInfo& file);

public:
    //获取对象的静态方法-全局创建/堆区创建
    static CKernel* GetInstance(){
        //全局创建变量
        static CKernel kernel;
        return &kernel;//在全局区，调用的时候创建一次
        //线程安全，系统自动回收
    }
private slots:
    //普通槽函数
    void slot_closeMainDialog();
    void slot_registerCommit(QString tel,QString pass,QString name);
    void slot_loginCommit(QString tel,QString pass);
    void slot_uploadFile(QString path,QString dir);
    void slot_getCurFileList();
    void slot_downloadFile(int fileid,QString dir);
    void slot_downloadFolder(int fileid,QString dir);
    void slot_addFolder(QString name,QString dir);
    void slot_changeDir(QString dir);
    void slot_uploadFolder(QString path,QString dir);
    void slot_shareFile(QVector<int>& fileidArr,QString dir);
    void slot_getShareList();
    void slot_getShareByLink(QString dir,int link);
    void slot_deleteFile(QVector<int>& fileidArr,QString dir);
    void slot_pauseUp(int timeStamp,bool isPause);
    void slot_pauseDown(int timeStamp,bool isPause);
    void slot_playVideo(int fileId,QString dir);

    //网络槽函数
    //客户端处理接收的数据
    void slot_dealClientData(uint from,char* data,int len);
    void slot_dealRegisterRs(uint from,char* data,int len);
    void slot_dealLoginRs(uint from,char* data,int len);
    void slot_dealUploadFileRs(uint from,char* data,int len);
    void slot_dealContentFileRs(uint from,char* data,int len);
    void slot_dealGetListRs(uint from,char* data,int len);
    void slot_dealFileHeadRq(uint from,char* data,int len);
    void slot_dealContentFileRq(uint from,char* data,int len);
    void slot_dealAddFolderRs(uint from,char* data,int len);
    void slot_dealQuickUploadRs(uint from,char* data,int len);
    void slot_dealShareFileRs(uint from,char* data,int len);
    void slot_dealGetShareListRs(uint from,char* data,int len);
    void slot_dealgetShareByLinkRs(uint from,char* data,int len);
    void slot_dealAddFolderRq(uint from,char* data,int len);
    void slot_dealDeleteFileRs(uint from,char* data,int len);
    void slot_dealCotinueUploadRs(uint from,char* data,int len);
    void slot_dealPlayVideoRs(uint from,char* data,int len);

    //数据库操作函数
    void initDatabase(int id);
    void slot_getDownloadTask(QList<FileInfo> &infoList);
    void slot_getUploadTask(QList<FileInfo> &infoList);
    void slot_deleteDownloadTask(FileInfo &info);
    void slot_deleteUploadTask(FileInfo &info);
    void slot_writeDownloadTask(FileInfo &info);
    void slot_writeUploadTask(FileInfo &info);
#ifdef USE_SERVER
    //服务端处理数据
    void slot_dealServerData(uint from,char* data,int len);
#endif

private:
    void loadIniFile();
    void setNetPackMap();
    void sendData(char* buf,int len);
    void setSystemPtah();


private:
    MainDialog* m_pMainDialog;
    TcpClientMediator* m_pClient;
    loginDialog* m_pLoginDialog;
    CSqlite* m_sql;
    PlayerDialog* m_pVedioPlayer;


#ifdef USE_SERVER
    TcpServerMediator* m_pServer;
#endif
    //网络配置
    QString m_ip;
    QString m_port;
    //用户信息
    QString m_name;
    int m_id;
    //获取文件列表使用目录
    QString m_curDir;
    //默认的下载路径-NetDisk exe同级
    QString m_sysPath;
    //协议处理函数数组
    PFUN m_netPackMap[_DEF_PACK_COUNT];
    //时间戳-文件信息 map
    std::map<int,FileInfo> m_mapTimeToFileinfo;
    //退出标志
    bool m_quit;


};

#endif // CKERNEL_H
