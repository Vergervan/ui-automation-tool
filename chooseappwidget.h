#ifndef CHOOSEAPPWIDGET_H
#define CHOOSEAPPWIDGET_H

#include <QWidget>
#include <QString>
#include <QMap>
#include <QListWidgetItem>
#include <QString>
#include "windows.h"
#include "tlhelp32.h"
#include "actions.h"

namespace Ui {
class ChooseAppWidget;
}

class ChooseAppWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChooseAppWidget(QWidget *parent = nullptr);
    ~ChooseAppWidget();
    inline void setItem(QListWidgetItem* item){
        this->item = item;
    }

private slots:
    void on_chooseButton_clicked();
signals:
    void appInfoEvent(QListWidgetItem*, HandleData);
private:
    Ui::ChooseAppWidget *ui;
    void fillList();

    QMap<QListWidgetItem*, HandleData> handleList;
    QListWidgetItem* item = nullptr;

    HandleData findMainWindow(unsigned long processId);
    static BOOL CALLBACK enumWindowsCallback(HWND handle, LPARAM lParam);
    static BOOL isMainWindow(HWND handle);
};

#endif // CHOOSEAPPWIDGET_H
