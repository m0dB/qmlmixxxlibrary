qt_add_library(qmlapp STATIC)

set_target_properties(qmlapp PROPERTIES AUTOMOC ON)
target_link_libraries(qmlapp PRIVATE Qt6::Quick)

qt_add_qml_module(qmlapp
    URI Application
    VERSION 1.0
    RESOURCE_PREFIX /example.mixxx.org/imports
    QML_FILES
        main.qml
)



