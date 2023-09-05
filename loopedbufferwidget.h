#ifndef LOOPEDBUFFERWIDGET_H
#define LOOPEDBUFFERWIDGET_H

#include <QWidget>
#include <QTableWidgetItem>
#include "editpastewidget.h"

namespace Ui {
class LoopedBufferWidget;
}

class LoopedBufferWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoopedBufferWidget(QWidget *parent = nullptr);
    ~LoopedBufferWidget();
    void setItem(QListWidgetItem* item, const QStringList& list);

signals:
    void saveListEvent(QListWidgetItem*,QStringList);
private slots:
    void on_saveButton_clicked();

private:
    Ui::LoopedBufferWidget *ui;
    QListWidgetItem* item = nullptr;
};

#endif // LOOPEDBUFFERWIDGET_H
