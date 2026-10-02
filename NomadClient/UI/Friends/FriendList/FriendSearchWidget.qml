import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle
{
    id: searchWidgetRoot
    color: "transparent"

    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 30
        spacing: 20

        ColumnLayout
        {
            Layout.fillWidth: true
            spacing: 8

            Text {
                text: "ДОБАВИТЬ В ДРУЗЬЯ"
                color: "#ffffff"
                font.bold: true
                font.pixelSize: 16
            }

            Text {
                text: "Вы можете добавить друга по его имени пользователя (Username)."
                color: "#b5bac1"
                font.pixelSize: 13
            }
        }

        // Поле ввода (Инпут поиска)
        Rectangle
        {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "#1e1f22"
            radius: 8
            border.color: inputField.activeFocus ? "#5865f2" : "transparent"
            border.width: 1

            RowLayout
            {
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 12
                spacing: 10

                TextField
                {
                    id: inputField
                    Layout.fillWidth: true
                    placeholderText: "Вы можете добавить друга по его имени пользователя (Username)..."
                    placeholderTextColor: "#949ba4"
                    color: "#ffffff"
                    font.pixelSize: 15
                    background: null

                    onAccepted: {
                        if (text.length > 0)
                        {
                            ClientConnection.searchFriend(text);
                        }
                    }
                }

                Button
                {
                    id: sendRequestBtn
                    text: "Поиск"
                    enabled: inputField.text.length > 0

                    contentItem: Text {
                        text: sendRequestBtn.text
                        color: "#ffffff"
                        font.bold: true
                        font.pixelSize: 14
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        implicitWidth: 100
                        implicitHeight: 36
                        color: sendRequestBtn.enabled ? (sendRequestBtn.hovered ? "#4752c4" : "#5865f2") : "#3f4147"
                        radius: 4
                    }

                    HoverHandler
                    {
                        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                    }
                    onClicked:
                    {
                        ClientConnection.searchFriend(inputField.text);
                    }
                }
            }
        }

        // ==========================================
        // Область результатов поиска
        // ==========================================
        Item
        {
            Layout.fillWidth: true
            Layout.fillHeight: true


            // ==========================================
            // Заглушка: если результатов поиска нет
            // ==========================================
            Rectangle
            {
                anchors.fill: parent
                color: "transparent"
                border.color: "#2b2d31"
                border.width: 1
                radius: 8
                visible: friendsListView.count === 0

                ColumnLayout
                {
                    anchors.centerIn: parent
                    spacing: 12

                    Text {
                        text: "🔍 Здесь будут отображаться результаты поиска"
                        color: "#949ba4"
                        font.pixelSize: 15
                        Layout.alignment: Qt.AlignHCenter
                    }

                    Text {
                        text: "Введи никнейм выше и нажми Enter или кнопку Поиск"
                        color: "#4e5058"
                        font.pixelSize: 13
                        Layout.alignment: Qt.AlignHCenter
                    }
                }
            }

            // ==========================================
            // Список результатов поиска из C++ FriendsManager
            // ==========================================
            ListView
            {
                id: friendsListView
                anchors.fill: parent
                model: FriendsManager.searchResults
                spacing: 8
                clip: true
                visible: count > 0

                delegate: Rectangle
                {
                    width: friendsListView.width
                    height: 60
                    color: itemMouseArea.hovered ? "#35373c" : "#2b2d31"
                    radius: 8

                    HoverHandler { id: itemMouseArea }

                    RowLayout
                    {
                        anchors.fill: parent
                        anchors.leftMargin: 12
                        anchors.rightMargin: 12
                        spacing: 12

                        // ==========================================
                        // Аватар пользователя
                        // ==========================================
                        Rectangle
                        {
                            width: 38
                            height: 38
                            radius: 19
                            color: "#5865f2"

                            Text
                            {
                                anchors.centerIn: parent
                                text: modelData["displayName"] && modelData["displayName"].length > 0
                                      ? modelData["displayName"].substring(0, 1).toUpperCase()
                                      : "?"
                                color: "#ffffff"
                                font.bold: true
                                font.pixelSize: 16
                            }
                        }

                        // ==========================================
                        // Текстовый блок (Имя и юзернейм)
                        // ==========================================
                        ColumnLayout
                        {
                            spacing: 2
                            Layout.fillWidth: true

                            Text
                            {
                                text: modelData["displayName"] ? modelData["displayName"] : ""
                                color: "#ffffff"
                                font.bold: true
                                font.pixelSize: 14
                            }

                            Text
                            {
                                text: modelData["username"] ? ("@" + modelData["username"]) : ""
                                color: "#b5bac1"
                                font.pixelSize: 12
                            }
                        }

                        // ==========================================
                        // Зеленая кнопка отправки запроса в друзья
                        // ==========================================
                        Button
                        {
                            id: addBtn
                            text: "Добавить"

                            contentItem: Text {
                                text: addBtn.text
                                color: "#ffffff"
                                font.bold: true
                                font.pixelSize: 12
                            }

                            background: Rectangle {
                                implicitWidth: 85
                                implicitHeight: 30
                                color: addBtn.hovered ? "#1f6638" : "#248046"
                                radius: 4
                            }

                            HoverHandler
                            {
                                cursorShape: addBtn.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                            }
                            onClicked:
                            {
                                ClientConnection.requestFriend(modelData["guid"], modelData["username"]);
                                addBtn.enabled = false;
                            }
                        }
                        Item
                        {
                            Layout.fillWidth: true
                        }
                    }
                }
            }
        }
    }
}
