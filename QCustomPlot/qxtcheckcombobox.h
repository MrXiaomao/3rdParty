#ifndef QXTCHECKCOMBOBOX_H
#define QXTCHECKCOMBOBOX_H

#include <QComboBox>
#include "qxtnamespace.h"
#include "qxtglobal.h"

class QxtCheckComboBoxPrivate;

class QxtCheckComboBox : public QComboBox
{
    Q_OBJECT
    QXT_DECLARE_PRIVATE(QxtCheckComboBox)
    Q_PROPERTY(QString separator READ separator WRITE setSeparator)
    Q_PROPERTY(QString defaultText READ defaultText WRITE setDefaultText)
    Q_PROPERTY(QStringList checkedItems READ checkedItems WRITE setCheckedItems)

public:
    explicit QxtCheckComboBox(QWidget* parent = 0);
    virtual ~QxtCheckComboBox();

    virtual void hidePopup();

    QString defaultText() const;
    void setDefaultText(const QString& text);

    Qt::CheckState itemCheckState(int index) const;
    void setItemCheckState(int index, Qt::CheckState state);

    QString separator() const;
    void setSeparator(const QString& separator);

    QStringList checkedItems() const;

public Q_SLOTS:
    void setCheckedItems(const QStringList& items);

Q_SIGNALS:
    void checkedItemsChanged(const QStringList& items);
};

#endif // QXTCHECKCOMBOBOX_H
