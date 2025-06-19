#include "clogic.h"
void CLogic::setNetPackMap()
{
    NetPackMap(_DEF_PACK_REGISTER_RQ)    = &CLogic::RegisterRq;
    NetPackMap(_DEF_PACK_LOGIN_RQ)       = &CLogic::LoginRq;
}

#define _DEF_COUT_FUNC_    cout << "clientfd:"<< clientfd << __func__ << endl;

#define _DEF_PATH "/home/Node/NetDisk/"
//注册
void CLogic::RegisterRq(sock_fd clientfd,char* szbuf,int nlen)
{
    _DEF_COUT_FUNC_
   //1. 拆包
    STRU_REGISTER_RQ* rq=(STRU_REGISTER_RQ*)szbuf;
    STRU_REGISTER_RS rs;
   string tel=rq->tel;
   string name=rq->name;
   string pass=rq->password;
    //2. 查询数据
    char sql[1000]="";
    list<string> strlst;
    sprintf(sql,"select u_tel from t_user where u_tel='%s';",rq->tel);
    bool res=m_sql->SelectMysql(sql,1,strlst);
    if(!res){
        std::cout<<"select fail:"<<sql<<std::endl;
        rs.result=register_error;
        m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));
        return;
    }
   //2.1 电话号是否已注册
    if(strlst.size()!=0){
        rs.result=user_is_exist;
    }else{
        //1.插入数据
        sprintf(sql,"insert into t_user(u_name,u_tel,u_tassword) values('%s','%s','%s');",rq->name,rq->tel,rq->password);
        res=m_sql->UpdataMysql(sql);
        if(!res){
            std::cout<<"update fail:"<<sql<<std::endl;
            rs.result=register_error;
            m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));
            return;
           }
        //2. 查询用户id
        strlst.clear();
        sprintf(sql,"select * from t_user where tel='%s';",rq->tel);
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
            sprintf(path,"%s%d/",path,id);
            //创建路径
            umask(0);
            mkdir(path,0777);
        }
        rs.result=register_success;
    }
   //3.返回注册结果
    m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));
}

//登录
void CLogic::LoginRq(sock_fd clientfd ,char* szbuf,int nlen)
{
    _DEF_COUT_FUNC_

    STRU_LOGIN_RS rs;

}
