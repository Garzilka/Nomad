import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

Item
{
    readonly property color colorBg: "#1e1f22"
    readonly property color colorAuth: "#313338"
    readonly property color colorText: "#b5bac1"

    id: loginScreen
    anchors.fill: parent

    property bool isLoginMode: true

    Rectangle
    {
        anchors.fill: parent
        color: colorBg

        Rectangle
        {
            id: authCard
            anchors.centerIn: parent
            width: 480
            height: isLoginMode ? 400 : 480
            color: colorAuth
            radius: 8

            Behavior on height { NumberAnimation { duration: 150 } }

            ColumnLayout
            {
                anchors.fill: parent
                anchors.margins: 32
                spacing: 20


                // ==========================================
                // Заголовки
                // ==========================================
                ColumnLayout
                {
                    Layout.fillWidth: true
                    spacing: 8

                    Text
                    {
                        text: isLoginMode ? "С возвращением!" : "Создать учетную запись"
                        font.pixelSize: 24
                        font.bold: true
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                        Layout.fillWidth: true
                    }

                    Text
                    {
                        text: isLoginMode ? "Мы рады снова вас видеть!" : "Пожалуйста, заполните форму ниже"
                        font.pixelSize: 14
                        color: colorText
                        horizontalAlignment: Text.AlignHCenter
                        Layout.fillWidth: true
                    }
                }

                // ==========================================
                // Поля ввода
                // ==========================================
                ColumnLayout
                {
                    spacing: 16
                    Layout.fillWidth: true

                    // Поле EMAIL
                    ColumnLayout
                    {
                        spacing: 8
                        Layout.fillWidth: true
                        visible: !loginScreen.isLoginMode

                        Text
                        {
                            text: "АДРЕС ЭЛЕКТРОННОЙ ПОЧТЫ"
                            font.pixelSize: 12
                            font.bold: true
                            color: colorText
                        }

                        TextField
                        {
                            id: emailField
                            Layout.fillWidth: true
                            implicitHeight: 40
                            color: "#f2f3f5"
                            placeholderTextColor: "#949ba4"
                            background: Rectangle
                            {
                                color: "#1e1f22"
                                radius: 4
                                border.color: emailField.activeFocus ? "#5865f2" : "transparent"
                                border.width: 1
                            }
                        }
                    }

                    // ==========================================
                    // Поле ИМЯ ПОЛЬЗОВАТЕЛЯ
                    // ==========================================
                    ColumnLayout
                    {
                        spacing: 8
                        Layout.fillWidth: true

                        Text
                        {
                            text: loginScreen.isLoginMode ? "ДАННЫЕ ВХОДА (ИМЯ)" : "ИМЯ ПОЛЬЗОВАТЕЛЯ"
                            font.pixelSize: 12
                            font.bold: true
                            color: colorText
                        }

                        TextField
                        {
                            id: usernameField
                            Layout.fillWidth: true
                            implicitHeight: 40
                            color: "#f2f3f5"
                            placeholderTextColor: "#949ba4"
                            background: Rectangle {
                                color: "#1e1f22"
                                radius: 4
                                border.color: usernameField.activeFocus ? "#5865f2" : "transparent"
                                border.width: 1
                            }
                        }
                    }

                    // ==========================================
                    // Поле ПАРОЛЬ
                    // ==========================================
                    ColumnLayout
                    {
                        spacing: 8
                        Layout.fillWidth: true

                        Text
                        {
                            text: "ПАРОЛЬ"
                            font.pixelSize: 12
                            font.bold: true
                            color: colorText
                        }

                        TextField
                        {
                            id: passwordField
                            Layout.fillWidth: true
                            implicitHeight: 40
                            echoMode: TextInput.Password
                            color: "#f2f3f5"
                            placeholderTextColor: "#949ba4"
                            background: Rectangle
                            {
                                color: "#1e1f22"
                                radius: 4
                                border.color: passwordField.activeFocus ? "#5865f2" : "transparent"
                                border.width: 1
                            }
                        }
                    }
                }

                // ==========================================
                // Кнопки управления
                // ==========================================
                ColumnLayout
                {
                    spacing: 12
                    Layout.fillWidth: true

                    Button
                    {
                        Layout.fillWidth: true
                        implicitHeight: 44

                        HoverHandler
                        {
                            cursorShape: Qt.PointingHandCursor
                        }

                        background: Rectangle
                        {
                            color: parent.hovered ? "#4752c4" : "#5865f2"
                            radius: 3
                        }

                        contentItem: Text
                        {
                            text: loginScreen.isLoginMode ? "Вход" : "Продолжить"
                            color: "#ffffff"
                            font.pixelSize: 16
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked:
                        {
                            if (loginScreen.isLoginMode)
                            {
                                ClientConnection.authorization(usernameField.text, passwordField.text);
                                console.log("Попытка входа:", usernameField.text)
                            }
                            else
                            {
                                ClientConnection.registration(usernameField.text, emailField.text, passwordField.text);
                                console.log("Регистрация пользователя:", usernameField.text, "Email:", emailField.text)
                            }
                        }
                    }
                    RowLayout
                    {
                        Layout.alignment: Qt.AlignLeft
                        spacing: 4
                        Text
                        {
                            text: loginScreen.isLoginMode ? "Нужна учетная запись?" : "Уже есть учетная запись?"
                            color: "#949ba4"
                            font.pixelSize: 13
                        }
                        Text
                        {
                            text: loginScreen.isLoginMode ? "Зарегистрироваться" : "Войти"
                            color: "#00a8fc"
                            font.pixelSize: 13
                            MouseArea
                            {
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked:
                                {
                                    loginScreen.isLoginMode = !loginScreen.isLoginMode
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
