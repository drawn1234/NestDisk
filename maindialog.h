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
public:
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();
    void closeEvent(QCloseEvent* event);
private slots:
    void on_pb_file_clicked();

    void on_pb_trans_clicked();

    void on_pb_share_clicked();

    void on_pb_add_clicked();

    void slot_action_addFolder(bool flag);
    void slot_action_uploadFile(bool flag);
    void slot_action_uploadFolder(bool flag);
public slots:
    void slot_insertUploadFile(FileInfo& file);
    void slot_insertUploadComplete(FileInfo& file);
    void slot_updateFileProgress(int timestamp,int pos);
    void slot_deleteUploadFileByRow(int row);
    void slot_insertFileInfo(FileInfo& file);
private:
    Ui::MainDialog *ui;
    QMenu m_menuAddFile;

};
#endif // MAINDIALOG_H
