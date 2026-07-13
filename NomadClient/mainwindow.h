#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Core/connection.h"

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
    Connection* MyConnection = nullptr;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void SendMessage();

};
#endif // MAINWINDOW_H
