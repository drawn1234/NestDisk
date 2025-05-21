#ifndef MAINDIALOG_H
#define MAINDIALOG_H
#include<QCloseEvent>
#include <QDialog>

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
public:
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();
    void closeEvent(QCloseEvent* event);
private:
    Ui::MainDialog *ui;
};
#endif // MAINDIALOG_H
