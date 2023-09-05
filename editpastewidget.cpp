#include "editpastewidget.h"
#include "ui_editpastewidget.h"

EditPasteWidget::EditPasteWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditPasteWidget)
{
    setWindowFlags(Qt::Window);
    setWindowModality(Qt::WindowModal);
    ui->setupUi(this);
    ui->textEdit->setFocus();
}

EditPasteWidget::~EditPasteWidget()
{
    delete ui;
}

void EditPasteWidget::save()
{
    if(this->intCheck)
    {
        bool ok = false;
        ui->textEdit->text().toInt(&ok);
        if(!ok)
        {
            ui->textEdit->setStyleSheet("QLineEdit {color: red;}");
            return;
        }
    }
    emit editTextEvent(this->item, this->type, ui->textEdit->text());
    this->close();
}

void EditPasteWidget::on_saveButton_clicked()
{
    save();
}

void EditPasteWidget::on_textEdit_returnPressed()
{
    save();
}

void EditPasteWidget::setDefaultText(QString text)
{
    ui->textEdit->setText(text);
}


