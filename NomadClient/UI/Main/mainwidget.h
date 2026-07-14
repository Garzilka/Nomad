#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE



class QMainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QMainWidget(QWidget *parent = nullptr);
    ~QMainWidget();

private:
    Ui::MainWidget *ui;

private slots:
    void SendMessage();
    void ReceiveMessage(const QString Message);
};

#endif // MAINWIDGET_H
