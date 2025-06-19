#include "ckernel.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QSettings>
#include "md5.h"
//定义带参数宏计算协议数
#define NetMap(a) m_netPackMap[a-_DEF_PACK_BASE]
static std::string getMD5(QString val);
CKernel::CKernel(QObject *parent)
    : QObject{parent}
{
    //加载配置文件
    m_ip="";
    m_port="";
    loadIniFile();
    //创建网络中介者
    m_pClient=new TcpClientMediator;
    //客户端连接真实地址
    m_pClient->OpenNet(m_ip.toStdString().c_str(),m_port.toShort());
    //调用协议初始化
    this->setNetPackMap();
    //网络信号连接
    connect(m_pClient,SIGNAL(SIG_ReadyData(uint,char*,int)),
            this,SLOT(slot_dealClientData(uint,char*,int)));

#ifdef USE_SERVER
    //开启服务器网络
    m_pServer->OpenNet();
    m_pServer=new TcpServerMediator;
    connect(m_pServer,SIGNAL(SIG_ReadyData(uint,char*,int)),
            this,SLOT(slot_dealServerData(uint,char*,int)));
#endif

    //创建窗口对象
    m_pMainDialog=new MainDialog;
     connect(m_pMainDialog,SIGNAL(sig_close()),this,SLOT(slot_closeMainDialog()));

    //创建登录窗口并显示
    m_pLoginDialog=new loginDialog;
    m_pLoginDialog->show();
    //绑定信号与处理函数
    connect(m_pLoginDialog,SIGNAL(SIG_loginCommit(QString,QString)),this,SLOT(slot_loginCommit(QString,QString)));
    connect(m_pLoginDialog,SIGNAL(SIG_registerCommit(QString,QString,QString)),this,SLOT(slot_registerCommit(QString,QString,QString)));

#ifdef USE_SERVER
    //发送数据测试给服务器
    char buf[]="hello server";
    m_pClient->SendData(0,buf,sizeof(buf));
    //长度输出函数-sizeof(数组名)-数组长度
    //strlen(0)+1
#endif


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
    m_pClient->CloseNet();
    delete m_pClient;
    delete m_pMainDialog;
    m_pMainDialog=nullptr;
    delete m_pLoginDialog;
}

void CKernel::slot_registerCommit(QString tel, QString pass, QString name)
{
    qDebug()<<__func__;
    //处理注册数据
    //1.打包注册请求
    STRU_REGISTER_RQ rq;
    //兼容中文
    std::string strName=name.toStdString();
    strcpy(rq.name,strName.c_str());
    strcpy(rq.tel,tel.toStdString().c_str());
    //todo:密码转换为md5
    // strcpy(rq.password,pass.toStdString().c_str());
    strcpy(rq.password,getMD5(pass).c_str());

    //2. 发送给服务器
    sendData((char*)&rq,sizeof(rq));
}

void CKernel::slot_loginCommit(QString tel, QString pass)
{
    qDebug()<<__func__;
    //处理登录数据
    //1.打包注册请求
    STRU_LOGIN_RQ rq;
    strcpy(rq.tel,tel.toStdString().c_str());
    //todo:密码转换为md5
    //strcpy(rq.password,pass.toStdString().c_str());
    strcpy(rq.password,getMD5(pass).c_str());

    //2. 发送给服务器
    sendData((char*)&rq,sizeof(rq));

}

void CKernel::slot_dealClientData(uint from, char *data, int len)
{
    qDebug()<<__func__;
#ifdef USE_SERVER
    QString str=QString("来自服务端:%1").arg(QString::fromStdString(data));
    QMessageBox::about(NULL,"提示",str);
    //阻塞的，模态窗口-不可切换
#endif

    //调用处理函数
    int type =*(int*)data;
    //测试服务器回复输出
    qDebug()<<"type:"<<*(int*)data;
    //数据验证
    if(type>=_DEF_PACK_BASE&type<_DEF_PACK_BASE+_DEF_PACK_COUNT){
        PFUN pf= NetMap(type);
        if(pf){
            (this->*pf)(from,data,len);
        }
    }

    //回收资源
    delete[] data;
    data=nullptr;

}

void CKernel::slot_dealRegisterRs(uint from, char *data, int len)
{
    qDebug()<<__func__;
    //1. 拆包
    STRU_REGISTER_RS* rs=(STRU_REGISTER_RS*)data;
    //2. 判断处理结果
    switch(rs->result){
    case user_is_exist:
        QMessageBox::about(m_pLoginDialog,"提示","用户已存在,注册失败");
        break;
    case register_success:
        QMessageBox::about(m_pLoginDialog,"提示","注册成功");
        break;
    case register_error:
        QMessageBox::about(m_pLoginDialog,"提示","服务器内部原因，注册失败");
        break;
    }


}

void CKernel::slot_dealLoginRs(uint from, char *data, int len)
{
    qDebug()<<__func__;
    //1. 拆包
    STRU_LOGIN_RS* rs=(STRU_LOGIN_RS*)data;
    //2. 处理结果
    switch(rs->result){
    case user_not_exist:
        QMessageBox::about(m_pLoginDialog,"提示","用户不存在，登录失败");
        break;
    case password_error:
        QMessageBox::about(m_pLoginDialog,"提示","密码错误");
        break;
    case login_success:
        m_pLoginDialog->close();
        m_pMainDialog->show();
        break;
    }
}




//绑定协议处理函数
void CKernel::setNetPackMap()
{
    qDebug()<<__func__;
    //清空协议处理数组
    memset(m_netPackMap,0,sizeof(PFUN)*_DEF_PACK_COUNT);

    //协议映射表 key 协议偏移量 value 函数指针
    //通过协议头找到对应处理函数
    NetMap(_DEF_PACK_LOGIN_RS)=&CKernel::slot_dealLoginRs;
    NetMap(_DEF_PACK_REGISTER_RS)=&CKernel::slot_dealRegisterRs;
}

#define MD5_KEY "1234"
//生成MD5函数
//规定给输入的明文，加上对应的类型key值，以val_key的形式给明文加盐
//使用加盐后的明文生成MD5值
static std::string getMD5(QString val){
    qDebug()<<__func__;
    //static限制当前文件可用
    QString str=QString("%1_%2").arg(val).arg(MD5_KEY);
    MD5 md5(str.toStdString().c_str());
    qDebug()<<str<<"对应MD5"<<md5.toString().c_str();
    return md5.toString();
}

void CKernel::sendData(char* buf,int len)
{
    qDebug()<<__func__;
    //发送消息
    m_pClient->SendData(0,buf,len);
}

#ifdef USE_SERVER
void CKernel::slot_dealServerData(uint from, char *data, int len)
{
    qDebug()<<__func__;
    QString str=QString("来自客户端:%1").arg(QString::fromStdString(data));
    QMessageBox::about(m_pMainDialog,"提示",str);
    //阻塞的，模态窗口-不可切换
    //给客户端发送数据
    m_pServer->SendData(from,data,len);

    delete[] data;
    data=nullptr;
}
#endif
