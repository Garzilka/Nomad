cmake_minimum_required(VERSION 3.16)

project(NomadClient VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(Qt6 COMPONENTS Core Network REQUIRED)
find_package(OpenSSL REQUIRED)

qt_standard_project_setup(REQUIRES 6.10)

# Content of downloaded design from Figma to Qt or similar
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()

qt_add_executable(appNomadClient
    main.cpp
)

qt_add_qml_module(appNomadClient
    URI NomadClient
    VERSION 1.0

    QML_FILES
        Main.qml
        UI/LoginScreen.qml
        UI/MainWorkspace.qml
        UI/MainLeftPannel.qml

        UI/Settings/MainSettings.qml

        #============= SERVER ==========#

        UI/Server/Server.qml
        UI/Server/ServerPannel.qml
        UI/Server/ServerWorkSpace.qml

        UI/Server/ServerChat/Chats.qml

        UI/Server/KanBan/Boards.qml
        UI/Server/KanBan/KanBan.qml
        UI/Server/KanBan/TaskPopup.qml

        UI/Server/DocumentEditor/Documents.qml
        UI/Server/DocumentEditor/DocumentEditor.qml

        #============= FRIENDS ==========#

        UI/Friends/Friends.qml
        UI/Friends/FriendsPannel.qml
        UI/Friends/FriendsWorkSpace.qml

        UI/Friends/FriendList/FriendList.qml
        UI/Friends/FriendList/FriendListItem.qml
        UI/Friends/FriendList/FriendSearchWidget.qml

        UI/Friends/Requestlist/FriendRequestsWidget.qml

        #============= COMMON ==========#

        UI/Common/TitleBar/TitleBar.qml
        UI/Common/Members/Members.qml

        UI/Common/Chat/ChatSpace.qml
        UI/Common/Chat/MessageList.qml
        UI/Common/Chat/ChatMessageContainer.qml
        UI/Common/Chat/Containers/TextMessageContainer.qml
        UI/Common/Chat/Containers/InvitationMessageContainer.qml

        #============= SOURCES ==========#

    SOURCES
        Core/connection.cpp
        Core/connection.h
        Core/Events/newmessage.cpp
        Core/Events/newmessage.h
        Core/Data/CoreData.h
        Core/Lib/cryptolib.h
        Core/Lib/cryptolib.cpp

        #============= UI ==========#
        Core/UIManager/uimanager.h
        Core/UIManager/uimanager.cpp

        #============= SERVER ==========#
        Core/UIManager/Server/servermanager.h
        Core/UIManager/Server/servermanager.cpp

        #============= FRIENDS ==========#
        Core/UIManager/Friend/friendmanager.h
        Core/UIManager/Friend/friendmanager.cpp

        #============= CHAT ==========#
        Core/UIManager/ChatManager/chatmanager.h
        Core/UIManager/ChatManager/chatmanager.cpp
        QML_FILES MainWindow.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appNomadClient PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appNomadClient
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appNomadClient PRIVATE
    Qt6::Quick
    Qt6::Core
    Qt6::Network
    OpenSSL::SSL
    OpenSSL::Crypto
)

install(TARGETS appNomadClient
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
