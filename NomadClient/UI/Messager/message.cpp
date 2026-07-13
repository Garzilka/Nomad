#include "message.h"
#include "ui_message.h"

MessageWidget::MessageWidget(const QString InMessage, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Message)
{
    Message = InMessage;
    ui->setupUi(this);
    ui->L_Text->setText(Message);
}

MessageWidget::~MessageWidget()
{
    delete ui;
}
