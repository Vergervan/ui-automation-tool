#include "loopedbufferwidget.h"
#include "ui_loopedbufferwidget.h"

LoopedBufferWidget::LoopedBufferWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoopedBufferWidget)
{
    setWindowFlags(Qt::Window);
    setWindowModality(Qt::WindowModal);
    ui->setupUi(this);
}

LoopedBufferWidget::~LoopedBufferWidget()
{
    delete ui;
}


void LoopedBufferWidget::on_saveButton_clicked()
{
    auto list = ui->textEdit->toPlainText().split('\n');
    list.removeAll("");
    emit saveListEvent(this->item, list);
    this->close();
}

void LoopedBufferWidget::setItem(QListWidgetItem* item, const QStringList& list)
{
    this->item = item;
    ui->textEdit->setText(list.join('\n'));
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit->setTextCursor(cursor);
}

