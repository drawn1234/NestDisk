#include "ckernel.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QSettings>

CKernel::CKernel(QObject *parent)
    : QObject{parent}
{
    //加载配置文件
    m_ip="127.0.0.1";
    m_port="9876";
    loadIniFile();
    //创建网络中介者
    m_pClient=new TcpClientMediator;
    m_pServer=new TcpServerMediator;

    //开启服务器网络
    m_pServer->OpenNet();
    //客户端连接真实地址
    m_pClient->OpenNet("192.168.137.1");

    //网络信号连接
    connect(m_pClient,SIGNAL(SIG_ReadyData(uint,char*,int)),
            this,SLOT(slot_dealClientData(uint,char*,int)));
    connect(m_pServer,SIGNAL(SIG_ReadyData(uint,char*,int)),
            this,SLOT(slot_dealServerData(uint,char*,int)));

    //创建窗口对象，显示
    m_pMainDialog=new MainDialog;
     connect(m_pMainDialog,SIGNAL(sig_close()),this,SLOT(slot_closeMainDialog()));
    m_pMainDialog->show();


    //发送数据测试给服务器
    char buf[]="hello server";
    m_pClient->SendData(0,buf,sizeof(buf));
    //长度输出函数-sizeof(数组名)-数组长度
    //strlen(0)+1


}

CKernel::~CKernel()
{

}

void CKernel::loadIniFile()
{

    //加载配置文件
    qDebug()<<__func__;
    //获取exe目录
    QString path= QCoreApplication::applicationDirPath()+"/config.ini";
    //文件是否存在
    QFileInfo info(path);
    if(info.exists()){
        //存在-加载
        QSettings setting(path,QSettings::IniFormat);
        //打开组
        setting.beginGroup("net");
        //加载值
        QVariant strIp= setting.value("ip","");
        QVariant strPort=setting.value("port","");
        if(!strIp.toString().isEmpty())m_ip=strIp.toString();
        if(!strPort.toString().isEmpty())m_port=strPort.toString();
        //关闭组
        setting.endGroup();
    }else{
        //不存在-创建加载
        QSettings setting(path,QSettings::IniFormat);//没有就会创建
        //打开组
        setting.beginGroup("net");
        //设置 key value
        setting.setValue("ip",m_ip);
        setting.setValue("port",m_port);
        //关闭组
        setting.endGroup();
    }
    qDebug()<<"ip:"<<m_ip<<"port:"<<m_port;
}

void CKernel::slot_closeMainDialog()
{
    //关闭窗口，回收窗口对象
    qDebug()<<__func__;
    delete m_pMainDialog;
    m_pMainDialog=nullptr;
}

void CKernel::slot_dealClientData(uint from, char *data, int len)
{
    qDebug()<<__func__;
    QString str=QString("来自服务端:%1").arg(QString::fromStdString(data));
    QMessageBox::about(NULL,"提示",str);
    //阻塞的，模态窗口-不可切换
    //回收资源
    delete data;
    data=nullptr;

}

void CKernel::slot_dealServerData(uint from, char *data, int len)
{
    qDebug()<<__func__;
    QString str=QString("来自客户端:%1").arg(QString::fromStdString(data));
    QMessageBox::about(m_pMainDialog,"提示",str);
    //阻塞的，模态窗口-不可切换
    //给客户端发送数据
    m_pServer->SendData(from,data,len);

    delete data;
    data=nullptr;
}
