#ifndef AUTHORIZATIONWIDGET_H
#define AUTHORIZATIONWIDGET_H

#include <QWidget>

namespace Ui {
class AuthorizationWidget;
}

class QAuthorizationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QAuthorizationWidget(QWidget *parent = nullptr);
    ~QAuthorizationWidget();

Q_SIGNALS:
    void OnAuthorizationSuccess();

private:
    Ui::AuthorizationWidget *ui;

private slots:
    void AuthMessage();
    void RegMessage();

    void GoToReg();
    void GoToAuth();
};

#endif // AUTHORIZATIONWIDGET_H
