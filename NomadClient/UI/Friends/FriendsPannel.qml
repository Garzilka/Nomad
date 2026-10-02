import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import NomadClient

// ИСПРАВЛЕНО: Корневым элементом сделан Rectangle вместо Item.
// Это автоматически дает панели цвет и правильное поведение размеров внутри слоев.
Rectangle
{
    id: friendsPannelRoot

    color: "#2b2d31" // Цвет панели Discord
    implicitWidth: 240
    implicitHeight: parent ? parent.height : 500

    // Временная модель данных для списка друзей
    ListModel
    {
        id: friendsModel
        ListElement { name: "Дмитрий (PM)"; status: "В сети"; statusColor: "#23a55a"; avatarColor: "#f5a623"; currentActivity: "В Kaiten двигает карточки" }
        ListElement { name: "Raider"; status: "Не беспокоить"; statusColor: "#f23f43"; avatarColor: "#5865f2"; currentActivity: "Пишет криптобиблиотеку" }
        ListElement { name: "Иван Дизайнер"; status: "Не в сети"; statusColor: "#80848e"; avatarColor: "#e3e5e8"; currentActivity: "" }
    }

    // Храним имя активного в данный момент чата, чтобы правильно подсвечивать нужную строчку
    property string activeChatName: ""

    ColumnLayout
    {
        anchors.fill: parent
        spacing: 0

        // Поиск
        Rectangle
        {
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            color: "#2b2d31"

            Rectangle
            {
                anchors.fill: parent
                anchors.margins: 8
                color: "#1e1f22"
                radius: 4

                RowLayout
                {
                    anchors.fill: parent
                    anchors.leftMargin: 8
                    anchors.rightMargin: 8

                    TextField
                    {
                        id: searchField
                        Layout.fillWidth: true
                        placeholderText: "Найти беседу..."
                        placeholderTextColor: "#949ba4"
                        color: "#dbdee1"
                        font.pixelSize: 13
                        background: Item {}
                    }
                    Text
                    {
                        text: "🔍";
                        color: "#949ba4";
                        font.pixelSize: 12
                    }
                }
            }
        }

        // ==========================================
        // Вкладка "Все друзья" (Общий список)
        // ==========================================
        ItemDelegate
        {
            id: friendsButton
            Layout.fillWidth: true
            Layout.preferredHeight: 40

            highlighted: FriendsManager.State === 0

            background: Rectangle
            {
                color: friendsButton.highlighted ? "#35373c" : (friendsButton.hovered ? "#35373c" : "transparent")
                radius: 4
                anchors.fill: parent
                anchors.margins: 4
            }

            contentItem: RowLayout
            {
                spacing: 12
                Text
                {
                    text: "🧑‍🤝‍🧑";
                    font.pixelSize: 16
                }
                Text
                {
                    text: "Друзья";
                    color: "#ffffff";
                    font.bold: true;
                    font.pixelSize: 13
                }
                Item
                {
                    Layout.fillWidth: true
                }
            }
            onClicked: {
                FriendsManager.State = 0;
                friendsPannelRoot.activeChatName = ""; // Сбрасываем активный чат при переходе к списку друзей
            }
        }

        // ==========================================
        // Кнопка "Заявки"
        // ==========================================
        ItemDelegate
        {
            id: requestsTabButton
            Layout.fillWidth: true
            Layout.preferredHeight: 40

            // Подсвечиваем кнопку, если состояние равно 3
            highlighted: FriendsManager.State === 3

            background: Rectangle
            {
                color: requestsTabButton.highlighted ? "#35373c" : (requestsTabButton.hovered ? "#35373c" : "transparent")
                radius: 4
                anchors.fill: parent
                anchors.margins: 4
            }

            contentItem: RowLayout
            {
                spacing: 12
                Text
                {
                    text: "🔔";
                    font.pixelSize: 16
                }
                Text
                {
                    text: "Заявки";
                    color: "#ffffff";
                    font.bold: true;
                    font.pixelSize: 13
                }
                Item
                {
                    Layout.fillWidth: true
                }
            }
            onClicked: {
                // Переводим стейт в 3 при клике
                FriendsManager.State = 3;
                friendsPannelRoot.activeChatName = ""; // Сбрасываем активный чат
            }
        }

        // ==========================================
        // Заголовок ЛС
        // ==========================================
        Text
        {
            text: "ЛИЧНЫЕ СООБЩЕНИЯ"
            color: "#949ba4"
            font.pixelSize: 11
            font.bold: true
            Layout.leftMargin: 12
            Layout.topMargin: 16
            Layout.bottomMargin: 8
        }

        // ==========================================
        // Список диалогов на левой панели
        // ==========================================
        ListView
        {
            id: directMessagesList
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: friendsModel
            clip: true
            spacing: 2

            delegate: ItemDelegate
            {
                id: dmDelegateItem
                width: directMessagesList.width
                height: 42

                highlighted: FriendsManager.State === 1 && friendsPannelRoot.activeChatName === model.name

                background: Rectangle
                {
                    color: dmDelegateItem.highlighted ? "#404249" : (dmDelegateItem.hovered ? "#35373c" : "transparent")
                    radius: 4
                    anchors.fill: parent
                    anchors.margins: 4
                }

                contentItem: RowLayout
                {
                    spacing: 10

                    // Аватарка со статус-точкой
                    Rectangle
                    {
                        width: 32; height: 32; radius: 16; color: avatarColor
                        Rectangle
                        {
                            width: 10; height: 10; radius: 5
                            color: statusColor
                            border.color: "#2b2d31"
                            border.width: 1.5
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                        }
                    }

                    Text
                    {
                        text: name
                        color: dmDelegateItem.hovered || dmDelegateItem.highlighted ? "#dbdee1" : "#949ba4"
                        font.pixelSize: 14
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }
                }

                onClicked:
                {
                    friendsPannelRoot.activeChatName = model.name; // Запоминаем, какой чат открыли
                    FriendsManager.State = 1;
                }
            }
        }
    }
}
