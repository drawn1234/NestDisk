#include "clogic.h"
void CLogic::setNetPackMap()
{
    NetPackMap(_DEF_PACK_REGISTER_RQ)    = &CLogic::RegisterRq;
    NetPackMap(_DEF_PACK_LOGIN_RQ)       = &CLogic::LoginRq;
    NetPackMap(_DEF_PACK_UPLOAD_FILE_RQ) = &CLogic::uploadFile;
    NetPackMap(_DEF_PACK_FILE_CONTENT_RQ) = &CLogic::fileContentRq;
    NetPackMap(_DEF_PACK_FILE_LIST_RQ) = &CLogic::getFileList;
    NetPackMap(_DEF_PACK_DOWNLOAD_FILE_RQ) = &CLogic::downloadFile;
    NetPackMap(_DEF_PACK_DOWNLOAD_FOLDER_RQ) = &CLogic::downloadFileFolder;
    NetPackMap(_DEF_PACK_FILE_HEADER_RS) = &CLogic::downloadFileHeadRs;
    NetPackMap(_DEF_PACK_FILE_CONTENT_RS) = &CLogic::fileContentRs;
    NetPackMap(_DEF_PACK_ADD_FOLDER_RQ) = &CLogic::addFolder;
    NetPackMap(_DEF_PACK_SHARE_FILE_RQ) = &CLogic::shareFile;
    NetPackMap(_DEF_PACK_MY_SHARE_RQ) = &CLogic::getShareList;
    NetPackMap(_DEF_PACK_GET_SHARE_RQ) = &CLogic::getShareByLink;
    NetPackMap(_DEF_PACK_DELETE_FILE_RQ) = &CLogic::deleteFile;

}

long CLogic::number()
{
    return 1000000000;
}

#define _DEF_COUT_FUNC_    cout << "clientfd:"<< clientfd <<" "<< __func__ << endl;

#define _DEF_PATH "/home/xx/Node/NetDisk/"
//注册
void CLogic::RegisterRq(sock_fd clientfd,char* szbuf,int nlen)
{
    _DEF_COUT_FUNC_
   //1. 拆包
    STRU_REGISTER_RQ* rq=(STRU_REGISTER_RQ*)szbuf;
    STRU_REGISTER_RS rs;
    //2. 查询数据
    char sql[_DEF_CONTENT_SIZE]="";
    list<string> strlst;
    sprintf(sql,"select u_tel from t_user where u_tel='%s';",rq->tel);
    bool res=m_sql->SelectMysql(sql,1,strlst);
    if(!res){
        std::cout << "select fail: " << sql << std::endl;
        rs.result=register_error;
        m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));
        return;
    }
   //2.1 电话号是否已注册
    if(strlst.size()!=0){
        rs.result=user_is_exist;
    }else{
        //1.插入数据
        sprintf(sql,"insert into t_user(u_name,u_tel,u_password) values('%s','%s','%s');",rq->name,rq->tel,rq->password);
        res=m_sql->UpdataMysql(sql);
        if(!res){
            std::cout<<"update fail:"<<sql<<std::endl;
            rs.result=register_error;
            m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));
            return;
           }
        //2. 查询用户id
        strlst.clear();
        sprintf(sql,"select u_id from t_user where u_tel='%s';",rq->tel);
        res=m_sql->SelectMysql(sql,1,strlst);
        if(!res){
            rs.result=register_error;
            m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));
            return;
        }
        if(strlst.size()!=0){
            //3.根据id 创建本地用户专属目录
            int id=stoi(strlst.front());
            strlst.pop_front();
            char path[_MAX_PATH_SIZE]="";
            sprintf(path,"%s%d/",_DEF_PATH,id);
            //创建路径
            umask(0);
            if (mkdir(path, 0777) == -1) {
                std::cout << "mkdir fail: " << path << ", error: " << strerror(errno) << std::endl;
                rs.result = register_error;
                m_tcp->SendData(clientfd, (char*)&rs, sizeof(rs));
                return;
            }
        }
        rs.result=register_success;
    }
   //3.返回注册结果
    SendData(clientfd,(char*)&rs,sizeof(rs));
}

//登录
void CLogic::LoginRq(sock_fd clientfd ,char* szbuf,int nlen)
{
    _DEF_COUT_FUNC_
    //1. 拆包
    STRU_LOGIN_RQ* rq=(STRU_LOGIN_RQ*)szbuf;
    STRU_LOGIN_RS rs;
    //2. 判断
    char sql[_DEF_CONTENT_SIZE]="";
    sprintf(sql,"select u_password,u_id,u_name from t_user where u_tel='%s'",rq->tel);
    list<string> lststr;
    bool res=m_sql->SelectMysql(sql,3,lststr);
    if(!res){
        std::cout<<"select fail:"<<sql<<std::endl;
    }
    if(lststr.size()!=0){
       string pass=lststr.front();
       //std::cout<<"password:"<<pass<<std::endl;
       lststr.pop_front();
       int id=stoi(lststr.front());
       lststr.pop_front();
       string name=lststr.front();
       lststr.pop_front();
       if(rq->password!=pass){
           rs.result=password_error;
           SendData(clientfd,(char*)&rs,sizeof(rs));
           return ;
       }else{
           rs.result=login_success;
           rs.userid=id;
           strcpy(rs.name,name.c_str());

           //将用户信息存入映射表
           STRU_USERINFO* info=nullptr;
           if(!m_mapIdToUserinfo.find(id,info)){
                info=new STRU_USERINFO;
           }else{
               //考虑将用户下线
           }
           strcpy(info->name,name.c_str());
           info->userid=id;
           info->clientfd=clientfd;

           m_mapIdToUserinfo.insert(id,info);
       }
    }else{
        rs.result=user_not_exist;
    }
    //3. 发送消息
    SendData(clientfd,(char*)&rs,sizeof(rs));
}

//上传文件
void CLogic::uploadFile(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //1.拆包
    STRU_UPLOAD_FILE_RQ* rq=(STRU_UPLOAD_FILE_RQ*)szbuf;
    FileInfo* file;
    //2.是否秒传
    //根据md5 state=1 查数据库 得到f_id
    list<string> lststr;
    char sql[1024]="";
    sprintf(sql,"select f_id from t_file where f_MD5='%s'and f_state=1;",rq->md5);
    bool res=m_sql->SelectMysql(sql,1,lststr);
    if(!res){
        printf("select fileid error:%s\n",sql);
        return;
    }
    int fid;
    //是秒传
    if(lststr.size()>0){
        //记录fid
        fid=stoi(lststr.front());
        lststr.pop_front();
        //写入用户文件关系
        sprintf(sql,"insert into t_user_file(u_id,f_id,f_dir,f_name,f_uploadTime) values('%d','%d','%s','%s','%s');",
                rq->userid,fid,rq->dir,rq->fileName,rq->time);
        res=m_sql->UpdataMysql(sql);
        if(!res){
            printf("insert file-user error:%s\n",sql);
            return;
        }
        //发送秒传回复包
        STRU_QUICK_UPLOAD_RS rs;
        rs.fileid=fid;
        rs.userid=rq->userid;
        rs.timestamp=rq->timestamp;
        rs.result=true;

        SendData(clientfd,(char*)&rs,sizeof(rs));
        return;
    }
    //3.创建文件信息
    //拼接文件路径
    char cPath[1000]="";
    //使用md5作为服务器中的文件名，避免文件重复
    sprintf(cPath,"%s%d%s%s",_DEF_PATH,rq->userid,rq->dir,rq->md5);//_DEF_PATH+userid+dir+md5

    file=new FileInfo;
    file->dir=rq->dir;
    file->md5=rq->md5;
    file->name=rq->fileName;
    file->size=rq->size;
    file->time=rq->time;
    file->type=rq->fileType;
    file->fileFd=open(cPath,O_CREAT|O_WRONLY|O_TRUNC,0777); //使用linux创建打开文件-读写，创建，清空
    if(file->fileFd==-1){
        perror("文件打开失败");
        return;
    }
    file->fid;
    file->absolutePath=cPath;
    //4.map存储文件信息
    //使用用户id+时间戳存储文件信息userid*1000 000 000 +timestamp
    //1000 000 000使用宏定义会发生截断
    int64_t user_time=rq->timestamp+rq->userid*number();
    m_mapTimstampToFileinfo.insert(user_time,file);
    //5. 数据库操作
        //1. 插入文件信息
        sprintf(sql,"insert into t_file(f_size,f_path,f_md5,f_count,f_state,f_type) values('%d','%s','%s',0,0,'%s');",
                file->size,cPath,file->md5.c_str(),file->type.c_str());
        res=m_sql->UpdataMysql(sql);
        if(!res){
            printf("insert  file error:%s\n",sql);
            return;
        }
        //2.查文件id
        lststr.clear();
        sprintf(sql,"select f_id from t_file where f_MD5='%s' and f_path='%s';",rq->md5,cPath);
        res=m_sql->SelectMysql(sql,1,lststr);
        if(!res){
            printf("select fileid error:%s\n",sql);
            return;
        }
        string id=lststr.front();
        lststr.pop_front();
        file->fid=stoi(id);
        fid=file->fid;
        //3. 插入用户文件关系
        sprintf(sql,"insert into t_user_file(u_id,f_id,f_dir,f_name,f_uploadTime) values('%d','%d','%s','%s','%s');",
                rq->userid,file->fid,file->dir.c_str(),file->name.c_str(),file->time.c_str());
        res=m_sql->UpdataMysql(sql);
        if(!res){
            printf("insert file-user error:%s\n",sql);
            return;
        }
    //6. 编写发送回复
    STRU_UPLOAD_FILE_RS rs;
    rs.fileid=fid;
    rs.userid=rq->userid;
    rs.timestamp=rq->timestamp;
    rs.result=true;

    SendData(clientfd,(char*)&rs,sizeof(rs));
}

//文件块请求
void CLogic::fileContentRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //1.拆包
    STRU_FILE_CONTENT_RQ* rq=(STRU_FILE_CONTENT_RQ*)szbuf;
    STRU_FILE_CONTENT_RS rs;
    //2.获取文件信息
    int64_t user_time=rq->userid*number()+rq->timestamp;
    FileInfo* file=nullptr;
    if(!m_mapTimstampToFileinfo.find(user_time,file)){
         //1.找不到
        printf("找不到文件信息\n");
        return;
    }
    //3.写入
    int len=write(file->fileFd,rq->content,rq->len);
    if(len!=rq->len){
        //3.1.失败-跳回到读取前
        rs.result=false;
        lseek(file->fileFd,-1*len,SEEK_CUR);
    }else{
        //3.2.成功 pos更新位置
        rs.result=true;
        file->pos+=len;
        //到达末尾
        if(file->pos>=file->size){
            //更新文件状态已完成
            char sql[1024]="";
            sprintf(sql,"update t_file set f_state=1 where f_id='%d';",rq->fileid);
            bool res=m_sql->UpdataMysql(sql);
            if(!res){
                printf("数据库更新失败：%s\n",sql);
            }
            //是-关闭文件;回收map节点
            close(file->fileFd);
            m_mapTimstampToFileinfo.erase(user_time);
            delete file;

        }

    }

    //4.返回结果
    rs.len=rq->len;
    rs.fileid=rq->fileid;
    rs.userid=rq->userid;
    rs.timestamp=rq->timestamp;
    SendData(clientfd,(char*)&rs,sizeof(rs));
}

//获取文件列表
void CLogic::getFileList(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //1. 拆包
    STRU_GET_FILE_RQ* rq=(STRU_GET_FILE_RQ*)szbuf;
    string dir=rq->dir;
    //2. 根据dir查询
    char sql[1024]="";
    sprintf(sql,"select f_name,f_size,f_uploadtime,f_id,f_type from user_file_info where f_dir='%s' and u_id='%d' and f_state=1;",
            rq->dir,rq->userid);
    list<string> lststr;
    bool res=m_sql->SelectMysql(sql,5,lststr);
    int count=lststr.size()/5;
    if(!res){
        printf("查询数据库失败:%s\n",sql);
        return;
    }
    //3.保存文件信息
     int rslen=sizeof(STRU_GET_FILE_RS)+sizeof(STRU_FILE_INFO)*count;
    STRU_GET_FILE_RS* rs=(STRU_GET_FILE_RS*)malloc(rslen);
    rs->init();
    strcpy(rs->dir,rq->dir);
    rs->count=count;
    string name="";
    int size=0;
    string time="";
    int fileid=0;
    string type="";
    //也可以使用循环作为条件-while(lststr.size()!=0)
    for(int i=0;i<rs->count;i++){
        name=lststr.front();
        lststr.pop_front();
        size=stoi(lststr.front());
        lststr.pop_front();
        time=lststr.front();
        lststr.pop_front();
        fileid=stoi(lststr.front());
        lststr.pop_front();
        type=lststr.front();
        lststr.pop_front();

        strcpy(rs->fileInfo[i].name,name.c_str()) ;
        rs->fileInfo[i].size=size;
        strcpy(rs->fileInfo[i].time,time.c_str()) ;
        rs->fileInfo[i].fileid=fileid;
        strcpy(rs->fileInfo[i].fileType,type.c_str()) ;
    }

    //4. 发送回复

    SendData(clientfd,(char*)rs,rslen);
    free(rs);
    rs=nullptr;

}

//下载文件
void CLogic::downloadFile(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
   //1.拆包
    STRU_DOWNLOAD_FILE_RQ* rq=(STRU_DOWNLOAD_FILE_RQ*)szbuf;
    string dir=rq->dir;
    int fileid=rq->fileid;
    int userid=rq->userid;
    int timestamp=rq->timestamp;

    STRU_FILE_HEADER_RQ rqH;
    //2.查询数据库 查询文件信息 没有-返回
    char sql[1024]="";
    list<string> lststr;
    sprintf(sql,"select f_name,f_size,f_uploadtime,f_path,f_MD5 from user_file_info where u_id='%d' and f_dir='%s'and f_state=1 and f_id='%d';",
            userid,dir.c_str(),fileid);
    int res=m_sql->SelectMysql(sql,5,lststr);
    if(!res){
        printf("查询数据库失败:%s\n",sql);
        return;
    }
    FileInfo* file=nullptr;
    if(lststr.size()!=0){
        //有 保存文件信息
        file=new FileInfo;
        string name=lststr.front();
        lststr.pop_front();
        int size=stoi(lststr.front());
        lststr.pop_front();
        string time=lststr.front();
        lststr.pop_front();
        string path=lststr.front();
        lststr.pop_front();
        string md5=lststr.front();
        lststr.pop_front();
        file->dir=dir;
        file->md5=md5;
        file->name=name;
        file->size=size;
        file->time=time;
        file->type="file";
        file->absolutePath=path;
        file->fid=fileid;
        file->fileFd=open(path.c_str(),O_RDONLY);
        if(file->fileFd<=0){
            printf("打开文件失败：%s\n",path.c_str());
            delete file;
            file=nullptr;
            return;
        }
        //key值
        int64_t user_time=userid*number()+timestamp;
        //存入map
        m_mapTimstampToFileinfo.insert(user_time,file);
        if(!m_mapTimstampToFileinfo.IsExist(user_time)) {
            printf("存入map失败\n");
            return;
        }


    }else{//没有文件信息-返回
        //发送文件回复
        return;
    }
    //3. 发送文件头请求
    strcpy(rqH.dir,dir.c_str());
    strcpy(rqH.md5,file->md5.c_str());
    rqH.size=file->size;
    rqH.fileid=fileid;
    strcpy(rqH.fileName,file->name.c_str());
    rqH.timestamp=rq->timestamp;
    strcpy(rqH.fileType,file->type.c_str());
    SendData(clientfd,(char*)&rqH,sizeof(rqH));
}

//下载文件夹
void CLogic::downloadFileFolder(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //1.拆包
    STRU_DOWNLOAD_FOLDER_RQ*rq =(STRU_DOWNLOAD_FOLDER_RQ*)szbuf;
    //2. 遍历该文件夹下所有文件
    int timestamp=rq->timestamp;
    int userid=rq->userid;
    int fid=rq->fileid;
    string dir=rq->dir;
    //3.查询文件信息-name
    list<string> lststr;
    char sql[1024]="";
    sprintf(sql,"select f_name from user_file_info where u_id='%d' and f_id='%d' and f_dir='%s';"
            ,userid,fid,dir.c_str());
    bool res=m_sql->SelectMysql(sql,1,lststr);
    if(!res){
        printf("查询数据库失败:%s\n",sql);
        return;
    }
    if(lststr.size()<=0){
        printf("未查询到文件name信息");
        return;
    }
    string name=lststr.front();

    //5.下载文件夹
    downloadFolderByDir(timestamp,userid,fid,dir,name,clientfd);
}
//文件头回复
void CLogic::downloadFileHeadRs(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //1.拆包
    STRU_FILE_HEADER_RS* rs=(STRU_FILE_HEADER_RS*)szbuf;
    STRU_FILE_CONTENT_RQ rq;
    //2.取文件信息
    int64_t user_time=rs->userid*number()+rs->timestamp;
    FileInfo* file=nullptr;
    if(!m_mapTimstampToFileinfo.find(user_time,file)){
        printf("找不到文件信息\n");
        return;
    }
    //3.读文件
    int len=read(file->fileFd,rq.content,_DEF_BUFFER);
    if(len<0){
        perror("读取文件失败");//如果读取文件失败，依然发送内容请求，内容请求失败，发送文件头回复，成死循环
        return;
    }
    //4.发送文件内容请求
    rq.len=len;
    rq.fileid=rs->fileid;
    rq.userid=rs->userid;
    rq.timestamp=rs->timestamp;
    SendData(clientfd,(char*)&rq,sizeof(rq));
}
//文件内容回复
void CLogic::fileContentRs(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //1.拆包
    STRU_FILE_CONTENT_RS* rs=(STRU_FILE_CONTENT_RS*)szbuf;
    STRU_FILE_CONTENT_RQ rq;
    //2.获取文件信息
    FileInfo* file=nullptr;
    int64_t user_time=rs->userid*number()+rs->timestamp;
    if(!m_mapTimstampToFileinfo.find(user_time,file)){
        printf("未找到该文件信息\n");
        return;
    }
    int len=0;
    if(rs->result==false){
        //下载失败 游标回跳
        lseek(file->fileFd,-1*rs->len,SEEK_CUR);
    }else{
        //成功  pos偏移
        file->pos+=rs->len;
        //3.读取文件内容
        len=read(file->fileFd,rq.content,_DEF_BUFFER);
        //读取失败，游标偏移原位
        if(len<0){
            lseek(file->fileFd,-1*len,SEEK_CUR);
            perror("读取文件失败");
        }else{
            //读取成功
           //如果到末尾，关闭文件，删除文件信息
            if(file->pos>=file->size){
                close(file->fileFd);
                m_mapTimstampToFileinfo.erase(user_time);
                delete file;
                file=nullptr;
                return;
            }
        }
    }

    //4.发送文件块请求
    rq.len=len;
    rq.fileid=rs->fileid;
    rq.userid=rs->userid;
    rq.timestamp=rs->timestamp;
    SendData(clientfd,(char*)&rq,sizeof(rq));
}
//新建文件夹
void CLogic::addFolder(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //1.拆包
    STRU_ADD_FOLDER_RQ* rq=(STRU_ADD_FOLDER_RQ*)szbuf;
    STRU_ADD_FOLDER_RS rs;
    //2.数据库处理
    //插入文件表 size path count md5 state type
    char sql[1024]="";
    list<string> lststr;
    rq->dir;
    rq->time;
    rq->fileName;
    rq->timestamp;
    char path[1024]="";
    sprintf(path,"%s%d%s%s",_DEF_PATH,rq->userid,rq->dir,rq->fileName);
    sprintf(sql,"insert into t_file (f_size,f_path,f_count,f_md5,f_state,f_type) values(0,'%s',0,'?',1,'folder');",path);
    bool res= m_sql->UpdataMysql(sql);
    if(!res){
        printf("插入数据库失败1:%s\n",sql);
        return;
    }
    //查询文件id
    sprintf(sql,"select f_id from t_file where f_path='%s';",path);
    int ires=m_sql->SelectMysql(sql,1,lststr);
    if(!res){
        printf("查询数据库失败:%s\n",sql);
        return;
    }
    int fid=stoi(lststr.front());
    lststr.pop_front();
    //插入用户文件关系表 u_id f_id f_dir f_name f_uploadtime
    sprintf(sql,"insert into t_user_file(u_id,f_id,f_dir,f_name,f_uploadtime) values('%d','%d','%s','%s','%s');"
            ,rq->userid,fid,rq->dir,rq->fileName,rq->time);
    res=m_sql->UpdataMysql(sql);
    if(!res){
        printf("插入数据库失败2:%s\n",sql);
        return;
    }
    //3.创建目录
    umask(0000);
    ires=mkdir(path,0777);
    if(ires!=0&&errno != EEXIST){
        perror("创建目录失败");
        return ;
    }
    //4.发送回复
    rs.result=true;
    rs.userid=rq->userid;
    rs.timestamp=rq->timestamp;
    SendData(clientfd,(char*)&rs,sizeof (rs));
}

//分享文件
void CLogic::shareFile(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //1.拆包
    STRU_SHARE_FILE_RQ* rq=(STRU_SHARE_FILE_RQ*)szbuf;

    list<string> lststr;
    char sql[1024]="";
    bool res=false;
    int link=0;
    do{
        //2.随机生成分享链接-9位
        link=1+random()%9;//随机1-9
        link*=100000000;
        link+=random()%100000000;
        //3.去重 查链接是否已经存在
        sprintf(sql,"select s_link from user_file_info where s_link='%d';",link);
        res=m_sql->SelectMysql(sql,1,lststr);
        if(!res){
            printf("查询数据库失败%s\n",sql);
            return;
        }
    }while(lststr.size()>0);
    //4.遍历所有文件，设置分享链接
    string dir=rq->dir;
    int u_id=rq->userid;
    string shareTime=rq->shareTime;
    int fid=0;
    for(int i=0;i<rq->itemCount;i++){
        fid=rq->fileidArray[i];
        //根据fid dir userId插入文件信息
        sprintf(sql,"update t_user_file set s_link='%d',s_linkTime='%s' where u_id='%d' and f_id='%d' and f_dir='%s';",
                link,shareTime.c_str(),u_id,fid,dir.c_str());
        res=m_sql->UpdataMysql(sql);
        if(!res){
            printf("更新数据库失败%s\n",sql);
            return;
        }
    }
    //5.分享文件回复
    STRU_SHARE_FILE_RS rs;
    rs.result=true;
    SendData(clientfd,(char*)&rs,sizeof(rs));
}

//获取分享列表
void CLogic::getShareList(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //1.拆包
    STRU_MY_SHARE_RQ* rq=(STRU_MY_SHARE_RQ*)szbuf;
    int u_id=rq->userid;
    //2.查询分享列表
    int itemCount=0;
    list<string> lststr;
    char sql[1024]="";
    sprintf(sql,"select f_name,f_size,s_linkTime,s_link from user_file_info where u_id='%d' and s_link is not null;",u_id);
    bool res=m_sql->SelectMysql(sql,4,lststr);
    if(!res){
        printf("查询数据库失败%s\n",sql);
        return;
    }
    if(lststr.size()==0)printf("查询结果为空\n");
    itemCount=lststr.size()/4;
    //3.发送回复
    int rsLen=sizeof(STRU_MY_SHARE_RS)+sizeof(STRU_MY_SHARE_FILE)*itemCount;
    STRU_MY_SHARE_RS* rs=(STRU_MY_SHARE_RS*)malloc(rsLen);
    rs->init();
    rs->itemCount=itemCount;
    //插入回复数据
    string name="";
    int size=0;
    string time="";
    int link=0;
    for(int i=0;i<itemCount;i++){
        name=lststr.front();
        lststr.pop_front();
        size=stoi(lststr.front());
        lststr.pop_front();
        time=lststr.front();
        lststr.pop_front();
        link=stoi(lststr.front());
        lststr.pop_front();
        strcpy(rs->items[i].name,name.c_str());
        rs->items[i].size=size;
        strcpy(rs->items[i].time,time.c_str());
        rs->items[i].shareLink=link;
    }
    SendData(clientfd,(char*)rs,rsLen);
}
//根据分享码获取文件
void CLogic::getShareByLink(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //1.拆包
    STRU_GET_SHARE_RQ* rq=(STRU_GET_SHARE_RQ*)szbuf;
    STRU_GET_SHARE_RS rs;
    int link=rq->shareLink;
    int userDestId=rq->userid;
    string dirDest=rq->dir;
    string time=rq->time;
    //2.根据分享码获取分享文件列表
    list<string> lststr;
    char sql[1024]="";
    //f_type u_id(分享人的),f_id,f_dir(分享人的),f_name,f_uploadTime
    sprintf(sql,"select u_id,f_id,f_dir,f_name,f_type from user_file_info where s_link='%d';",link);
    bool res= m_sql->SelectMysql(sql,5,lststr);
    if(!res){
        printf("查询数据库失败:%s\n",sql);
        return;
    }
    int listCount=lststr.size()/5;
    if(lststr.size()==0){
        rs.result=false;
        SendData(clientfd,(char*)&rs,sizeof(rs));
        return;
    }
    //3.遍历文件列表
    int userSourceId=0;
    int fid=0;
    string dirSource="";
    string name="";
    string type="";
   while(lststr.size()>0){
        userSourceId=stoi(lststr.front());
        lststr.pop_front();
        fid=stoi(lststr.front());
        lststr.pop_front();
        dirSource=lststr.front();
        lststr.pop_front();
        name=lststr.front();
        lststr.pop_front();
        type=lststr.front();
        lststr.pop_front();
        //文件-插入用户关系表
        //文件夹-遍历所有文件，插入用户关系表
        sprintf(sql,"insert into t_user_file(u_id,f_id,f_dir,f_name,f_uploadTime) values('%d','%d','%s','%s','%s');",
                userDestId,fid,dirDest.c_str(),name.c_str(),time.c_str());
        res=m_sql->UpdataMysql(sql);
        if(!res){
            printf("更新数据库失败%s\n",sql);
            return;
        }
        if(type=="folder"){
            //插入用户关系表 根据分享人目录遍历 获取人目录插入
            dirDest=dirDest+name+"/";
            dirSource=dirSource+name+"/";
            //根据新路径 查询分享人文件夹下文件
            //遍历列表-递归
            addFolderByShareDir(dirDest,userDestId,dirSource,userSourceId,time);

        }
    }
    //4.发送回复
    rs.result=true;
    strcpy(rs.dir,rq->dir);
    SendData(clientfd,(char*)&rs,sizeof(rs));
}

//删除文件
void CLogic::deleteFile(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //1.拆包
    STRU_DELETE_FILE_RQ* rq=(STRU_DELETE_FILE_RQ*)szbuf;

    list<string> lststr;
    char sql[1024]="";
    int res=false;
    //2.遍历所有文件
    string dir=rq->dir;
    int u_id=rq->userid;
    int fid=0;
    string fileType="";
    string name="";
    string path="";
    for(int i=0;i<rq->fileCount;i++){
        fid=rq->fileidArray[i];
        //3.查询文件类型
        sprintf(sql,"select f_type,f_name,f_path from user_file_info where f_id='%d' and u_id='%d' and f_dir='%s';",
                fid,u_id,dir.c_str());
        res=m_sql->SelectMysql(sql,3,lststr);
        if(!res){
            printf("查询数据库失败%s\n",sql);
            return;
        }
        //4.根据文件类型删除文件关系
        fileType=lststr.front();
        lststr.pop_front();
        name=lststr.front();
        lststr.pop_front();
        path=lststr.front();
        lststr.pop_front();
        if(fileType=="file"){
            //删除文件
            deleteFileById(u_id,fid,dir,path);
        }else if(fileType=="folder"){
            //删除文件夹
            deleteFolderById(u_id,fid,dir,name);
        }
    }
    //5.分享文件回复
    STRU_DELETE_FILE_RS rs;
    rs.result=true;
    strcpy(rs.dir,rq->dir);
    SendData(clientfd,(char*)&rs,sizeof(rs));
}






//-------------------工具函数-------------------------------------------------------
 //根据源用户目录，向目标用户目录插入文件
void CLogic::addFolderByShareDir(string dirDest, int userDestId, string dirSource, int userSourceId,string time)
{
    printf("addFolderByShareDir\n");
    //1..根据目录查询列表
    list<string> lststr;
    char sql[1024]="";
    //f_type,f_id,f_name
    sprintf(sql,"select f_id,f_name,f_type from user_file_info where f_dir='%s';",dirSource.c_str());
    bool res= m_sql->SelectMysql(sql,3,lststr);
    if(!res){
        printf("查询数据库失败:%s\n",sql);
        return;
    }
    if(lststr.size()==0){
        printf("当前文件夹为空:%s\n",dirDest.c_str());
    }
    //3.遍历文件列表
    int fid=0;
    string name="";
    string type="";
   while(lststr.size()>0){
        fid=stoi(lststr.front());
        lststr.pop_front();
        name=lststr.front();
        lststr.pop_front();
        type=lststr.front();
        lststr.pop_front();
        //文件-插入用户关系表
        //文件夹-遍历所有文件，插入用户关系表
        sprintf(sql,"insert into t_user_file(u_id,f_id,f_dir,f_name,f_uploadTime) values('%d','%d','%s','%s','%s');",
                userDestId,fid,dirDest.c_str(),name.c_str(),time.c_str());
        res=m_sql->UpdataMysql(sql);
        if(!res){
            printf("更新数据库失败%s\n",sql);
            return;
        }
        if(type=="folder"){
            //插入用户关系表 根据分享人目录遍历 获取人目录插入
            string newDirDest=dirDest+name+"/";
            string newDirSource=dirSource+name+"/";
             //遍历列表-递归
            addFolderByShareDir(newDirDest,userDestId,newDirSource,userSourceId,time);
        }
   }
}
//根据文件夹信息下载文件夹
void CLogic::downloadFolderByDir(int timeStamp, int userid, int fid, string dir,string name,int clientfd)
{
    printf("downloadFolderByDir:%s\n",dir.c_str());
    //1.拼接路径
    string curDir=dir+name+"/";
    //给客户端发送信息新建文件夹
    STRU_ADD_FOLDER_RQ rqFolder;
    strcpy(rqFolder.dir,curDir.c_str());
    SendData(clientfd,(char*)&rqFolder,sizeof(rqFolder));
    //2.遍历所有文件
    list<string> lststr;
    char sql[1024]="";
    sprintf(sql,"select f_type,f_id,f_name from user_file_info where u_id='%d' and f_dir='%s';"
            ,userid,curDir.c_str());
    bool res=m_sql->SelectMysql(sql,3,lststr);
    if(!res){
        printf("查询数据库失败:%s\n",sql);
        return;
    }
    FileInfo* file;
    string fileType="";
    int fileid=0;
    string fileName="";
    while(lststr.size()>0){
        fileType=lststr.front();
        lststr.pop_front();
        fileid=stoi(lststr.front());
        lststr.pop_front();
        fileName=lststr.front();
        lststr.pop_front();
        //为每一个文件创建新时间戳
        int newTimestamp=timeStamp+1;
        int64_t user_newTimeStamp=userid*number()+newTimestamp;
        while(m_mapTimstampToFileinfo.find(user_newTimeStamp,file)){
            newTimestamp++;
            user_newTimeStamp=userid*number()+newTimestamp;
        }
        //3.判断文件类型
        if(fileType=="file"){
            //4.文件-调用文件下载
            STRU_DOWNLOAD_FILE_RQ rq;
            rq.timestamp=newTimestamp;
            rq.fileid=fileid;
            rq.userid=userid;
            strcpy(rq.dir,curDir.c_str());
            downloadFile(clientfd,(char*)&rq,sizeof(rq));
        }else if(fileType=="folder"){
            //递归调用函数
            downloadFolderByDir(newTimestamp,userid,fileid,curDir,fileName,clientfd);
        }
    }

}
//删除文件
void CLogic::deleteFileById(int u_id, int f_id, string dir,string path)
{
    printf("deleteFileById:%s\n",path.c_str());
    list<string> lststr;
    char sql[1024]="";
    int res=false;

    //1.删除
    sprintf(sql,"delete from  t_user_file where u_id='%d' and f_id='%d' and f_dir='%s' ;",
            u_id,f_id,dir.c_str());
    res=m_sql->UpdataMysql(sql);
    if(!res){
        printf("更新数据库失败%s\n",sql);
        return;
    }
    //2.查询引用计数,路径
    sprintf(sql,"select f_count from t_file where  f_id='%d' ;",f_id);
    res=m_sql->SelectMysql(sql,1,lststr);
    if(!res){
        printf("查询数据库失败%s\n",sql);
        return;
    }
    //3.如果引用计数为0文件信息被删除
    if(lststr.size()==0){
        // 删除本地文件
        if (remove(path.c_str()) != 0) {  // 使用 remove 函数删除文件
            perror("删除本地文件失败");
        }
    }
}
//删除文件夹
void CLogic::deleteFolderById(int u_id, int f_id, string dir,string name)
{
    printf("deleteFolderById:%s/%s\n",dir.c_str(),name.c_str());
    //1.删除文件关系
    list<string> lststr;
    char sql[1024]="";
    int res=false;
    sprintf(sql,"delete from  t_user_file where u_id='%d' and f_id='%d' and f_dir='%s' ;",
            u_id,f_id,dir.c_str());
    res=m_sql->UpdataMysql(sql);
    if(!res){
        printf("更新数据库失败%s\n",sql);
        return;
    }
    //2.拼接路径
    string curDir=dir+name+"/";
    //3.查询列表文件
    lststr.clear();
    sprintf(sql,"select f_type,f_name,f_path from user_file_info where f_id='%d' and u_id='%d' and f_dir='%s';",
            f_id,u_id,dir.c_str());
    res=m_sql->SelectMysql(sql,3,lststr);
    if(!res){
        printf("查询数据库失败%s\n",sql);
        return;
    }
    int fid=0;
    string fileType="";
    string fileName="";
    //3.递归删除文件关系
    while(lststr.size()>0){
        fid=stoi(lststr.front());
        lststr.pop_front();
        fileType=lststr.front();
        lststr.pop_front();
        fileName=lststr.front();
        lststr.pop_front();
        if(fileType=="file"){
            sprintf(sql,"delete from  t_user_file where u_id='%d' and f_id='%d' and f_dir='%s' ;",
                    u_id,fid,dir.c_str());
            res=m_sql->UpdataMysql(sql);
            if(!res){
                printf("更新数据库失败%s\n",sql);
                return;
            }
        }else if(fileType=="folder"){
               deleteFolderById(u_id,fid,curDir,fileName);
        }
    }

}




