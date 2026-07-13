#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->BTN_Send, SIGNAL(released()), this, SLOT(SendMessage()));

    MyConnection = new Connection(nullptr);
    MyConnection->connectToServer("127.0.0.1", 55);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete MyConnection;
}

void MainWindow::SendMessage()
{
    qDebug() << "test";
    QString Message = ui->ET_Text->toPlainText();
    if(MyConnection->sendMessage(Message))
    {
        ui->ET_Text->clear();
    }
}
