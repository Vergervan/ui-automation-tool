
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QObject>
#include <QMap>
#include <QListWidget>
#include <QListWidgetItem>
#include <QThread>
#include <vector>
#include "mouselogger.h"
#include <windows.h>
#include "editpastewidget.h"
#include "chooseappwidget.h"
#include "loopedbufferwidget.h"
#include "actions.h"
#include <set>
#include <string>
#include <sstream>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget

{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void sendMouseClick(unsigned long, int, int);
    void sendPasteText(HWND, QString);
    unsigned long getMouseInputDown(unsigned long);
    unsigned long getMouseInputUp(unsigned long);
private slots:
    void onMousePressed(unsigned long, int, int);
    void onKeyPressed(std::set<DWORD>);
    void onHotkeyPressed(unsigned long);
    void getEditText(QListWidgetItem*, ActionType, QString);
    void getProcessInfo(QListWidgetItem*, HandleData);
    void getLoopedBufferList(QListWidgetItem*, QStringList);
    void stopLoop();
    void updateLoopCount(int x);
    void simulateKeysPress(const std::vector<DWORD>& keys);

    void on_clickActionButton_clicked();

    void on_removeButton_clicked();

    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_startButton_clicked();

    void on_editButton_clicked();

    void on_pasteBufferButton_clicked();

    void on_openAppButton_clicked();

    void on_upItemButton_clicked();

    void on_downItemButton_clicked();

    void on_loopedBufferButton_clicked();

    void on_delayButton_clicked();

    void on_keyActionButton_clicked();

    void on_dublicateButton_clicked();

    void on_loopsEdit_textChanged(const QString &arg1);

    void on_loopsEdit_returnPressed();

private:
    Ui::Widget *ui;
    QMap<QListWidgetItem*, ActionInfo> actionMap;
    std::vector<int> loopBuffers;
    HWND mainHwnd = 0;
    bool isStopLoop = false;
    bool forceLoop = false;
    int loopCount = 0;

    void removeFromLoopBuffers(int x);
    int findMaxLoopBuffer();

    //QMetaObject::Connection mouseConnection;

};

#endif // WIDGET_H
