#include "maindialog.h"
#include "ui_maindialog.h"
#include <QMessageBox>
#include <QCursor>
#include <QDebug>
#include <QFileDialog>
#include <QProgressBar>
MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MainDialog)
{
    ui->setupUi(this);
    //设置默认显示界面
    //1. 显示文件界面
    ui->sw_page->setCurrentIndex(0);
    //2. 传输界面默认显示已完成
    ui->tw_trans->setCurrentIndex(2);
    //3. 设置窗口标题
    this->setWindowTitle("我的网盘");
    //4. 设置窗口最大，最小化
    this->setWindowFlags(Qt::WindowMinMaxButtonsHint|Qt::WindowCloseButtonHint);

    //设置添加文件菜单
    //1. 定义菜单项 资源路径
    QAction* action_addFolder=new QAction(QIcon(":/resources/images/folder.png"),"新建文件夹");
    QAction* action_uploadFile=new QAction("上传文件");
    QAction* action_uploadFolder=new QAction("上传文件夹");
    //2. 添加菜单项
    m_menuAddFile.addAction(action_addFolder);
    m_menuAddFile.addSeparator();//分隔符
    m_menuAddFile.addAction(action_uploadFile);
    m_menuAddFile.addSeparator();
    m_menuAddFile.addAction(action_uploadFolder);
    //3. 绑定菜单项对应槽函数
    connect(action_addFolder,SIGNAL(triggered(bool)),this,SLOT(slot_action_addFolder(bool)));
    connect(action_uploadFile,SIGNAL(triggered(bool)),this,SLOT(slot_action_uploadFile(bool)));
    connect(action_uploadFolder,SIGNAL(triggered(bool)),this,SLOT(slot_action_uploadFolder(bool)));

    //设置文件菜单
    //1. 定义菜单项
    QAction* action_downloadFile=new QAction("下载文件");
    QAction* action_shareFile=new QAction("分享文件");
    QAction* action_deleteFile=new QAction("删除文件");
    QAction* action_star=new QAction("收藏");

    //2.添加菜单项
    m_menuFileInfo.addAction(action_addFolder);
    m_menuFileInfo.addSeparator();
    m_menuFileInfo.addAction(action_downloadFile);
    m_menuFileInfo.addSeparator();
    m_menuFileInfo.addAction(action_shareFile);
    m_menuFileInfo.addSeparator();
    m_menuFileInfo.addAction(action_deleteFile);
    m_menuFileInfo.addSeparator();
    m_menuFileInfo.addAction(action_star);

    //3. 绑定菜单项槽函数
    connect(action_downloadFile,SIGNAL(triggered(bool)),this,SLOT(slot_action_dowloadFile(bool)));
    connect(action_shareFile,SIGNAL(triggered(bool)),this,SLOT(slot_action_shareFile(bool)));
    connect(action_deleteFile,SIGNAL(triggered(bool)),this,SLOT(slot_action_deleteFile(bool)));

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

void MainDialog::on_pb_file_clicked()
{
    //设置当前显示界面
    ui->sw_page->setCurrentIndex(0);
}

void MainDialog::on_pb_trans_clicked()
{
    ui->sw_page->setCurrentIndex(1);
}

void MainDialog::on_pb_share_clicked()
{
     ui->sw_page->setCurrentIndex(2);
}

void MainDialog::on_pb_add_clicked()
{
    //点击添加文件按钮
    //1. 弹出菜单
    m_menuAddFile.exec(QCursor::pos());
}
//新建文件夹
#include <QInputDialog>
void MainDialog::slot_action_addFolder(bool flag)
{
    qDebug()<<__func__;
    qDebug()<<__func__;
    //1.弹出输入窗口
    QString name=QInputDialog::getText(this,"新建文件夹","输入名称");
    //处理非法名字-空白字符
    QString nametmp=name;
    if(name.isEmpty()||nametmp.remove(" ").isEmpty()){
        QMessageBox::about(this,"提示","文件名不为空");
        return;
    }
    //过滤敏感词汇

    //长度处理

    //过滤非法字符 /\:*^&<>|
    //方法1：正则表达式 方法2：contains
    if(name.contains("\\")||name.contains("/")||name.contains(":")||name.contains("^")||name.contains("&")
        ||name.contains("<")||name.contains(">")||name.contains("?"))
    {
        QMessageBox::about(this,"提示","名字不能包含‘/\:*^&<>|’");
        return;
    }
    //2.判断是否已经存在

    //3. 采集文件名以及当前路径
    QString dir=ui->lb_path->text();
    Q_EMIT sig_addFolder(name,dir);

}

void MainDialog::slot_action_uploadFile(bool flag)
{
    qDebug()<<__func__;
    //1.弹窗选择文件
    QString path=QFileDialog::getOpenFileName(this,"选择文件","./");
    if(path.isEmpty()){
        qDebug()<<"上传文件失败";
        return;}
    //2.判断目前是否有相同的上传文件 有就取消

    //3. 发送上传文件信号
    QString dir=ui->lb_path->text();
    Q_EMIT sig_uploadFile(path,dir);
}


void MainDialog::slot_action_uploadFolder(bool flag)
{
     //上传文件夹
    qDebug()<<__func__;
    //1.点击，弹出文件选择对话框 选择路径
    QString path = QFileDialog::getExistingDirectory(this,"打开文件夹","./");
    //2.是否为空
    if(path.isEmpty()){
        return;
    }
    //3.过滤 是否正在上传 TODO:

    //4.发信号 上传什么目录下的什么名字的文件夹
    Q_EMIT sig_uploadFolder(path,ui->lb_path->text());
}

void MainDialog::slot_insertUploadFile(FileInfo &file)
{
    qDebug()<<__func__;
    //表格插入文件信息-行信息
    //列：文件 时间 大小 速率 进度 按钮
    //1. 新增一行 获取当前行+1 设置行数
    int rows=ui->tb_upload->rowCount();
    ui->tb_upload->setRowCount(rows+1);
    //2. 设置这一行的每一列的控件（添加对象）
    MytablewigetItem *item0=new MytablewigetItem;//表格对象需要创建在堆区，避免函数结束自动回收
    item0->slot_setFile(file);
    QTableWidgetItem *item1=new QTableWidgetItem(file.time);
    QTableWidgetItem *item2=new QTableWidgetItem(file.getSize(file.size));
    QTableWidgetItem *item3=new QTableWidgetItem("0 KM/S");
    ui->tb_upload->setItem(rows,0,item0);
    ui->tb_upload->setItem(rows,1,item1);
    ui->tb_upload->setItem(rows,2,item2);
    ui->tb_upload->setItem(rows,3,item3);
    //添加进度条
    QProgressBar *item4=new QProgressBar;
    item4->setMaximum(file.size);//设置最大值-进度条会自动换算
    ui->tb_upload->setCellWidget(rows,4,item4);

    //设置按钮
    QPushButton *button=new QPushButton;
    if(file.isPause){
        button->setText("开始");
    }else{
        button->setText("暂停");
    }
    ui->tb_upload->setCellWidget(rows,5,button);
}

void MainDialog::slot_insertDownloadFile(FileInfo &file)
{
    //插入下载文件
    qDebug()<<__func__;
    //表格插入文件信息-行信息
    //列：文件 时间 大小 速率 进度 按钮
    //1. 新增一行 获取当前行+1 设置行数
    int rows=ui->tb_download->rowCount();
    ui->tb_download->setRowCount(rows+1);
    //2. 设置这一行的每一列的控件（添加对象）
    MytablewigetItem *item0=new MytablewigetItem;//表格对象需要创建在堆区，避免函数结束自动回收
    item0->slot_setFile(file);
    QTableWidgetItem *item1=new QTableWidgetItem(file.time);
    QTableWidgetItem *item2=new QTableWidgetItem(file.getSize(file.size));
    QTableWidgetItem *item3=new QTableWidgetItem("0 KM/S");
    ui->tb_download->setItem(rows,0,item0);
    ui->tb_download->setItem(rows,1,item1);
    ui->tb_download->setItem(rows,2,item2);
    ui->tb_download->setItem(rows,3,item3);
    //添加进度条
    QProgressBar *item4=new QProgressBar;
    item4->setMaximum(file.size);//设置最大值-进度条会自动换算
    ui->tb_download->setCellWidget(rows,4,item4);

    //设置按钮
    QPushButton *button=new QPushButton;
    if(file.isPause){
        button->setText("开始");
    }else{
        button->setText("暂停");
    }
    ui->tb_download->setCellWidget(rows,5,button);
}

void MainDialog::slot_insertTbComplete(FileInfo &file,QString transType)
{
    //回收文件或者文件上传结束使用
    //上传完成
    qDebug()<<__func__;
    //表格插入文件信息-行信息
    //列：文件 时间 大小 速率 进度 按钮
    //1. 新增一行 获取当前行+1 设置行数
    int rows=ui->tb_finished->rowCount();
    ui->tb_finished->setRowCount(rows+1);
    //2. 设置这一行的每一列的控件（添加对象）
    MytablewigetItem *item0=new MytablewigetItem;//表格对象需要创建在堆区，避免函数结束自动回收
    item0->slot_setFile(file);
    QTableWidgetItem *item1=new QTableWidgetItem(file.time);
    QTableWidgetItem *item2=new QTableWidgetItem(file.getSize(file.size));
    ui->tb_finished->setItem(rows,0,item0);
    ui->tb_finished->setItem(rows,1,item1);
    ui->tb_finished->setItem(rows,2,item2);

    if(transType==_DEF_UPLOAD)
    {
        QTableWidgetItem * item3=new QTableWidgetItem("上传完成");
        ui->tb_finished->setItem(rows,3,item3);
    }
    else if(transType==_DEF_DOWNLOAD){
        QPushButton* button=new QPushButton;//下载完成 点击按钮弹出文件夹
        button->setIcon(QIcon(":/resources/images/folder.png"));
        //设置按钮风格
        button->setFlat(true);//设置扁平
        //如何将文件路径告诉按钮？-给文件添加属性：tooltip提示
        //方法2：发送信号
        button->setToolTip(file.absolutePath);
        //按钮功能实现
        connect(button,SIGNAL(clicked(bool)),this,SLOT(slot_openPath(bool)));
        ui->tb_finished->setCellWidget(rows,3,button);
    }
}
#include <QProcess>
void MainDialog::slot_openPath(bool flag){
    qDebug()<<__func__;
    //实现下载完成列表中，点击下载完成按钮，弹出下载文件
    //如何得到button按钮？使用sender得到信号发送者的指针-button
    QPushButton* button=(QPushButton*)QObject::sender();
    QString path=button->toolTip();//通过tooltip提示得到文件路径
    //如何打开文件路径？-打开文件资源管理器进程
    //char pathbuf[1024]="";
    //需要将"/"转换为"\\"
    path.replace('/','\\');
    //explorer 使用 explorer+路径 可以运行
    QProcess process;
    QStringList lst;
    //QStringList填入
    //方法1：
    //lst.push_back("/select,");
    //lst.push_back(path);
    //方法2：左移填入
    lst<<QString("/select,")<<path;
    process.startDetached("explorer",lst);//进程名 参数列表 工作路径
}

void MainDialog::slot_deleteAllFileInfo()
{
    qDebug()<<__func__;
    //删除所有的文件项
    //ui->tb_file->clear();//清空所有文字-行数不变
    int rows=ui->tb_file->rowCount();
    QTableWidgetItem* item0;
    QTableWidgetItem* item1;
    for(int i=rows-1;i>=0;i--){
        item0=ui->tb_file->takeItem(i,0);
        delete item0;
        item0=nullptr;
        for(int j=1;j<3;j++){
            item1=ui->tb_file->takeItem(i,j);
            delete item1;
            item1=nullptr;
        }
        ui->tb_file->removeRow(i);
    }
}

void MainDialog::slot_deleteAllShare()
{
    //删除所有的分享列表
    qDebug()<<__func__;
    //name time size link
    int rows=ui->tb_share->rowCount();
    QTableWidgetItem* item1;
    for(int i=rows-1;i>=0;i--){
        for(int j=0;j<4;j++){
            item1=ui->tb_share->takeItem(i,j);
            delete item1;
            item1=nullptr;
        }
        ui->tb_file->removeRow(i);
    }
}

void MainDialog::slot_insertAllShare(STRU_MY_SHARE_FILE *shareList, int listCount)
{
    //插入所有的分享列表
    qDebug()<<__func__;
    //1.循环遍历
    QTableWidgetItem *item0=nullptr;
    QTableWidgetItem *item1=nullptr;
    QTableWidgetItem *item2=nullptr;
    QTableWidgetItem *item3=nullptr;
    //name time size link
     ui->tb_share->setRowCount(listCount);
    for(int i=0;i<listCount;i++){
        //1. 新增一行 获取当前行+1 设置行数
        item0=new QTableWidgetItem(shareList[i].name);
        item1=new QTableWidgetItem(shareList[i].time);
        item2=new QTableWidgetItem(FileInfo::getSize(shareList[i].size));
        item3=new QTableWidgetItem(QString::number(shareList[i].shareLink));
        //2. 设置这一行的每一列的控件（添加对象）
        ui->tb_share->setItem(i,0,item0);
        ui->tb_share->setItem(i,1,item1);
        ui->tb_share->setItem(i,2,item2);
        ui->tb_share->setItem(i,3,item3);
    }
}

void MainDialog::slot_updateUploadFileProgress(int timestamp, int pos)
{
    qDebug()<<__func__;
    //更新进度条
    //1.遍历所有项 第0列
    //法1：获取到当前多少行，固定行数遍历<-(使用)
    //法2：使用count做循环条件，所有项清空之后可以作为循环结束条件
    int rows=ui->tb_upload->rowCount();
    for(int i=0;i<rows;i++){
        //2.取到每一个文件的时间信息戳，比对是否一致
        MytablewigetItem* item0=(MytablewigetItem*)ui->tb_upload->item(i,0);
        //3.一致，更新进度
        if(item0->m_file.timestamp==timestamp){
            QProgressBar* item4=(QProgressBar*)ui->tb_upload->cellWidget(i,4);
            item4->setValue(pos);
            item0->m_file.pos=pos;
            //4.看是否结束
            if(item4->value()>=item4->maximum()){
                //是-删除该项-添加到完成
                slot_insertTbComplete(item0->m_file,"upload");
                slot_deleteUploadFileByRow(i);
                return;
            }
        }

    }

}

void MainDialog::slot_updateDownloadFileProgress(int timestamp, int pos)
{
    //更新下载进度条
    qDebug()<<__func__;
    //1.遍历所有项 第0列
    //法1：获取到当前多少行，固定行数遍历<-(使用)
    //法2：使用count做循环条件，所有项清空之后可以作为循环结束条件
    int rows=ui->tb_download->rowCount();
    for(int i=0;i<rows;i++){
        //2.取到每一个文件的时间信息戳，比对是否一致
        MytablewigetItem* item0=(MytablewigetItem*)ui->tb_download->item(i,0);
        //3.一致，更新进度
        if(item0->m_file.timestamp==timestamp){
            QProgressBar* item4=(QProgressBar*)ui->tb_download->cellWidget(i,4);
            item4->setValue(pos);
            item0->m_file.pos=pos;
            //4.看是否结束
            if(item4->value()>=item4->maximum()){
                //是-删除该项-添加到完成
                slot_insertTbComplete(item0->m_file,"download");
                slot_deleteDownloadFileByRow(i);
                return;
            }
        }

    }
}

void MainDialog::slot_deleteUploadFileByRow(int row)
{
    qDebug()<<__func__;
    //删除uploadfile中行
    QTableWidgetItem *item0;
    QWidget *item4;
    for(int i=0;i<=3;i++){
        item0=ui->tb_upload->takeItem(row,i);
        delete item0;
        item0=nullptr;
    }
    for(int i=4;i<=5;i++){
        item4=ui->tb_upload->cellWidget(row,i);
        delete item4;
        item4=nullptr;
    }
    ui->tb_upload->removeRow(row);

}

void MainDialog::slot_deleteDownloadFileByRow(int row)
{
    qDebug()<<__func__;
    //删除下载的行
    //删除uploadfile中行
    QTableWidgetItem *item0;
    QWidget *item4;
    for(int i=0;i<=3;i++){
        item0=ui->tb_download->takeItem(row,i);
        delete item0;
        item0=nullptr;
    }
    for(int i=4;i<=5;i++){
        item4=ui->tb_download->cellWidget(row,i);
        delete item4;
        item4=nullptr;
    }
    ui->tb_download->removeRow(row);
}

void MainDialog::slot_insertFileInfo(FileInfo& file)
{
    qDebug()<<__func__;
    //1.插入行
    int rows=ui->tb_file->rowCount();
    ui->tb_file->setRowCount(rows+1);
    //2. 插入行数据
    MytablewigetItem* item0=new MytablewigetItem;
    QTableWidgetItem* item1=new QTableWidgetItem(file.time);
    QTableWidgetItem* item2=new QTableWidgetItem(file.getSize(file.size));
    item0->slot_setFile(file);
    ui->tb_file->setItem(rows,0,item0);
    ui->tb_file->setItem(rows,1,item1);
    ui->tb_file->setItem(rows,2,item2);
}


void MainDialog::on_tb_file_cellClicked(int row, int column)
{
    //选择单元格
    MytablewigetItem* item0=(MytablewigetItem*)ui->tb_file->item(row,0);
    if(item0->checkState()==Qt::Checked){
        item0->setCheckState(Qt::Unchecked);
    }else{
        item0->setCheckState(Qt::Checked);
    }
}


void MainDialog::on_tb_file_customContextMenuRequested(const QPoint &pos)
{
    //表格鼠标右键
    //1. 在鼠标右键位置显示菜单
    m_menuFileInfo.exec(QCursor::pos());
}

void MainDialog::slot_action_dowloadFile(bool flag)
{
    qDebug()<<__func__;
    //遍历列表
    int rows=ui->tb_file->rowCount();
    MytablewigetItem* item0;
    //获取当前目录
    QString dir=ui->lb_path->text();
    QString type="";
    for(int i=0;i<rows;i++){
        //看选中的文件项
        item0=(MytablewigetItem*)ui->tb_file->item(i,0);
        if(item0->checkState()==Qt::Checked){
            //列表中有这个文件，不能开始 过滤

            //获取文件类型
            type=item0->m_file.type;
            //发信号 下文件/文件夹
            if(type=="file"){
                Q_EMIT sig_downloadFile(item0->m_file.fileid,dir);
            }else if(type=="folder"){
                Q_EMIT sig_downloadFolder(item0->m_file.fileid,dir);
            }
        }
    }


}

void MainDialog::slot_action_shareFile(bool flag)
{
    //分享文件
    qDebug()<<__func__;
    //1.申请数组
    QVector<int> fileidArr;
    //2.遍历所有项
    int rows=ui->tb_file->rowCount();
    MytablewigetItem* item0=nullptr;
    for(int i=0;i<rows;i++){
        item0=(MytablewigetItem*)ui->tb_file->item(i,0);
        //3.看是否打勾
        if(item0->checkState()==Qt::Checked){
            //4.添加到数组中
            fileidArr.push_back(item0->m_file.fileid);
        }
    }
    //5.发送信号
    Q_EMIT sig_shareFile(fileidArr,ui->lb_path->text());
}

void MainDialog::slot_action_deleteFile(bool flag)
{
    qDebug()<<__func__;
}


void MainDialog::on_tb_file_cellDoubleClicked(int row, int column)
{
    //双击行进行跳转进如文件夹
    //1.文件类型判断
    MytablewigetItem* item0=(MytablewigetItem*)ui->tb_file->item(row,0);
    if(item0->m_file.type=="folder"){
        //2.文件夹路径拼接
        QString dir=ui->lb_path->text()+item0->m_file.name+"/";
        //3.更新当前目录-刷新文件列表
        ui->lb_path->setText(dir);
        Q_EMIT sig_changeDir( dir);
    }

}


void MainDialog::on_pb_last_clicked()
{
    //点击路径回退
    //1.获取当前目录
    QString curDir=ui->lb_path->text();
    //2.是否是根目录-不是
    if(curDir=="/")return;
    //3.找最右边的"/",从它往左寻找第二个"/"
    //.left()从左边开始取固定长度字符串
    curDir=curDir.left(curDir.lastIndexOf("/"));
    //新目录=第二个"/"+左边所有字符串
    curDir=curDir.left(curDir.lastIndexOf("/")+1);
    //4.更新目录
    ui->lb_path->setText(curDir);
    Q_EMIT sig_changeDir(curDir);

}

