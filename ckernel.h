#ifndef CKERNEL_H
#define CKERNEL_H

#include <QObject>
#include "maindialog.h"
#include <qdebug.h>
#include <TcpClientMediator.h>.
#include <TcpServerMediator.h>
#include <QMessageBox>
class INetMediator;
class CKernel : public QObject
{
    Q_OBJECT
private:
    explicit CKernel(QObject *parent = nullptr);
    //私有拷贝构造
    explicit CKernel(const CKernel & kernel){}
    ~CKernel();
    void loadIniFile();
signals:

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
    //网络槽函数
    //客户端处理接收的数据
    void slot_dealClientData(uint from,char* data,int len);
    //服务端处理数据
    void slot_dealServerData(uint from,char* data,int len);
private:
    MainDialog* m_pMainDialog;
    TcpClientMediator* m_pClient;
    TcpServerMediator* m_pServer;
    QString m_ip;
    QString m_port;
};

#endif // CKERNEL_H
