import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import NomadClient

Item
{
    id: chatWorkSpaceRoot
    anchors.fill: parent

    ListModel
    {
        id: serverMembersModel
        ListElement { name: "Разработчик"; roleColor: "#f04747"; statusColor: "#23a55a"; customStatus: "В игре: Qt Creator"; avatarColor: "#5865f2" }
        ListElement { name: "Иван Дизайнер"; roleColor: "#faa61a"; statusColor: "#f04747"; customStatus: "Не беспокоить"; avatarColor: "#3ba55d" }
        ListElement { name: "Дмитрий (PM)"; roleColor: "#dbdee1"; statusColor: "#949ba4"; customStatus: ""; avatarColor: "#747f8d" }
    }

    RowLayout
    {
        anchors.fill: parent

        ColumnLayout
        {
            anchors.fill: parent
            spacing: 0

            // ==========================================================
            // 1. ШАПКА ЧАТА (Хедер канала)
            // ==========================================================
            Rectangle
            {
                Layout.fillWidth: true
                Layout.preferredHeight: 48
                color: "#313338"

                Rectangle
                {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 1
                    color: "#1f2023"
                }

                RowLayout
                {
                    anchors.fill: parent
                    anchors.leftMargin: 16
                    anchors.rightMargin: 16
                    spacing: 8

                    Text { text: "@"; color: "#80848e"; font.pixelSize: 20; font.bold: true }
                    // В будущем сюда передадим имя активного собеседника
                    Text { text: "Личные сообщения"; color: "#ffffff"; font.bold: true; font.pixelSize: 15 }
                    Rectangle { width: 1; height: 16; color: "#3f4147"; Layout.leftMargin: 8; Layout.rightMargin: 8 }
                    Text { text: "Безопасное сквозное шифрование чата Nomad"; color: "#949ba4"; font.pixelSize: 12; Layout.fillWidth: true; elide: Text.ElideRight }
                }
            }

            // ==========================================================
            // 2. ЛЕНТА СООБЩЕНИЙ
            // ==========================================================
            MessageList
            {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            // ==========================================================
            // 3. ПОЛЕ ВВОДА СООБЩЕНИЯ (Нижняя панель)
            // ==========================================================
            Rectangle
            {
                Layout.fillWidth: true
                Layout.preferredHeight: 68
                color: "#313338"

                Rectangle
                {
                    anchors.fill: parent
                    anchors.margins: 16
                    color: "#383a40"
                    radius: 8

                    RowLayout
                    {
                        anchors.fill: parent
                        anchors.leftMargin: 16
                        anchors.rightMargin: 12

                        TextField
                        {
                            id: messageInputField
                            Layout.fillWidth: true
                            placeholderText: "Написать сообщение..."
                            placeholderTextColor: "#646970"
                            color: "#dbdee1"
                            font.pixelSize: 14
                            background: Item {}

                            onAccepted:
                            {
                                if (text.trim() !== "")
                                {
                                    ChatManager.sendMessageFromUI(text.trim());
                                    text = ""
                                }
                            }
                        }

                        Button
                        {
                            implicitWidth: 32
                            implicitHeight: 32
                            flat: true
                            HoverHandler { cursorShape: Qt.PointingHandCursor }
                            background: Rectangle { color: parent.hovered ? "#4752c4" : "#5865f2"; radius: 4 }
                            contentItem: Text { text: "➔"; color: "white"; font.bold: true; font.pixelSize: 14; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                            onClicked: messageInputField.accepted()
                        }
                    }
                }
            }
        }

        Members
        {
            membersData: serverMembersModel
            Layout.fillHeight: true
            Layout.preferredWidth: 240
            visible: FriendsManager.State !== 1
        }
    }
}
