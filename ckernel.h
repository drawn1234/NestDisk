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
    void slot_getCurFileList(QString dir);
    void slot_downloadFile(int fileid,QString dir);
    void slot_downloadFolder(int fileid,QString dir);
    void slot_addFolder(QString name,QString dir);
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
};

#endif // CKERNEL_H
