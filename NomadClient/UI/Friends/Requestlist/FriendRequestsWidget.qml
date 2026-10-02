import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle
{
    id: requestsWidgetRoot
    color: "transparent"

    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 30
        spacing: 20

        Text {
            text: "ЗАЯВКИ В ДРУЗЬЯ"
            color: "#ffffff"
            font.bold: true
            font.pixelSize: 16
        }

        Item
        {
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Заглушка, если список пуст
            Rectangle
            {
                anchors.fill: parent
                color: "transparent"
                border.color: "#2b2d31"
                border.width: 1
                radius: 8
                visible: requestsListView.count === 0

                ColumnLayout
                {
                    anchors.centerIn: parent
                    spacing: 12

                    Text {
                        text: "⌛ У вас нет ожидающих заявок"
                        color: "#949ba4"
                        font.pixelSize: 15
                        Layout.alignment: Qt.AlignHCenter
                    }
                }
            }

            // Список заявок. Подключаем к той же C++ модели,
            // либо выделим под нее отдельное свойство, если списки разделены.
            // Предположим, что сервер шлет их в общую модель FriendsManager.pendingResults
            // или ты используешь отфильтрованный список.
            ListView
            {
                id: requestsListView
                anchors.fill: parent
                model: FriendsManager.pendingRequests // Сюда бэкенд должен выгрузить результаты sendPendingRequests
                spacing: 8
                clip: true
                visible: count > 0

                delegate: Rectangle
                {
                    width: requestsListView.width
                    height: 60
                    color: reqMouseArea.hovered ? "#35373c" : "#2b2d31"
                    radius: 8

                    HoverHandler { id: reqMouseArea }

                    RowLayout
                    {
                        anchors.fill: parent
                        anchors.leftMargin: 12
                        anchors.rightMargin: 12
                        spacing: 12

                        // Аватар
                        Rectangle
                        {
                            width: 38
                            height: 38
                            radius: 19
                            color: "#4f545c"

                            Text {
                                anchors.centerIn: parent
                                text: modelData["displayName"] && modelData["displayName"].length > 0
                                      ? modelData["displayName"].substring(0, 1).toUpperCase()
                                      : "?"
                                color: "#ffffff"
                                font.bold: true
                                font.pixelSize: 16
                            }
                        }

                        // Никнеймы и тип заявки (Входящая / Исходящая)
                        ColumnLayout
                        {
                            spacing: 2
                            Layout.fillWidth: true

                            Text {
                                text: modelData["displayName"] ? modelData["displayName"] : ""
                                color: "#ffffff"
                                font.bold: true
                                font.pixelSize: 14
                            }

                            Text {
                                // Выводим красивый статус направления заявки в стиле Discord
                                text: modelData["relationStatus"] === 1 ? "Входящая заявка в друзья" : "Исходящая заявка в друзья"
                                color: "#b5bac1"
                                font.pixelSize: 12
                            }
                        }

                        // Ряды кнопок действий управления
                        RowLayout
                        {
                            spacing: 8

                            // Кнопка "Принять" (Отображается ТОЛЬКО для входящих, relationStatus === 1)
                            Button
                            {
                                id: acceptBtn
                                visible: modelData["relationStatus"] === 1
                                text: "✔"

                                contentItem: Text {
                                    text: acceptBtn.text
                                    color: "#ffffff"
                                    font.bold: true
                                    font.pixelSize: 14
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }

                                background: Rectangle {
                                    implicitWidth: 36
                                    implicitHeight: 36
                                    color: acceptBtn.hovered ? "#1f6638" : "#248046" // Зеленый
                                    radius: 18 // Круглая кнопка
                                }

                                HoverHandler { cursorShape: Qt.PointingHandCursor }

                                onClicked: {
                                    // Вызываем C++ обработку: Action = 0 (Accept)
                                    // Так как мы поправили QCore, сетевой тип ResponseFriend (9) вызовет handleFriendResponseProcessing
                                    // Сформируем и отправим пакет:
                                    FriendsManager.handleFriendAction(modelData["guid"], 0);
                                }
                            }

                            // Кнопка "Отклонить" / "Отменить" (Отображается для всех типов заявок)
                            Button
                            {
                                id: declineBtn
                                text: "✖"

                                contentItem: Text {
                                    text: declineBtn.text
                                    color: "#ffffff"
                                    font.bold: true
                                    font.pixelSize: 12
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }

                                background: Rectangle {
                                    implicitWidth: 36
                                    implicitHeight: 36
                                    color: declineBtn.hovered ? "#952b2a" : "#da373c" // Красный
                                    radius: 18
                                }

                                HoverHandler { cursorShape: Qt.PointingHandCursor }

                                onClicked: {
                                    // Вызываем C++ обработку: Action = 1 (Reject/Cancel)
                                    FriendsManager.handleFriendAction(modelData["guid"], 1);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
