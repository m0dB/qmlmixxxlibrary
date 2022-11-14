#include <QSqlDatabase>

class MixxxDb {
    QSqlDatabase m_db;

    MixxxDb();

  public:
    ~MixxxDb();
    const QSqlDatabase& database() const;
    static MixxxDb* singleton();
};
