#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <UI/Messager/message.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->BTN_Send, SIGNAL(released()), this, SLOT(SendMessage()));

    MyConnection = new Connection(nullptr);
    MyConnection->connectToServer("127.0.0.1", 55);
    connect(MyConnection, &Connection::OnNewMessage, this, &MainWindow::ReceiveMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete MyConnection;
}

void MainWindow::ReceiveMessage(const QString Message)
{
    ui->VB_MessageList->layout()->addWidget(new MessageWidget(Message));
}

void MainWindow::SendMessage()
{
    QString Message = ui->ET_Text->toPlainText();
    if(MyConnection->sendMessage(Message))
    {
        ui->VB_MessageList->layout()->addWidget(new MessageWidget(Message));
        ui->ET_Text->clear();
    }
}
