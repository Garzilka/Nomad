#ifndef AUTHORIZATIONWIDGET_H
#define AUTHORIZATIONWIDGET_H

#include <QWidget>

namespace Ui {
class AuthorizationWidget;
}

class AuthorizationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AuthorizationWidget(QWidget *parent = nullptr);
    ~AuthorizationWidget();

private:
    Ui::AuthorizationWidget *ui;
};

#endif // AUTHORIZATIONWIDGET_H
