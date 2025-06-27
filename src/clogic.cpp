#include "clogic.h"
void CLogic::setNetPackMap()
{
    NetPackMap(_DEF_PACK_REGISTER_RQ)    = &CLogic::RegisterRq;
    NetPackMap(_DEF_PACK_LOGIN_RQ)       = &CLogic::LoginRq;
    NetPackMap(_DEF_PACK_UPLOAD_FILE_RQ) = &CLogic::uploadFile;
    NetPackMap(_DEF_PACK_FILE_CONTENT_RQ) = &CLogic::fileContentRq;
    NetPackMap(_DEF_PACK_FILE_LIST_RQ) = &CLogic::getFileList;


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
    //2.是否秒传
        //1.是 TODO:

    //3.创建文件信息
    //拼接文件路径
    char cPath[1000]="";
    //使用md5作为服务器中的文件名，避免文件重复
    sprintf(cPath,"%s%d%s%s",_DEF_PATH,rq->userid,rq->dir,rq->md5);//_DEF_PATH+userid+dir+md5

    FileInfo* file=new FileInfo;
    file->dir=rq->dir;
    file->md5=rq->md5;
    file->name=rq->fileName;
    file->size=rq->size;
    file->time=rq->time;
    file->type=rq->fileType;
    file->fileFd=open(cPath,O_CREAT|O_WRONLY|O_TRUNC,0777); //使用linux创建打开文件-读写，创建，清空
    file->fid;
    file->absolutePath=cPath;
    //4.map存储文件信息
    //使用用户id+时间戳存储文件信息userid*1000 000 000 +timestamp
    //1000 000 000使用宏定义会发生截断
    int64_t user_time=rq->timestamp+rq->userid*number();
    m_mapTimstampToFileinfo.insert(user_time,file);
    //5. 数据库操作
        //1. 插入文件信息
        list<string> lststr;
        char sql[1024]="";
        sprintf(sql,"insert into t_file(f_size,f_path,f_md5,f_count,f_state,f_type) values('%d','%s','%s',0,1,'%s');",
                file->size,cPath,file->md5.c_str(),file->type.c_str());
        bool res=m_sql->UpdataMysql(sql);
        if(!res){
            printf("insert  file error:%s\n",sql);

        }
        //2.查文件id
        lststr.clear();
        sprintf(sql,"select f_id from t_file where f_MD5='%s' and f_path='%s';",rq->md5,cPath);
        res=m_sql->SelectMysql(sql,1,lststr);
        if(!res){
            printf("select fileid error:%s\n",sql);

        }
        string id=lststr.front();
        lststr.pop_front();
        file->fid=stoi(id);
        //3. 插入用户文件关系
        sprintf(sql,"insert into t_user_file(u_id,f_id,f_dir,f_name,f_uploadTime) values('%d','%d','%s','%s','%s');",
                rq->userid,file->fid,file->dir.c_str(),file->name.c_str(),file->time.c_str());
        res=m_sql->UpdataMysql(sql);
        if(!res){
            printf("insert file-user error:%s\n",sql);

        }
    //6. 编写发送回复
    STRU_UPLOAD_FILE_RS rs;
    rs.fileid=file->fid;
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
            //是-关闭文件;回收map节点
            close(file->fileFd);
            m_mapTimstampToFileinfo.erase(user_time);
            delete file;
            //更新文件状态已完成
            char sql[1024]="";
            sprintf(sql,"update t_file set f_state=1 where f_id='%d';",rq->fileid);
            bool res=m_sql->UpdataMysql(sql);
            if(!res){
                printf("数据库更新失败：%s\n",sql);
            }
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
    if(lststr.size()==0)return;
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
