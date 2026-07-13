#include "authorizationwidget.h"
#include "ui_authorizationwidget.h"

AuthorizationWidget::AuthorizationWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AuthorizationWidget)
{
    ui->setupUi(this);
}

AuthorizationWidget::~AuthorizationWidget()
{
    delete ui;
}
