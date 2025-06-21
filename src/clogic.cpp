#include "clogic.h"
void CLogic::setNetPackMap()
{
    NetPackMap(_DEF_PACK_REGISTER_RQ)    = &CLogic::RegisterRq;
    NetPackMap(_DEF_PACK_LOGIN_RQ)       = &CLogic::LoginRq;
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
            char path[_MAX_PATH]="";
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
