#ifndef MYTABLEWIGETITEM_H
#define MYTABLEWIGETITEM_H
#include "common.h"
#include <QTableWidgetItem>

class MytablewigetItem : public QObject, public QTableWidgetItem
{
    Q_OBJECT
public:
    explicit MytablewigetItem();

private:
    friend class MainDialog;
    FileInfo m_file;
public slots:
    void slot_setFile(FileInfo& file);

signals:
};

#endif // MYTABLEWIGETITEM_H
