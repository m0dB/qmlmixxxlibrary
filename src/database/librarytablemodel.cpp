#include "librarytablemodel.h"

#include <QDebug>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <chrono>
#include <mutex>
#include <thread>

#include "mixxxdb.h"

void Worker::execute(const QString& queryString, const QString& signal) {
    QSqlQuery query(queryString, MixxxDb::singleton()->database());
    QList<QSqlRecord> results;
    query.exec();
    while (query.next()) {
        results.push_back(query.record());
    }
    QMetaObject::invokeMethod(this, signal.toLocal8Bit().data(), Qt::QueuedConnection, Q_ARG(const QList<QSqlRecord>&, results));
}

class DurationFormatter : public Formatter {
  public:
    QVariant format(const QVariant& variant) {
        if (variant.canConvert<double>()) {
            int seconds = static_cast<int>(variant.toDouble() + 0.5);
            int minutes = seconds / 60;
            seconds -= minutes * 60;
            return QString::number(minutes) + ":" + QStringLiteral("%1").arg(seconds, 2, 10, QLatin1Char('0'));
        }
        return variant;
    }
};

class BpmFormatter : public Formatter {
  public:
    QVariant format(const QVariant& variant) {
        if (variant.canConvert<double>()) {
            return QString::number(variant.toDouble(), 'f', 1);
        }
        return variant;
    }
};

LibraryTableModel::LibraryTableModel(QObject* parent)
        : QAbstractTableModel(parent), m_fieldNames{{"location", "artist", "title", "album", "year", "duration", "bpm", "datetime_added"}} {
    m_formatters.resize(columnCount());
    for (int i = 0; i < m_fieldNames.size(); i++) {
        QString header = m_fieldNames[i];
        if (header == "duration") {
            m_formatters[i] = std::move(std::unique_ptr<Formatter>(new DurationFormatter));
        } else if (header == "bpm") {
            m_formatters[i] = std::move(std::unique_ptr<Formatter>(new BpmFormatter));
        }
    }

    Worker* worker = new Worker;

    worker->moveToThread(&m_workerThread);

    connect(&m_workerThread, &QThread::finished, worker, &QObject::deleteLater);

    connect(this, &LibraryTableModel::execute, worker, &Worker::execute);

    connect(worker, &Worker::totalRowCountReady, this, &LibraryTableModel::totalRowCountReady);
    connect(worker, &Worker::ready, this, &LibraryTableModel::ready);
    connect(worker, &Worker::readySort, this, &LibraryTableModel::readySort);

    m_workerThread.start();

    requestTotalRowCount();
}

LibraryTableModel::~LibraryTableModel() {
    m_workerThread.quit();
    m_workerThread.wait();
}

void LibraryTableModel::requestTotalRowCount() {
    execute("SELECT COUNT(*) FROM library", "totalRowCountReady");
}

void LibraryTableModel::requestData(const QString& signal) {
    int row = static_cast<int>(m_targetContentY / 20.);
    int from = std::max(0, row - 1);
    int to = std::min(m_totalRowCount, from + 100);

    int offset = from;
    int limit = to - from;
    const auto s = QString("SELECT " + m_fieldNames.join(",") + " FROM library") + m_sort + QString(" LIMIT %1 OFFSET %2").arg(limit).arg(offset);

    emit execute(s, signal);
}

void LibraryTableModel::totalRowCountReady(const QList<QSqlRecord>& results) {
    int totalRowCount = results[0].value(0).toInt();
    if (m_totalRowCount != totalRowCount) {
        m_totalRowCount = totalRowCount;
        emit totalRowCountChanged(m_totalRowCount);
    }

    requestData("ready");
}

void LibraryTableModel::sort(int column, Qt::SortOrder sortOrder) {
    m_sort = " ORDER BY " + m_fieldNames[column] + (sortOrder == Qt::AscendingOrder ? " ASC" : " DESC");
    requestData("readySort");
}

int LibraryTableModel::rowCount(const QModelIndex& parent) const {
    return m_totalRowCount;
}

int LibraryTableModel::columnCount(const QModelIndex& parent) const {
    return m_fieldNames.size();
}

void LibraryTableModel::ready(const QList<QSqlRecord>& results) {
    if (m_results.isEmpty()) {
        beginResetModel();
        m_results = results;
        endResetModel();
    } else {
        m_results = results;
    }
    if (m_readyContentY != m_targetContentY) {
        m_readyContentY = m_targetContentY;
        emit readyContentYChanged(m_readyContentY);
    }
    if (m_targetContentY != m_pendingContentY) {
        setTargetContentY(m_pendingContentY);
    }
}
void LibraryTableModel::readySort(const QList<QSqlRecord>& results) {
    ready(results);
    int row = static_cast<int>(m_readyContentY / 20.);
    int from = std::max(0, row - 1);
    int to = std::min(m_totalRowCount, from + 100);
    dataChanged(index(from, 0), index(to - 1, columnCount() - 1));
}

QVariant LibraryTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }

    if (m_results.isEmpty()) {
        qDebug() << "no results";
        return QVariant(QString(""));
    }

    int row = static_cast<int>(m_readyContentY / 20.);
    int from = std::max(0, row - 1);
    int to = std::min(m_totalRowCount, from + 100);

    if (index.row() < from || index.row() >= to) {
        qDebug() << "out of range" << index.row() << from << to;
        return QVariant();
    }
    QSqlRecord record = m_results[index.row() - from];

    const auto& formatter = m_formatters[index.column()];
    if (formatter) {
        return formatter->format(record.value(index.column()));
    }

    return record.value(index.column());
}

QVariant LibraryTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QVariant();
    }
    return QVariant(m_fieldNames[section]);
}

void LibraryTableModel::setTargetContentY(qreal y) {
    if (m_targetContentY != y) {
        if (m_targetContentY == m_readyContentY) {
            m_targetContentY = y;
            m_pendingContentY = y;
            emit targetContentYChanged(m_targetContentY);
            requestData("ready");
        } else {
            m_pendingContentY = y;
        }
    }
}
