#include "logindialog.h"
#include "ui_logindialog.h"
#include <QDebug>
#include <QMessageBox>
#include <QRegExp>
loginDialog::loginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::loginDialog)
{
    ui->setupUi(this);

    //设置窗口标题
    setWindowTitle("登录/注册");

    //窗口默认登录
    ui->tabWidget->setCurrentIndex(0);//0-第一个选项卡
}

loginDialog::~loginDialog()
{
    delete ui;
}

void loginDialog::on_pb_submit_regist_clicked()
{
    qDebug()<<__func__;
    //注册提交
    //1. 验证信息正确性
    //信息不为空/信息不全为空格
    //手机号11位
    QString tel=ui->le_tel_regist->text();
    QString telTmp=tel;
    if(tel.isEmpty()||telTmp.remove(" ").size()==0){
        QMessageBox::about(this,"提示","输入不能为空");
        return;
    }else if(tel.size()!=11){
        QMessageBox::about(this,"提示","手机号必须为11位");
        return;
    }
    //手机合法校验
    QRegExp exp("^1[356789][0-9]\{9\}$");
    bool res=exp.exactMatch(tel);
    if(!res){
        QMessageBox::about(this,"提示","手机号非法");
        return;
    }
    //密码最长20位
    QString pass=ui->le_pass_regist->text();
    QString passTmp=pass;
    if(pass.isEmpty()||passTmp.remove(" ").size()==0){
        QMessageBox::about(this,"提示","输入不能为空");
        return;
    }else if(pass.size()>20){
        QMessageBox::about(this,"提示","密码需要小于20位");
        return;
    }
    QString passComfir=ui->le_passcomfir->text();
    QString passCTmp=passComfir;
    if(passComfir.isEmpty()||passCTmp.remove(" ").size()==0){
        QMessageBox::about(this,"提示","输入不能为空");
        return;
    }else if(pass!=passComfir){
        QMessageBox::about(this,"提示","两次密码输入不一致");
        return;
    }
    //昵称<12位
    QString name=ui->le_name->text();
    QString nameTmp=name;
    if(name.isEmpty()||nameTmp.remove(" ").size()==0){
        QMessageBox::about(this,"提示","输入不能为空");
        return;
    }else if(name.size()>11){
        QMessageBox::about(this,"提示","昵称必须小于12位");
        return;
    }
    //TODO::过滤敏感词汇
    // if(){
    //     //过滤不合法输入

    // }
    //2. 转发信息给核心类
    Q_EMIT SIG_registerCommit(tel,pass,name);
}


void loginDialog::on_pb_clear_regist_clicked()
{
    qDebug()<<__func__;
    //注册清空
    ui->le_tel_regist->setText("");
    ui->le_pass_regist->setText("");
    ui->le_name->setText("");
    ui->le_passcomfir->setText("");
}


void loginDialog::on_pb_submit_clicked()
{
    qDebug()<<__func__;
    //登录提交
    //获取信息
    QString tel=ui->le_tel->text();
    QString telTmp=tel;
    QString pass=ui->le_tel->text();
    QString passTmp=pass;
    //1.信息正确定验证
    //验证电话
    if(tel.isEmpty()||telTmp.remove(" ").size()==0){
        QMessageBox::about(this,"提示","输入不能为空");
        return;
    }
    //手机合法校验
    QRegExp exp("^1[356789][0-9]\{9\}$");
    bool res=exp.exactMatch(tel);
    if(!res){
        QMessageBox::about(this,"提示","手机号非法");
        return;
    }
    //验证密码
    if(pass.isEmpty()||passTmp.remove(" ").size()==0){
        QMessageBox::about(this,"提示","输入不能为空");
        return;
    }else if(pass.size()>20){
        QMessageBox::about(this,"提示","密码需要小于20位");
        return;
    }
    //2. 转发给kernel
    Q_EMIT SIG_loginCommit(tel,pass);
}


void loginDialog::on_pb_clear_clicked()
{
    qDebug()<<__func__;
    //登录清空
    ui->le_tel->setText("");
    ui->le_pass->setText("");

}

