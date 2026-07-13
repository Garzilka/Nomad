#ifndef MESSAGE_H
#define MESSAGE_H

#include <QWidget>

namespace Ui {
class Message;
}

class MessageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MessageWidget(const QString InMessage, QWidget *parent = nullptr);
    ~MessageWidget();

private:
    Ui::Message *ui;
    QString Message;
};

#endif // MESSAGE_H
