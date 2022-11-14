qt_add_library(extensions STATIC)
set_target_properties(extensions PROPERTIES AUTOMOC ON)
qt_add_qml_module(extensions
    URI Extensions
    VERSION 1.0
    RESOURCE_PREFIX /example.mixxx.org/imports
    SOURCES multirowselectionmodel.cpp
    )
