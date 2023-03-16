#include "mixxxdb.h"

#include <iostream>

MixxxDb::MixxxDb()
        : m_db(QSqlDatabase::addDatabase("QSQLITE")) {
    m_db.setDatabaseName("mixxxdb.sqlite");
    if (!m_db.open()) {
        std::cerr << "failed to open mixxxdb.sqlite. place a copy in your work directory" << std::endl;
        exit(-1);
    }
}

MixxxDb::~MixxxDb() {
    m_db.close();
}

const QSqlDatabase& MixxxDb::database() const {
    return m_db;
}

MixxxDb* MixxxDb::singleton() {
    static MixxxDb* instance = new MixxxDb();
    return instance;
}
