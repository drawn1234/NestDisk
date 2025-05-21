#include "ckernel.h"

CKernel::CKernel(QObject *parent)
    : QObject{parent}
{
    //创建窗口对象，显示
    m_pMainDialog=new MainDialog;
    m_pMainDialog->show();
    connect(m_pMainDialog,SIGNAL(sig_close()),this,SLOT(slot_closeMainDialog()));
}

void CKernel::slot_closeMainDialog()
{
    //关闭窗口，回收窗口对象
    qDebug()<<__func__;
    delete m_pMainDialog;
    m_pMainDialog=nullptr;
}
