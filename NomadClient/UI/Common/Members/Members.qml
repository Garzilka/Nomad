import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle
{
    id: memberListRoot
    width: 240
    color: "#2b2d31"

    // Входное свойство для модели участников
    property var membersData

    ColumnLayout
    {
        anchors.fill: parent
        anchors.topMargin: 16
        anchors.leftMargin: 8
        anchors.rightMargin: 8
        spacing: 8

        // Заголовок списка с количеством участников
        Text
        {
            text: "УЧАСТНИКИ — " + (memberListRoot.membersData ? memberListRoot.membersData.count : 0)
            color: "#949ba4"
            font.bold: true
            font.pixelSize: 12
            font.letterSpacing: 0.5
            Layout.leftMargin: 8
            Layout.fillWidth: true
        }

        // ==========================================
        // Основной список участников
        // ==========================================
        ListView
        {
            id: membersListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: memberListRoot.membersData
            spacing: 2
            clip: true

            delegate: ItemDelegate
            {
                id: delegateItem
                width: membersListView.width
                height: 44

                background: Rectangle
                {
                    color: delegateItem.hovered ? "#35373c" : "transparent"
                    radius: 4
                }

                RowLayout
                {
                    anchors.fill: parent
                    anchors.leftMargin: 8
                    anchors.rightMargin: 8
                    spacing: 12

                    // Блок аватара со статус-точкой
                    Rectangle
                    {
                        Layout.preferredWidth: 32
                        Layout.preferredHeight: 32
                        radius: 16
                        color: model.avatarColor ? model.avatarColor : "#5865f2"

                        // Индикатор статуса (В сети / Отошел / Не беспокоить)
                        Rectangle
                        {
                            width: 10
                            height: 10
                            radius: 5
                            // Если статус не передан, красим по умолчанию в зеленый
                            color: model.statusColor ? model.statusColor : "#23a55a"
                            border.color: delegateItem.hovered ? "#35373c" : "#2b2d31"
                            border.width: 2
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                        }
                    }

                    // ==========================================
                    // Текстовый блок (Имя + Кастомный статус)
                    // ==========================================
                    ColumnLayout
                    {
                        spacing: 0
                        Layout.fillWidth: true

                        Text
                        {
                            text: model.name
                            color: model.roleColor ? model.roleColor : "#dbdee1" // Цвет имени зависит от роли
                            font.pixelSize: 14
                            font.bold: true
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }

                        // Отображается только если у пользователя есть активность/статус-текст
                        Text
                        {
                            text: model.customStatus ? model.customStatus : ""
                            color: "#949ba4"
                            font.pixelSize: 11
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                            visible: text !== ""
                        }
                    }
                }

                // Изменение курсора при наведении на участника
                HoverHandler
                {
                    cursorShape: Qt.PointingHandCursor
                }
            }
        }
    }
}
