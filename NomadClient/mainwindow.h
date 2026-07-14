#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Core/connection.h"
#include "UI/Main/mainwidget.h"
#include "UI/Authorization/authorizationwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    QMainWidget* MainWidget = nullptr;
    QAuthorizationWidget* AuthWidget = nullptr;

private slots:
    void AuthComplete();
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

};
#endif // MAINWINDOW_H
