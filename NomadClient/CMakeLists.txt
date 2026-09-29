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
    QML_FILES
        Main.qml
        QML_FILES LoginScreen.qml MainWorkspace.qml
        QML_FILES KanBan/KanBan.qml KanBan/TaskPopup.qml
        QML_FILES DocumentEditor/DocumentEditor.qml
        SOURCES Core/connection.cpp Core/connection.h
        SOURCES Core/Events/newmessage.cpp Core/Events/newmessage.h
        SOURCES Core/Data/CoreData.h
        SOURCES Core/Lib/cryptolib.h Core/Lib/cryptolib.cpp
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
