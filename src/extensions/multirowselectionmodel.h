#pragma once

#include <qqml.h>

#include <QItemSelectionModel>

// Extension of QItemSelectionModel to deal with (multi) row selection

class MultiRowSelectionModel : public QItemSelectionModel {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QList<QUrl> uriList READ uriList NOTIFY uriListChanged)
    Q_PROPERTY(QString uriListAsString READ uriListAsString NOTIFY uriListChanged)

    int m_lastRow{0};
    int m_adjecentStartRow{0};
    QList<QUrl> m_uriList;
    QString m_uriListAsString;

  public:
    MultiRowSelectionModel(QAbstractItemModel* model, QObject* parent);
    MultiRowSelectionModel(QAbstractItemModel* model = nullptr);

    Q_INVOKABLE void selectSingleRow(int row);
    Q_INVOKABLE void toggleRow(int row);
    Q_INVOKABLE void selectAdjecentRows(int row);
    Q_INVOKABLE bool isRowSelected(int row) const;

  private:
    QList<QUrl> uriList() const;
    QString uriListAsString() const;
    void updateUriList();
  signals:
    void uriListChanged();
};
