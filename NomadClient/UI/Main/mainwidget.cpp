#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "Core/connection.h"
#include <UI/Messager/message.h>

QMainWidget::QMainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    connect(ui->BTN_Send, SIGNAL(released()), this, SLOT(SendMessage()));
    connect(&QConnection::getInstance(), &QConnection::OnNewMessage, this, &QMainWidget::ReceiveMessage);
}

QMainWidget::~QMainWidget()
{
    delete ui;
}


void QMainWidget::ReceiveMessage(const QString Message)
{
    ui->VB_MessageList->layout()->addWidget(new MessageWidget(Message));
}

void QMainWidget::SendMessage()
{
    QString Message = ui->ET_Text->toPlainText();

    if(QConnection::getInstance().sendMessage(new SMessageData("", ETypeOfMessage::Message, "", Message)))
    {
        ui->VB_MessageList->layout()->addWidget(new MessageWidget(Message));
        ui->ET_Text->clear();
    }
}
