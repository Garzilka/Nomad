#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>

enum Pages {
    AuthorisationPage = 0,
    ChatPage = 1,
    SettingsPage = 2
};


void MainWindow::AuthComplete()
{
    ui->SW_Placement->setCurrentIndex(1);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    AuthWidget = new class QAuthorizationWidget(nullptr);
    connect(&QConnection::getInstance(), &QConnection::OnAuthComplete, this, &MainWindow::AuthComplete);

    QWidget *page1 = new QWidget();
    QVBoxLayout *layout1 = new QVBoxLayout(page1);
    layout1->addWidget(AuthWidget);

    MainWidget = new class QMainWidget(nullptr);
    QWidget *page2 = new QWidget();
    QVBoxLayout *layout2 = new QVBoxLayout(page2);
    layout2->addWidget(MainWidget);

    ui->SW_Placement->addWidget(page1);
    ui->SW_Placement->addWidget(page2);

    ui->SW_Placement->currentChanged(Pages::AuthorisationPage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

