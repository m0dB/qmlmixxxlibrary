qt_add_library(database STATIC)
set_target_properties(database PROPERTIES AUTOMOC ON)
qt_add_qml_module(database
    URI Database
    VERSION 1.0
    RESOURCE_PREFIX /example.mixxx.org/imports 
    SOURCES mixxxdb.cpp librarytablemodel.cpp headerview.cpp
    )
target_link_libraries(database PUBLIC Qt6::Sql)
