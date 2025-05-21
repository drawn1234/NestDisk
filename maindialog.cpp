#include "maindialog.h"
#include "ui_maindialog.h"
#include <QMessageBox>
MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MainDialog)
{
    ui->setupUi(this);
}

MainDialog::~MainDialog()
{
    delete ui;
}

void MainDialog::closeEvent(QCloseEvent *event)
{
    if(QMessageBox::question(this,"提示","是否退出")
        ==QMessageBox::Yes){
        //关闭
        event->accept();
        Q_EMIT sig_close();
    }else{
        event->ignore();
    }
}
