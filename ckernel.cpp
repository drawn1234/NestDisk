#include "ckernel.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QSettings>
#include <errhandlingapi.h>
#include "md5.h"
#include "qdir.h"
#include <QThread>
#include <QEventLoop>
//定义带参数宏计算协议数
#define NetMap(a) m_netPackMap[a-_DEF_PACK_BASE]
//工具函数声明
static std::string getMD5(QString val);
static std::string getFileMd5(QString path);
void Utf8ToGB2312( char* gbbuf , int nlen ,QString& utf8);
QString GB2312ToUtf8( char* gbbuf );
CKernel::CKernel(QObject *parent)
    : QObject{parent},m_id(0),m_curDir("/"),m_quit(false)
{
    //加载配置文件
    m_ip="";
    m_port="";
    loadIniFile();
    //设置系统路径
    setSystemPtah();
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
    connect(this,SIGNAL(sig_updateUploadFileProgress(int,int)),
             m_pMainDialog,SLOT(slot_updateUploadFileProgress(int,int)));
    connect(this,SIGNAL(sig_insertFileInfo(FileInfo&)),
            m_pMainDialog,SLOT(slot_insertFileInfo(FileInfo&)));
    connect(m_pMainDialog,SIGNAL(sig_downloadFile(int,QString)),
             this,SLOT(slot_downloadFile(int,QString)));
    connect(m_pMainDialog,SIGNAL(sig_downloadFolder(int,QString)),
            this,SLOT(slot_downloadFolder(int,QString)));
    connect(this,SIGNAL(sig_updateDownloadFileProgress(int,int)),
            m_pMainDialog,SLOT(slot_updateDownloadFileProgress(int,int)));
    connect(m_pMainDialog,SIGNAL(sig_addFolder(QString,QString)),
            this,SLOT(slot_addFolder(QString,QString)));
    connect(m_pMainDialog,SIGNAL(sig_changeDir(QString)),
            this,SLOT(slot_changeDir(QString)));
    connect(m_pMainDialog,SIGNAL(sig_uploadFolder(QString,QString)),
            this,SLOT(slot_uploadFolder(QString,QString)));
    connect(m_pMainDialog,SIGNAL(sig_shareFile(QVector<int>&,QString)),
            this,SLOT(slot_shareFile(QVector<int>&,QString)));
    connect(m_pMainDialog,SIGNAL(sig_getShareByLink(QString,int)),
            this,SLOT(slot_getShareByLink(QString,int)));
    connect(m_pMainDialog,SIGNAL(sig_deleteFile(QVector<int>&,QString)),
            this,SLOT(slot_deleteFile(QVector<int>&,QString)));
    connect(m_pMainDialog,SIGNAL(sig_pauseUp(int,bool)),
        this,SLOT(slot_pauseUp(int,bool)));

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
    //避免时间戳重复
    while(m_mapTimeToFileinfo.count(timeStamp)>0){
        timeStamp++;
    }
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

void CKernel::slot_getCurFileList()
{
    qDebug()<<__func__;
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

void CKernel::slot_downloadFile(int fileid, QString dir)
{
    //下载文件请求
    qDebug()<<__func__;
    //1.下载请求
    STRU_DOWNLOAD_FILE_RQ rq;
    rq.fileid=fileid;
    string dirtmp=dir.toStdString();
    strcpy(rq.dir,dirtmp.c_str());
    int timeStamp=QDateTime::currentDateTime().toString("hhmmsszzz").toInt();
    //避免时间戳重复
    while(m_mapTimeToFileinfo.count(timeStamp)>0){
        timeStamp++;
    }
    rq.timestamp=timeStamp;
    rq.userid=m_id;
    //2.发送请求
    sendData((char*)&rq,sizeof(rq));
}

void CKernel::slot_downloadFolder(int fileid, QString dir)
{
    //下载文件夹请求
    qDebug()<<__func__;
    //1.打包
    STRU_DOWNLOAD_FOLDER_RQ rq;
    strcpy(rq.dir,dir.toUtf8().constData());
    rq.fileid=fileid;
    rq.userid=m_id;
    rq.timestamp=QDateTime::currentDateTime().toString("hhssmmzzz").toInt();
    while(m_mapTimeToFileinfo.count(rq.timestamp)>0){
        rq.timestamp++;
    }
    //2.发送请求
    sendData((char*)&rq,sizeof(rq));
}

void CKernel::slot_addFolder(QString name, QString dir)
{
    //新建文件夹
    qDebug()<<__func__;
    //1.打包数据
    STRU_ADD_FOLDER_RQ rq;
    strcpy(rq.dir,dir.toUtf8().constData());
    strcpy(rq.fileName,name.toUtf8().constData());
    strcpy(rq.time,QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toUtf8().constData());
    rq.timestamp=QDateTime::currentDateTime().toString("hhmmsszzz").toInt();
    rq.userid=m_id;
    //2.发送新建文件夹请求
    sendData((char*)&rq,sizeof(rq));
}

void CKernel::slot_changeDir(QString dir)
{
    qDebug()<<__func__;
    //更新当前目录
    m_curDir=dir;
    //刷新文件列表
    slot_getCurFileList();
}
#include <QDir>
#include <QFileInfoList>
void CKernel::slot_uploadFolder(QString path,QString dir)
{
    //上传文件夹
    qDebug()<<__func__;
    //1.处理当前文件夹 新建文件夹
    QFileInfo qfile(path);
    QDir dr(path);
    slot_addFolder(qfile.fileName(),dir);
    //qDebug()<<"folder:"<<qfile.fileName()<<"dir:"<<dir;

    //2.获取文件夹下一层 所有文件路径
    QFileInfoList lst=dr.entryInfoList();
    //遍历所有文件
    QString newDir=dir+qfile.fileName()+"/";//进入文件夹路径
    for(int i=0;i<lst.size();i++){
        QFileInfo info=lst.at(i);
        //如果是. 继续
        if(info.fileName()==".")continue;
        //如果是.. 继续
        if(info.fileName()=="..")continue;

        //如果是文件夹 slot_uploadFolder递归
        if(info.isDir()){
            slot_uploadFolder(info.absoluteFilePath(),newDir);
        }else{
            //如果是文件 uploadFile 路径-文件的绝对路径 传到的路径dir
           // qDebug()<<"file path:"<<info.absoluteFilePath()<<"file dir:"<<newDir;
            slot_uploadFile(info.absoluteFilePath(),newDir);
        }
    }
}

void CKernel::slot_shareFile(QVector<int> &fileidArr, QString dir)
{
    //分享文件
    qDebug()<<__func__;
    //1.打包数据
    int packLen=sizeof(STRU_SHARE_FILE_RQ)+sizeof(int)*fileidArr.size();
    STRU_SHARE_FILE_RQ* rq=(STRU_SHARE_FILE_RQ*)malloc(packLen);
    rq->init();
    strcpy(rq->dir,dir.toUtf8().constData());
    rq->itemCount=fileidArr.size();
    rq->userid=m_id;
    QString shaerTime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    strcpy(rq->shareTime,shaerTime.toStdString().c_str());
    for(int i=0;i<fileidArr.size();i++){
        rq->fileidArray[i]=fileidArr[i];
    }
    //2.发送请求
    sendData((char*)rq,packLen);
    free(rq);
}

void CKernel::slot_getShareList()
{
    //获取分享列表进行刷新
    qDebug()<<__func__;
    //1.打包数据
    STRU_MY_SHARE_RQ rq;
    rq.userid=m_id;
    //2.发送请求
    sendData((char*)&rq,sizeof(rq));
}

void CKernel::slot_getShareByLink(QString dir, int link)
{
    //根据分享码获取文件
    qDebug()<<__func__;
    //1.打包数据
    STRU_GET_SHARE_RQ rq;
    rq.userid=m_id;
    rq.shareLink=link;
    strcpy(rq.dir,dir.toUtf8().constData());
    QString time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    strcpy(rq.time,time.toStdString().c_str());
    //2.发送分享请求
    sendData((char*)&rq,sizeof(rq));
}

void CKernel::slot_deleteFile(QVector<int>& fileidArr,QString dir)
{
    //删除文件
    qDebug()<<__func__;
    //1.打包数据
    int packLen=sizeof(STRU_DELETE_FILE_RQ)+sizeof(int)*fileidArr.size();
    STRU_DELETE_FILE_RQ* rq=(STRU_DELETE_FILE_RQ*)malloc(packLen);
    rq->init();
    strcpy(rq->dir,dir.toUtf8().constData());
    rq->fileCount=fileidArr.size();
    rq->userid=m_id;
    for(int i=0;i<fileidArr.size();i++){
        rq->fileidArray[i]=fileidArr[i];
    }
    //2.发送请求
    sendData((char*)rq,packLen);
    free(rq);
}

void CKernel::slot_pauseUp(int timeStamp,bool isPause)
{
    //ispause 1 从正在上传变成暂停 isPause 0 从暂停 变为继续下载
    qDebug()<<__func__;
    //找到文件信息结构体
    //1.map有-程序未退出-用户暂停 直接置位
    if(m_mapTimeToFileinfo.count(timeStamp)>0){
        m_mapTimeToFileinfo[timeStamp].isPause=isPause;
    }
    //2.map没有-程序异常退出 断点续传，使用协议
    else{
        //断点续传
    }

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
        slot_getCurFileList();
        //分享列表刷新显示
        slot_getShareList();
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

    //判断是否暂停
    while(file.isPause){
        //方法1：sleep(1000);
        //方法2：使用QT线程类进行睡眠-ms
        //sleep函数在主线程中循环会影响主线程执行，应该将函数放在子线程中执行
        //为了避免阻塞窗口线程，影响时间循环，加入下面处理 将信号取出并执行
        //每处理100ms该事件，取出其它累积事件处理
        QThread::msleep(50);
        QCoreApplication::processEvents(QEventLoop::AllEvents,50);
        //如果程序退出，该循环一直存在，为了避免-添加标志位-退出线程
        if(m_quit)return;
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
        Q_EMIT sig_updateUploadFileProgress(file.timestamp,file.pos);//时间戳判断文件信息
        //判断是否结束
        if(file.pos>=file.size){
            if(file.dir==m_curDir){
                //刷新列表
                slot_getCurFileList();
            }
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
    //在插入列表之前删除原列表
    m_pMainDialog->slot_deleteAllFileInfo();
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

void CKernel::slot_dealFileHeadRq(uint from, char *data, int len)
{
    //处理文件头请求
    qDebug()<<__func__;
    //1.拆包
    STRU_FILE_HEADER_RQ* rq=(STRU_FILE_HEADER_RQ*)data;
    STRU_FILE_HEADER_RS rs;
    //2.保存文件信息
    FileInfo file;
    file.fileid=rq->fileid;
    file.name=rq->fileName;
    file.type=rq->fileType;
    file.dir=rq->dir;
    file.md5=rq->md5;
    file.size=rq->size;
    file.timestamp=rq->timestamp;
    file.time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //默认路径 sysPath（不含最后的"/"）+dir+name
    file.absolutePath=QString("%1%2%3").arg(m_sysPath).arg(rq->dir).arg(rq->fileName);
    //dir 可能有多层，需要循环创建目录 TODO:如果不循环创建路径，路径不存在会导致文件打开失败
    QString tmpDir=file.dir;    // /NetDisk/11
    QStringList dirList=tmpDir.split("/");  //分割函数 NetDisk 11
    QString curPath=m_sysPath;
    bool res=false;
    for(QString& node:dirList){
        if(!node.isEmpty()){
            curPath+="/";
            curPath+=node;
            QDir dir;
            if(!dir.exists(curPath)){
                res=dir.mkdir(curPath);
                if(!res){
                    qDebug() << "创建目录失败:" << curPath << "错误:" << strerror(errno);
                }
            }
        }
    }
    // //方法2：直接适用QDir创建
    // QFileInfo fileInfo(file.absolutePath);
    // // 获取文件所在目录（不包含文件名）
    // QString dirPath = fileInfo.absolutePath();

    // // 创建目录（包括所有不存在的父目录）
    // bool success = QDir().mkpath(dirPath);
    // if (!success) {
    //     qDebug() << "创建目录失败:" << dirPath;
    //     return; // 或其他错误处理
    // }
    //3.打开文件-二进制文本形式打开
    char pathbuf[1000]="";
    Utf8ToGB2312(pathbuf,1000,file.absolutePath);
    file.pFile=fopen(pathbuf,"wb+");
    if(!file.pFile){
        qDebug() << "打开文件失败:" << file.absolutePath << "错误:" << strerror(errno);
        return;
    }
    //保存下载信息到空间 TODO:
    m_pMainDialog->slot_insertDownloadFile(file);
    //4.保存到map

    m_mapTimeToFileinfo[rq->timestamp]=file;
    //5.发送文件头回复
    rs.fileid=rq->fileid;
    rs.timestamp=rq->timestamp;
    rs.userid=m_id;
    rs.result=1;
    sendData((char*)&rs,sizeof(rs));
}

void CKernel::slot_dealContentFileRq(uint from, char *data, int len)
{
    //处理下载的文件内容请求
    qDebug()<<__func__;
    //1.拆包
    STRU_FILE_CONTENT_RQ* rq=(STRU_FILE_CONTENT_RQ*)data;
    STRU_FILE_CONTENT_RS rs;
    //2.读取文件信息
    FileInfo& file=m_mapTimeToFileinfo[rq->timestamp];
    if(m_mapTimeToFileinfo.count(rq->timestamp)<=0){
        qDebug()<<"文件信息为空";
        return;
    }
    //判断是否暂停
    while(file.isPause){
        //方法1：sleep(1000);
        //方法2：使用QT线程类进行睡眠-ms
        //sleep函数在主线程中循环会影响主线程执行，应该将函数放在子线程中执行
        //为了避免阻塞窗口线程，影响时间循环，加入下面处理 将信号取出并执行
        //每处理100ms该事件，取出其它累积事件处理
        QThread::msleep(50);
        QCoreApplication::processEvents(QEventLoop::AllEvents,50);
        if(m_quit)return;
    }

    //3.写入文件
    int wlen=fwrite(rq->content,1,rq->len,file.pFile);
    if(wlen!=rq->len){
        //失败 游标回跳
        fseek(file.pFile,-1*wlen,SEEK_CUR);
        rs.result=false;
        qDebug()<<"写入文件失败";

    }else{
        //成功 更新pos
        rs.result=true;
        file.pos+=wlen;
        //更新下载进度-进度检查决定控件删除
        Q_EMIT sig_updateDownloadFileProgress(file.timestamp,file.pos);
        //如果到达末尾
        if(file.pos>=file.size){
            //关闭文件，回收map节点
            fclose(file.pFile);
            m_mapTimeToFileinfo.erase(file.timestamp);
            return;
        }
   }
    //4.发送结果
   rs.fileid=rq->fileid;
   rs.len=rq->len;
   rs.timestamp=rq->timestamp;
   rs.userid=rq->userid;
   sendData((char*)&rs,sizeof(rs));
}

void CKernel::slot_dealAddFolderRs(uint from, char *data, int len)
{
    //处理新建文件夹请求
    qDebug()<<__func__;
    //1.拆包
    STRU_ADD_FOLDER_RS* rs=(STRU_ADD_FOLDER_RS*)data;
    //2.判断结果
    if(rs->result==false){
        QMessageBox::about(m_pMainDialog,"提示","服务器问题，新建文件失败");
        return;
    }
    //3.刷新文件列表
    slot_getCurFileList();
}

void CKernel::slot_dealQuickUploadRs(uint from, char *data, int len)
{
    //处理文件秒传
    qDebug()<<__func__;
    //1.拆包
    STRU_QUICK_UPLOAD_RS* rs=(STRU_QUICK_UPLOAD_RS*)data;
    //2.判断结果-成功
    if(rs->result==false)return;
    //3.获取文件信息
    if(m_mapTimeToFileinfo.count(rs->timestamp)==0)return;
    FileInfo& file=m_mapTimeToFileinfo[rs->timestamp];
    //4.加入上传完成列表
    m_pMainDialog->slot_insertTbComplete(file,_DEF_UPLOAD);
    //5.刷新当前列表
    if(m_curDir==file.dir){//是当前目录就刷新
        slot_getCurFileList();
    }
    //6.关闭文件信息 删除节点
    fclose(file.pFile);
    m_mapTimeToFileinfo.erase(rs->timestamp);

}

void CKernel::slot_dealShareFileRs(uint from, char *data, int len)
{
    //分享文件回复
    qDebug()<<__func__;
    //1.拆包
    STRU_SHARE_FILE_RS* rs=(STRU_SHARE_FILE_RS*)data;
    //2.刷新分享列表
    slot_getShareList();
}

void CKernel::slot_dealGetShareListRs(uint from, char *data, int len)
{
    //添加文件列表回复
    qDebug()<<__func__;
    //1.拆包
    STRU_MY_SHARE_RS* rs=(STRU_MY_SHARE_RS*)data;
    STRU_MY_SHARE_FILE* shareList=rs->items;
    int listCount=rs->itemCount;
    //2.删除所有分享列表
    m_pMainDialog->slot_deleteAllShare();
    //3.插入所有列表
    m_pMainDialog->slot_insertAllShare(shareList,listCount);
}

void CKernel::slot_dealgetShareByLinkRs(uint from, char *data, int len)
{
    //根据分享码获取文件回复
    qDebug()<<__func__;
    //1.拆包
    STRU_GET_SHARE_RS* rs=(STRU_GET_SHARE_RS*)data;
    //2.判断结果
    if(rs->result==false){
        QMessageBox::about(m_pMainDialog,"提示","分享码不存在");
        return;
    }
    //3.刷新当前文件列表
    if(rs->dir==m_curDir)slot_getCurFileList();
}

void CKernel::slot_dealAddFolderRq(uint from, char *data, int len)
{
    qDebug()<<__func__;
    //1.拆包
    STRU_ADD_FOLDER_RQ* rq=(STRU_ADD_FOLDER_RQ*)data;
    QString dir=rq->dir;

    //2.创建路径
    QString tmpDir=dir;    // /NetDisk/11
    QStringList dirList=tmpDir.split("/");  //分割函数 NetDisk 11
    QString curPath=m_sysPath;
    bool res=false;
    for(QString& node:dirList){
        if(!node.isEmpty()){
            curPath+="/";
            curPath+=node;
            QDir dir;
            if(!dir.exists(curPath)){
                res=dir.mkdir(curPath);
                if(!res){
                    qDebug() << "创建目录失败:" << curPath << "错误:" << strerror(errno);
                }
            }
        }
    }
}

void CKernel::slot_dealDeleteFileRs(uint from, char *data, int len)
{
    //处理删除文件回复
    qDebug()<<__func__;
    //分享文件回复
    qDebug()<<__func__;
    //1.拆包
    STRU_GET_SHARE_RS* rs=(STRU_GET_SHARE_RS*)data;
    //2.刷新文件列表
    if(rs->result==true&&rs->dir==m_curDir){
        slot_getCurFileList();
    }
    //3.待做 刷新回收站列表
    //slot_getDeleteList();
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
    NetMap(_DEF_PACK_FILE_HEADER_RQ)=&CKernel::slot_dealFileHeadRq;
    NetMap(_DEF_PACK_FILE_CONTENT_RQ)=&CKernel::slot_dealContentFileRq;
    NetMap(_DEF_PACK_ADD_FOLDER_RS)=&CKernel::slot_dealAddFolderRs;
    NetMap(_DEF_PACK_QUICK_UPLOAD_RS)=&CKernel::slot_dealQuickUploadRs;
    NetMap(_DEF_PACK_SHARE_FILE_RS)=&CKernel::slot_dealShareFileRs;
    NetMap(_DEF_PACK_MY_SHARE_RS)=&CKernel::slot_dealGetShareListRs;
    NetMap(_DEF_PACK_GET_SHARE_RS)=&CKernel::slot_dealgetShareByLinkRs;
    NetMap(_DEF_PACK_ADD_FOLDER_RQ)=&CKernel::slot_dealAddFolderRq;
    NetMap(_DEF_PACK_DELETE_FILE_RS)=&CKernel::slot_dealDeleteFileRs;

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

//系统路径组成：exe统计 ./NetDisk
#include <QDir>
#include <QCoreApplication>
void CKernel::setSystemPtah()
{
    qDebug()<<__func__;
    //设置系统路径
    QString path=QCoreApplication::applicationDirPath()+"/NetDisk";
    QDir dir;
    //没有文件夹 创建
    if(!dir.exists(path)){
        dir.mkdir(path);
    }
    //默认路径
    m_sysPath=path;
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
