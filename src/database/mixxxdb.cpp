#include "mixxxdb.h"

MixxxDb::MixxxDb()
        : m_db(QSqlDatabase::addDatabase("QSQLITE")) {
    m_db.setDatabaseName("mixxxdb.sqlite");
    m_db.open();
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
