import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

Item
{
    readonly property color colorBg: "#1e1f22"       // Глубокий темный фон
    readonly property color colorAuth: "#313338"    // Фон блоков
    readonly property color colorText: "#b5bac1"     // Основной текст


    id: loginScreen
    anchors.fill: parent

    property bool isLoading: true
    property bool minTimeElapsed: false
    property bool isLoginMode: true // Состояние окна: true — Вход, false — Регистрация

    function checkLoadingStatus()
    {
        if (ClientConnection.securitySuccessfully && minTimeElapsed)
        {
            loginScreen.isLoading = false;
        }
    }

    Connections
    {
        target: ClientConnection
        onSecuritySuccessfullyChanged:
        {
            console.log("C++ сообщил о готовности ключей:", ClientConnection.securitySuccessfully)
        }
    }
    // 2. ТАЙМЕР НА 2 СЕКУНДЫ
    Timer
    {
        id: startupTimer
        interval: 2000
        running: true // Стартует сразу при запуске экрана
        repeat: false
        onTriggered:
        {
            console.log("Минимальные 2 секунды анимации прошли.")
            if (ClientConnection.securitySuccessfully)
            {
                loginScreen.minTimeElapsed = true
                loginScreen.checkLoadingStatus()
            }
        }
    }


    Rectangle
    {
        anchors.fill: parent
        color: colorBg
        // ==========================================
        // БЛОК ЗАГРУЗКИ (Показывается, когда isLoading === true)
        // ==========================================
        ColumnLayout
        {
            id: loadingBlock
            anchors.centerIn: parent
            spacing: 24
            visible: isLoading
            opacity: visible ? 1.0 : 0.0

            // Плавное исчезновение/появление блока загрузки
            Behavior on opacity { NumberAnimation { duration: 250 } }

            // Кастомный анимированный спиннер (круг)
            Rectangle
            {
                Layout.alignment: Qt.AlignHCenter
                width: 50
                height: 50
                color: "transparent"
                border.color: "#35373c"
                border.width: 4
                radius: 25

                // Светящийся сектор загрузки
                Rectangle
                {
                    width: 50
                    height: 50
                    color: "transparent"
                    border.color: "#5865f2" // Фирменный цвет Discord
                    border.width: 4
                    radius: 25
                    clip: true

                    // Делаем из круга четверть сектора
                    Rectangle
                    {
                        width: 25
                        height: 25
                        color: colorBg
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                    }
                    Rectangle
                    {
                        width: 25
                        height: 25
                        color: colorBg
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                    }
                }

                // Бесконечная анимация вращения спиннера
                RotationAnimator on rotation
                {
                    from: 0
                    to: 360
                    duration: 1000
                    loops: Animation.Infinite
                    running: !NomadClient.SecuritySuccefully
                }
            }

            Text
            {
                text: "Установление безопасного соединения..."
                font.pixelSize: 16
                font.bold: true
                color: "#ffffff"
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }
        }

        // ==========================================
        // ЦЕНТРАЛЬНАЯ КАРТОЧКА ФОРМЫ (Когда isLoading === false)
        // ==========================================
        Rectangle
        {
            id: authCard
            anchors.centerIn: parent
            width: 480
            height: isLoginMode ? 400 : 480
            color: colorAuth
            radius: 8

            // Защита от кликов по невидимой форме во время загрузки
            visible: !isLoading
            opacity: visible ? 1.0 : 0.0

            // Плавное появление формы после окончания загрузки
            Behavior on opacity { NumberAnimation { duration: 300 } }
            Behavior on height { NumberAnimation { duration: 150 } }

            ColumnLayout
            {
                anchors.fill: parent
                anchors.margins: 32
                spacing: 20

                // Заголовки
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

                // Поля ввода
                ColumnLayout {
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

                    // Поле ИМЯ ПОЛЬЗОВАТЕЛЯ
                    ColumnLayout {
                        spacing: 8
                        Layout.fillWidth: true

                        Text {
                            text: loginScreen.isLoginMode ? "ДАННЫЕ ВХОДА (ИМЯ)" : "ИМЯ ПОЛЬЗОВАТЕЛЯ"
                            font.pixelSize: 12
                            font.bold: true
                            color: colorText
                        }

                        TextField {
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

                    // Поле ПАРОЛЬ
                    ColumnLayout {
                        spacing: 8
                        Layout.fillWidth: true

                        Text {
                            text: "ПАРОЛЬ"
                            font.pixelSize: 12
                            font.bold: true
                            color: colorText
                        }

                        TextField {
                            id: passwordField
                            Layout.fillWidth: true
                            implicitHeight: 40
                            echoMode: TextInput.Password
                            color: "#f2f3f5"
                            placeholderTextColor: "#949ba4"
                            background: Rectangle {
                                color: "#1e1f22"
                                radius: 4
                                border.color: passwordField.activeFocus ? "#5865f2" : "transparent"
                                border.width: 1
                            }
                        }
                    }
                }

                // Кнопки управления
                ColumnLayout
                {
                    spacing: 12
                    Layout.fillWidth: true

                    Button
                    {
                        Layout.fillWidth: true
                        implicitHeight: 44

                        HoverHandler {
                            cursorShape: Qt.PointingHandCursor
                        }

                        background: Rectangle {
                            color: parent.hovered ? "#4752c4" : "#5865f2"
                            radius: 3
                        }

                        contentItem: Text {
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