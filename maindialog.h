#ifndef MAINDIALOG_H
#define MAINDIALOG_H
#include<QCloseEvent>
#include <QDialog>
#include <QMenu>
#include "common.h"
#include"mytablewigetitem.h"
#include "packdef.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainDialog;
}
QT_END_NAMESPACE

class MainDialog : public QDialog
{
    Q_OBJECT
signals:
    void sig_close();
    void sig_uploadFile(QString path,QString dir);
    void sig_downloadFile(int fileid,QString dir);
    void sig_downloadFolder(int fileid,QString dir);
public:
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();
    void closeEvent(QCloseEvent* event);
private slots:
    //界面交互槽
    void on_pb_file_clicked();
    void on_pb_trans_clicked();
    void on_pb_share_clicked();
    //下载菜单槽
    void on_pb_add_clicked();
    void slot_action_addFolder(bool flag);
    void slot_action_uploadFile(bool flag);
    void slot_action_uploadFolder(bool flag);
    //复选
    void on_tb_file_cellClicked(int row, int column);
    //文件菜单槽
    void on_tb_file_customContextMenuRequested(const QPoint &pos);
    void slot_action_dowloadFile(bool flag);
    void slot_action_shareFile(bool flag);
    void slot_action_deleteFile(bool flag);
public slots:
    //界面控制槽
    void slot_insertUploadFile(FileInfo& file);
    void slot_insertUploadComplete(FileInfo& file);
    void slot_updateFileProgress(int timestamp,int pos);
    void slot_deleteUploadFileByRow(int row);
    void slot_insertFileInfo(FileInfo& file);
private:
    Ui::MainDialog *ui;
    QMenu m_menuAddFile;
    QMenu m_menuFileInfo;

};
#endif // MAINDIALOG_H
