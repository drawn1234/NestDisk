#include "ckernel.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QSettings>
#include <errhandlingapi.h>
#include "md5.h"

//定义带参数宏计算协议数
#define NetMap(a) m_netPackMap[a-_DEF_PACK_BASE]
//工具函数声明
static std::string getMD5(QString val);
static std::string getFileMd5(QString path);
void Utf8ToGB2312( char* gbbuf , int nlen ,QString& utf8);
QString GB2312ToUtf8( char* gbbuf );
CKernel::CKernel(QObject *parent)
    : QObject{parent},m_id(0),m_curDir("/")
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
    connect(m_pMainDialog,SIGNAL(sig_uploadFile(QString,QString)),
            this,SLOT(slot_uploadFile(QString,QString)));

     connect(this,SIGNAL(sig_updateFileProgress(int,int)),
             m_pMainDialog,SLOT(slot_updateFileProgress(int,int)));
    connect(this,SIGNAL(sig_insertFileInfo(FileInfo&)),
            m_pMainDialog,SLOT(slot_insertFileInfo(FileInfo&)));
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
//普通槽函数-------------------------------------------------------------------------------------


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

#include <QFileInfo>
#include <QDateTime>
void CKernel::slot_uploadFile(QString path, QString dir)
{
    qDebug()<<__func__;
    //1. 存储上传文件信息
    QFileInfo qFile(path);
    FileInfo file;
    file.absolutePath=path;
    file.dir=dir;
    file.md5=QString::fromStdString(getFileMd5(path));
    file.name=qFile.fileName();
    file.size=qFile.size();
    file.time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    file.type="file";
    char buf[1000]="";
    Utf8ToGB2312(buf,sizeof(buf),path);
    FILE* pFile=fopen(buf,"rb");
    if(!pFile){
        qDebug()<<"打开文件失败";
        return;
    }
    file.pFile=pFile;
    //2. 上传文件信息保留到map
    int timeStamp=QDateTime::currentDateTime().toString("hhmmsszzz").toInt();
    file.timestamp=timeStamp;
    m_mapTimeToFileinfo[timeStamp]=file;
    //3. 发送上传文件请求给服务器
    STRU_UPLOAD_FILE_RQ rq;

    //需要进行中文兼容转码，将数据转换为string，拷贝进char[]
    string strName=file.name.toStdString();
    string strdir=file.dir.toStdString();
    string strtype=file.type.toStdString();
    strcpy(rq.fileName,strName.c_str());
    strcpy(rq.dir,strdir.c_str());
    strcpy(rq.fileType,strtype.c_str());
    strcpy(rq.time,file.time.toStdString().c_str());
    strcpy( rq.md5,file.md5.toStdString().c_str());
    rq.size=file.size;
    rq.timestamp=timeStamp;
    rq.userid=m_id;
    //4. 发送上传文件请求给服务器
    sendData((char*)&rq,sizeof(rq));
}

void CKernel::slot_getCurFileList(QString dir)
{
    //获取当前文件列表
    //1.获取文件列表请求
    STRU_GET_FILE_RQ rq;
    rq.userid=m_id;
    //2. 兼容中文
    std::string stddir=m_curDir.toStdString();
    strcpy(rq.dir,stddir.c_str());
    //3. 发送请求
    sendData((char*)&rq,sizeof(rq));
}


//信息处理函数-------------------------------------------------------------------------------
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
        m_id=rs->userid;
        m_name=rs->name;
        //获取根目录下文件列表
        m_curDir="/";
        slot_getCurFileList(m_curDir);
        break;
    }
}

void CKernel::slot_dealUploadFileRs(uint from, char *data, int len)
{
    qDebug()<<__func__;
    //处理上传文件回复
    //1. 拆包
    STRU_UPLOAD_FILE_RS* rs=(STRU_UPLOAD_FILE_RS*)data;
    //2. 判断结果
    if(rs->result==false){
        qDebug()<<"上传文件失败";
    }
    //3. 获取文件信息
    if( m_mapTimeToFileinfo.count(rs->timestamp)==0){
        qDebug()<<"没有对应文件信息";
        return;
    }
     FileInfo& file=m_mapTimeToFileinfo[rs->timestamp];
    //4. 重新设置fid值
     file.fileid=rs->fileid;
    //5.加载上传信息到上传控件 TODO:
     m_pMainDialog->slot_insertUploadFile(file);
    //6.发送文件块请求
     STRU_FILE_CONTENT_RQ rq;
     rq.fileid=rs->fileid;
     rq.timestamp=rs->timestamp;
     rq.userid=m_id;
     rq.len=fread(rq.content,1,_DEF_BUFFER,file.pFile);
     sendData((char*)&rq,sizeof(rq));
}

void CKernel::slot_dealContentFileRs(uint from, char *data, int len)
{
    qDebug()<<__func__;
    //1.拆包
    STRU_FILE_CONTENT_RS* rs=(STRU_FILE_CONTENT_RS*)data;
    STRU_FILE_CONTENT_RQ rq;
    //2.查看结果
    FileInfo& file=m_mapTimeToFileinfo[rs->timestamp];
    if(m_mapTimeToFileinfo.count(rs->timestamp)==0){
        qDebug()<<"没有对应文件信息";
        return;
    }
    if(rs->result==false){
        //跳回原位置
        fseek(file.pFile,-1*rs->len,SEEK_CUR);
    }else{
        //3.更新文件信息
        file.pos+=rs->len;
        //更新上传进度
        //方法1：信号槽控制-多线程
        //方法2：直接调用 一定是当前函数在主线程
        Q_EMIT sig_updateFileProgress(file.timestamp,file.pos);//时间戳判断文件信息
        //判断是否结束
        if(file.pos>=file.size){
            //关闭文件
            fclose(file.pFile);
            m_mapTimeToFileinfo.erase(rs->timestamp);
            return;
        }
    }
    //4.发送文件块请求
    rq.fileid=rs->fileid;
    rq.timestamp=rs->timestamp;
    rq.userid=rs->userid;
    rq.len=fread(rq.content,1,_DEF_BUFFER,file.pFile);
    sendData((char*)&rq,sizeof(rq));
}

void CKernel::slot_dealGetListRs(uint from, char *data, int len)
{
    qDebug()<<__func__;
    //处理获取文件列表回复
    //1. 拆包
    STRU_GET_FILE_RS* rs=(STRU_GET_FILE_RS*)data;
    if(m_curDir!=QString::fromStdString(rs->dir))return;
    //2. 处理每个列表文件数据
    FileInfo file;
    for(int i=0;i<rs->count;i++){
        file.type=rs->fileInfo[i].fileType;
        file.name=rs->fileInfo[i].name;
        file.size=rs->fileInfo[i].size;
        file.time=rs->fileInfo[i].time;
        file.fileid=rs->fileInfo[i].fileid;
        //3. 插入文件列表
        //发送信号通知界面类处理
        Q_EMIT sig_insertFileInfo(file);
    }
}


//工具函数------------------------------------------------------------------------

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

void CKernel::setNetPackMap()
{
    qDebug()<<__func__;
    //清空协议处理数组
    memset(m_netPackMap,0,sizeof(PFUN)*_DEF_PACK_COUNT);

    //协议映射表 key 协议偏移量 value 函数指针
    //通过协议头找到对应处理函数
    NetMap(_DEF_PACK_LOGIN_RS)=&CKernel::slot_dealLoginRs;
    NetMap(_DEF_PACK_REGISTER_RS)=&CKernel::slot_dealRegisterRs;
    NetMap(_DEF_PACK_UPLOAD_FILE_RS)=&CKernel::slot_dealUploadFileRs;
    NetMap(_DEF_PACK_FILE_CONTENT_RS)=&CKernel::slot_dealContentFileRs;
    NetMap(_DEF_PACK_FILE_LIST_RS)=&CKernel::slot_dealGetListRs;

}

#include<QTextCodec>

// QString -> char* gb2312
void Utf8ToGB2312( char* gbbuf , int nlen ,QString& utf8)
{
    qDebug()<<__func__;
    //转码的对象
    QTextCodec * gb2312code = QTextCodec::codecForName( "gb2312");
    //QByteArray char 类型数组的封装类 里面有很多关于转码 和 写IO的操作
    QByteArray ba = gb2312code->fromUnicode( utf8 );// Unicode -> 转码对象的字符集

    strcpy_s ( gbbuf , nlen , ba.data() );
}

// char* gb2312 --> QString utf8
QString GB2312ToUtf8( char* gbbuf )
{
    //转码的对象
    QTextCodec * gb2312code = QTextCodec::codecForName( "gb2312");
    //QByteArray char 类型数组的封装类 里面有很多关于转码 和 写IO的操作
    return gb2312code->toUnicode( gbbuf );// 转码对象的字符集 -> Unicode
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
    string a="111";
    QString b=QString::fromStdString(a);
    a=b.toStdString();
    return md5.toString();
}

static std::string getFileMd5(QString path){
    qDebug()<<__func__;
    //1.path转码为ANSI
    char buf[1000]="";
    Utf8ToGB2312(buf,sizeof(buf),path);
    //2.打开文件
    FILE* pFile=fopen(buf,"rb");//二进制只读
    if(!pFile){
        qDebug()<<"打开文件失败";
        return string();
    }
    //3.循环读取文件，刷新MD5
    MD5 md;
    int len=0;
    do{
        len=fread(buf,1,1000,pFile);//每次读取1byte 最多读取1000次
        //if(len<=0)qDebug()<<"读取文件失败";
        md.update(buf,len);
    }while(len>0);
    int err=fclose(pFile);
    //4.返回MD5值
    qDebug()<<"文件MD5:"<<md.toString();
    return md.toString();
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
