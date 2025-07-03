#ifndef CLOGIC_H
#define CLOGIC_H
#include <errno.h>
//#define _DEF_NUMBER 1000000000UL
//宏定义添加UL 避免截断
#include"TCPKernel.h"
#include "packdef.h"
class CLogic
{
public:
    CLogic( TcpKernel* pkernel )
    {
        m_pKernel = pkernel;
        m_sql = pkernel->m_sql;
        m_tcp = pkernel->m_tcp;
    }
public:
    //设置协议映射
    void setNetPackMap();
    //计算参数
    long number();
    /************** 发送数据*********************/
    void SendData( sock_fd clientfd, char*szbuf, int nlen )
    {
        m_pKernel->SendData( clientfd ,szbuf , nlen );
    }
    /************** 网络处理 *********************/
    //注册
    void RegisterRq(sock_fd clientfd, char*szbuf, int nlen);
    //登录
    void LoginRq(sock_fd clientfd, char*szbuf, int nlen);
    //上传文件
    void uploadFile(sock_fd clientfd, char*szbuf, int nlen);
    //文件块请求
    void fileContentRq(sock_fd clientfd, char*szbuf, int nlen);
    //获取文件列表
    void getFileList(sock_fd clientfd, char*szbuf, int nlen);
    //下载文件
    void downloadFile(sock_fd clientfd, char*szbuf, int nlen);
    //下载文件夹
    void downloadFileFolder(sock_fd clientfd, char*szbuf, int nlen);
    //文件头回复处理
    void downloadFileHeadRs(sock_fd clientfd, char*szbuf, int nlen);
    //文件块回复
    void fileContentRs(sock_fd clientfd, char*szbuf, int nlen);
    //新建文件夹
    void addFolder(sock_fd clientfd, char*szbuf, int nlen);
    //分享文件
    void shareFile(sock_fd clientfd, char*szbuf, int nlen);
    //获取分享列表
    void getShareList(sock_fd clientfd, char*szbuf, int nlen);
    /*******************************************/

private:
    TcpKernel* m_pKernel;
    CMysql * m_sql;
    Block_Epoll_Net * m_tcp;
    MyMap<int,STRU_USERINFO*> m_mapIdToUserinfo;
    MyMap<int,FileInfo*> m_mapTimstampToFileinfo;
};

#endif // CLOGIC_H
