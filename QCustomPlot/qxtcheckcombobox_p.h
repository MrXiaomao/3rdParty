#ifndef QXTCHECKCOMBOBOX_P_H
#define QXTCHECKCOMBOBOX_P_H

#include <QListView>
#include <QStandardItemModel>
#include "qxtcheckcombobox.h"

class QxtCheckComboModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit QxtCheckComboModel(QObject* parent = 0);

    virtual Qt::ItemFlags flags(const QModelIndex& index) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

Q_SIGNALS:
    void checkStateChanged();
};

class QxtCheckComboBoxPrivate : public QObject, public QxtPrivate<QxtCheckComboBox>
{
    Q_OBJECT

public:
    QXT_DECLARE_PUBLIC(QxtCheckComboBox)
    QxtCheckComboBoxPrivate();
    bool eventFilter(QObject* receiver, QEvent* event);
    QString separator;
    QString defaultText;
    bool containerMousePress;

public Q_SLOTS:
    void updateCheckedItems();
    void toggleCheckState(int index);

private:
    QString elideTextToWidth(const QString& sourceText, int availableWidth);
};

#endif // QXTCHECKCOMBOBOX_P_H
