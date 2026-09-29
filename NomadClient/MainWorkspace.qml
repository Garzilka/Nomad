import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
visible: true
anchors.fill: parent

// Переменная для отслеживания режима (chat, boards, docs)
property string currentMode: "chat"

    RowLayout {
        anchors.fill: parent
        spacing: 0
        // ==========================================
        // 1. СПИСОК ПРОСТРАНСТВ (Самая левая панель)
        // ==========================================
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 72
            color: "#1e1f22"

            ColumnLayout {
                anchors.fill: parent
                anchors.topMargin: 12
                spacing: 8

                // Иконка компании / HQ
                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    width: 48
                    height: 48
                    radius: 16
                    color: "#5865f2"
                    Text {
                        text: "💼";
                        color: "white";
                        anchors.centerIn: parent;
                        font.pixelSize: 20
                    }
                }

                Rectangle { Layout.alignment: Qt.AlignHCenter; width: 32; height: 2; color: "#35363c" }

                // Список отделов
                Repeater {
                    model: ["IT", "HR", "MKT"]
                    delegate: Rectangle {
                        Layout.alignment: Qt.AlignHCenter
                        width: 48
                        height: 48
                        radius: 24
                        color: "#313338"
                        Text { text: modelData; color: "white"; anchors.centerIn: parent; font.bold: true }
                    }
                }
                Item { Layout.fillHeight: true }
            }
        }

        // ==========================================
        // 2. СОВМЕЩЕННАЯ ПАНЕЛЬ: ТАБ-БАР (ВВЕРХУ) + СПИСОК (СНИЗУ)
        // ==========================================
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 240
            color: "#2b2d31"

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // --- НОВЫЙ ВЕРХНИЙ TOOLBAR (Внутри левой панели) ---
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 48
                    color: "#232428" // Выделяем темным тоном, как шапку

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 8
                        anchors.rightMargin: 8
                        spacing: 4

                        // Кнопка: Чат
                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 32
                            flat: true
                            background: Rectangle {
                                color: currentMode === "chat" ? "#35373c" : "transparent"
                                radius: 4
                            }
                            text: "💬"
                            onClicked: currentMode = "chat"
                            ToolTip.visible: hovered
                            ToolTip.text: "Чаты"
                        }

                        // Кнопка: Доски
                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 32
                            flat: true
                            background: Rectangle {
                                color: currentMode === "boards" ? "#35373c" : "transparent"
                                radius: 4
                            }
                            text: "📋"
                            onClicked: currentMode = "boards"
                            ToolTip.visible: hovered
                            ToolTip.text: "Доски задач"
                        }

                        // Кнопка: Документы
                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 32
                            flat: true
                            background: Rectangle {
                                color: currentMode === "docs" ? "#35373c" : "transparent"
                                radius: 4
                            }
                            text: "📄"
                            onClicked: currentMode = "docs"
                            ToolTip.visible: hovered
                            ToolTip.text: "База знаний"
                        }
                    }
                }

                // Разделительная линия под таб-баром
                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: "#1f2023"
                }

                // Динамический список (Каналы / Доски / Статьи)
                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.topMargin: 8
                    clip: true

                    model: currentMode === "chat" ? ["# общий-чат", "# разработка", "🔊 Дейли-созвон"] :
                           currentMode === "boards" ? ["📊 Спринт текущий", "🎯 Бэклог продукта", "🐛 Баг-трекер"] :
                                                      ["📚 Онбординг", "🔐 Доступы и API", "🎨 Дизайн-система"]

                    delegate: ItemDelegate {
                        width: parent.width
                        height: 34
                        contentItem: Text {
                            text: modelData
                            color: hovered ? "#dbdee1" : "#949ba4"
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 13
                        }
                        background: Rectangle {
                            color: hovered ? "#35373c" : "transparent"
                            radius: 4
                            anchors.fill: parent
                            anchors.leftMargin: 8
                            anchors.rightMargin: 8
                        }
                    }
                }

                // Блок пользователя в самом низу
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 52
                    color: "#232428"
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        Rectangle { width: 32; height: 32; radius: 16; color: "#43b581" }
                        Text { text: "Разработчик"; color: "white"; font.bold: true; Layout.fillWidth: true }
                        Text { text: "⚙️"; color: "#b5bac1" }
                    }
                }
            }
        }

        // ==========================================
        // 3. ОСНОВНАЯ ЗОНА (Контентная область)
        // ==========================================
        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: currentMode === "chat" ? 0 : (currentMode === "boards" ? 1 : 2)

            // Слой 0: Интерфейс Чата
            Rectangle {
                color: "#313338"
                Item {
                    anchors.fill: parent
                    Text {
                        text: "💬 Здесь отображается Чат и сообщения";
                        color: "#949ba4";
                        font.pixelSize: 18;
                        anchors.centerIn: parent
                    }
                }
            }

            // Слой 1: Интерфейс Канбан-Доски (Kaiten style)
            Rectangle {
                color: "#313338"
                KanBan
                {

                }
            }
            // Слой 2: Текстовый редактор / Документы
            Rectangle {
                color: "#313338"
                DocumentEditor
                {

                }

            }
        }
    }
}