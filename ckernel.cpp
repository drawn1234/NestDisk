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
    //创建窗口对象，显示
    m_pMainDialog=new MainDialog;
    m_pMainDialog->show();
    connect(m_pMainDialog,SIGNAL(sig_close()),this,SLOT(slot_closeMainDialog()));
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
