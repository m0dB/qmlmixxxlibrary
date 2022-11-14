#include "librarytablemodel.h"

#include "mixxxdb.h"

// QSqlQueryModel to retreive the library table from mixxxdb.sqlite
//
// TODO reuse code from mixxx/src/library/basetracktablemodel.cpp
// from column cache, formatting of values, etc
// TODO add sorting per column (asc/dec)

LibraryTableModel::LibraryTableModel(QObject* parent)
        : QSqlQueryModel(parent), m_fields{"id", "artist", "title", "genre", "duration", "bpm"} {
    sortByColumn(0, Qt::AscendingOrder);

    setHeaderData(0, Qt::Horizontal, ""); // Don't show id
    setHeaderData(1, Qt::Horizontal, "Artist");
    setHeaderData(2, Qt::Horizontal, "Title");
    setHeaderData(3, Qt::Horizontal, "Genre");
    setHeaderData(4, Qt::Horizontal, "Duration");
    setHeaderData(5, Qt::Horizontal, "BPM");
}

void LibraryTableModel::sortByColumn(int column, Qt::SortOrder sortOrder) {
    QString qs = "SELECT " + m_fields.join(",") + " FROM library ORDER BY " + m_fields[column] + " " + (sortOrder == Qt::AscendingOrder ? "ASC" : "DESC");
    setQuery(qs, MixxxDb::singleton()->database());
}

QVariant LibraryTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }

    const auto result = QSqlQueryModel::data(index, role);

    // rudimentary format of duration and bpm
    switch (index.column()) {
    case 4: // Duration
    {
        if (result.canConvert<double>()) {
            int seconds = static_cast<int>(result.toDouble() + 0.5);
            int minutes = seconds / 60;
            seconds -= minutes * 60;
            return QString::number(minutes) + ":" + QStringLiteral("%1").arg(seconds, 2, 10, QLatin1Char('0'));
        }
        break;
    }
    case 5: // BPM
    {
        if (result.canConvert<double>()) {
            return QString::number(result.toDouble(), 'f', 1);
        }
        break;
    }
    default:
        break;
    }

    return result;
}
