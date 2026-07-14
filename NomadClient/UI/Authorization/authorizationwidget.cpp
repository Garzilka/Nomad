#include "authorizationwidget.h"
#include "ui_authorizationwidget.h"
#include "Core/connection.h"
#include "Core/Data/CoreData.h"

QAuthorizationWidget::QAuthorizationWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AuthorizationWidget)
{
    ui->setupUi(this);

    connect(ui->BTN_RegSuc, SIGNAL(released()), this, SLOT(RegMessage()));
    connect(ui->BTN_AuthSuc, SIGNAL(released()), this, SLOT(AuthMessage()));

    connect(ui->BTN_GoToReg, SIGNAL(released()), this, SLOT(GoToReg()));
    connect(ui->BTN_GoToAuth, SIGNAL(released()), this, SLOT(GoToAuth()));
}

QAuthorizationWidget::~QAuthorizationWidget()
{
    delete ui;
}

void QAuthorizationWidget::RegMessage()
{
    QConnection::getInstance().sendMessage(new SAuthorizationData("", ETypeOfMessage::Registration, ui->ET_RegLogin->text(), ui->ET_RegPas->text()));
}

void QAuthorizationWidget::AuthMessage()
{
    QConnection::getInstance().sendMessage(new SAuthorizationData("", ETypeOfMessage::Login, ui->ET_AutLogin->text(), ui->ET_AuthPas->text()));
}

void QAuthorizationWidget::GoToReg()
{
    ui->SW_Switcher->setCurrentIndex(0);
}

void QAuthorizationWidget::GoToAuth()
{
    ui->SW_Switcher->setCurrentIndex(1);
}
