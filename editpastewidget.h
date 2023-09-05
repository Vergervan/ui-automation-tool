#ifndef EDITPASTEWIDGET_H
#define EDITPASTEWIDGET_H

#include <QWidget>
#include <QListWidgetItem>

#include "actions.h"

namespace Ui {
class EditPasteWidget;
}

class EditPasteWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditPasteWidget(QWidget *parent = nullptr);
    ~EditPasteWidget();
    inline void setItem(QListWidgetItem* item){
        this->item = item;
    }
    inline void setIntCheck(bool val){
        this->intCheck = val;
    }
    inline void setActionType(ActionType type){
        this->type = type;
    }
    void setDefaultText(QString text);
signals:
    void editTextEvent(QListWidgetItem*, ActionType, QString);
private slots:
    void on_saveButton_clicked();

    void on_textEdit_returnPressed();

private:
    Ui::EditPasteWidget *ui;
    QListWidgetItem* item = nullptr;
    ActionType type;
    bool intCheck = false;

    void save();
};

#endif // EDITPASTEWIDGET_H
