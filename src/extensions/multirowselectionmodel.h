#pragma once

#include <qqml.h>

#include <QItemSelectionModel>

// Extension of QItemSelectionModel to deal with (multi) row selection

class MultiRowSelectionModel : public QItemSelectionModel {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString dragString READ dragString NOTIFY dragStringChanged)
    int m_lastRow{0};
    int m_adjecentStartRow{0};

  public:
    MultiRowSelectionModel(QAbstractItemModel* model, QObject* parent);
    MultiRowSelectionModel(QAbstractItemModel* model = nullptr);

    Q_INVOKABLE void selectSingleRow(int row);
    Q_INVOKABLE void toggleRow(int row);
    Q_INVOKABLE void selectAdjecentRows(int row);
    Q_INVOKABLE QString selectedColumnToString(int column) const;

  private:
    QString dragString() const;
  signals:
    void dragStringChanged();
};
