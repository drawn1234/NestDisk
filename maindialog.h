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
    void sig_addFolder(QString name,QString dir);
    void sig_changeDir(QString dir);
    void sig_uploadFolder(QString path,QString dir);
    void sig_shareFile(QVector<int>& fileidArr,QString dir);
    void sig_getShareByLink(QString dir,int link);
    void sig_deleteFile(QVector<int>& fileidArr,QString dir);
    void sig_pauseUp(int timeStamp,bool isPause);
public:
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();
    void closeEvent(QCloseEvent* event);
private slots:
    //界面交互槽
    void on_pb_file_clicked();
    void on_pb_trans_clicked();
    void on_pb_share_clicked();
    //右上角菜单槽
    void on_pb_add_clicked();
    void slot_action_addFolder(bool flag);
    void slot_action_uploadFile(bool flag);
    void slot_action_uploadFolder(bool flag);
    //复选
    void on_tb_file_cellClicked(int row, int column);
    //右键菜单槽
    void on_tb_file_customContextMenuRequested(const QPoint &pos);
    void slot_action_dowloadFile(bool flag);
    void slot_action_shareFile(bool flag);
    void slot_action_deleteFile(bool flag);
    void slot_action_getShare(bool flag);
    void on_tb_file_cellDoubleClicked(int row, int column);
    //上一路径
    void on_pb_last_clicked();
    //文件上传/下载右键菜单栏
    void slot_action_pauseAllUp(bool flag);
    void slot_action_pauseAllDown(bool flag);
    void slot_action_startAllUp(bool flag);
    void slot_action_startAllDown(bool flag);

    void slot_action_pauseUp(bool flag);
    void slot_action_pauseDown(bool flag);
    void slot_action_startUp(bool flag);
    void slot_action_startDown(bool flag);

    void on_tb_upload_cellClicked(int row, int column);

    void on_tb_download_cellClicked(int row, int column);

    void on_tb_finished_cellClicked(int row, int column);

public slots:
    //界面控制槽
    void slot_insertUploadFile(FileInfo& file);
    void slot_insertDownloadFile(FileInfo& file);
    void slot_insertTbComplete(FileInfo& file,QString transType);
    void slot_updateUploadFileProgress(int timestamp,int pos);
    void slot_updateDownloadFileProgress(int timestamp,int pos);
    void slot_deleteUploadFileByRow(int row);
    void slot_deleteDownloadFileByRow(int row);
    void slot_insertFileInfo(FileInfo& file);
    void slot_openPath(bool flag);
    void slot_deleteAllFileInfo();
    void slot_deleteAllShare();
    void slot_insertAllShare(STRU_MY_SHARE_FILE* shareList,int listCount);

private:
    Ui::MainDialog *ui;
    QMenu m_menuAddFile;
    QMenu m_menuFileInfo;
    QMenu m_menuUploadFile;
    QMenu m_menuDownloadFile;

};
#endif // MAINDIALOG_H
