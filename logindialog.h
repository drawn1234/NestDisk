#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class loginDialog;
}

class loginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit loginDialog(QWidget *parent = nullptr);
    ~loginDialog();

signals:
    void SIG_registerCommit(QString tel,QString pass,QString name);
    void SIG_loginCommit(QString tel,QString pass);
private slots:
    void on_pb_submit_regist_clicked();

    void on_pb_clear_regist_clicked();

    void on_pb_submit_clicked();

    void on_pb_clear_clicked();

private:
    Ui::loginDialog *ui;
};

#endif // LOGINDIALOG_H
