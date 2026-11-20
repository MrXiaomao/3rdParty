#ifndef QXTCHECKCOMBOBOX_P_H
#define QXTCHECKCOMBOBOX_P_H

#include <QListView>
#include <QStandardItemModel>
#include <QLineEdit>
#include <QIcon>
#include <QPainter>
#include <QFontMetrics>
#include <QStyleOption>
#include "qxtcheckcombobox.h"

// 只读行编辑器：自绘"图标 + 文本"，让每个勾选项都显示自己的图标
class QxtCheckComboBoxLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    struct DisplayItem
    {
        QIcon icon;
        QString text;
    };

    explicit QxtCheckComboBoxLineEdit(QWidget* parent = 0)
        : QLineEdit(parent)
        , m_iconSize(16)
        , m_spacing(4)
    {
    }

    void setDisplayItems(const QList<DisplayItem>& items, const QString& separator,
                         const QString& fallbackText)
    {
        m_items = items;
        m_separator = separator;
        m_fallbackText = fallbackText;
        update();
    }

    // 绘制全部内容所需的像素宽度
    int contentWidth() const
    {
        QFontMetrics fm(font());
        if (m_items.isEmpty())
            return fm.horizontalAdvance(m_fallbackText);

        int w = 0;
        for (int i = 0; i < m_items.size(); ++i)
        {
            if (i > 0)
                w += fm.horizontalAdvance(m_separator) + m_spacing;
            w += m_iconSize + 2 + fm.horizontalAdvance(m_items.at(i).text);
        }
        return w;
    }

protected:
    void paintEvent(QPaintEvent* /*event*/) Q_DECL_OVERRIDE
    {
        QStyleOptionFrame opt;
        initStyleOption(&opt);
        QPainter painter(this);
        style()->drawPrimitive(QStyle::PE_PanelLineEdit, &opt, &painter, this);

        const QRect cr = contentsRect();
        painter.setPen(palette().color(QPalette::Text));

        if (m_items.isEmpty())
        {
            painter.drawText(cr, Qt::AlignLeft | Qt::AlignVCenter, m_fallbackText);
            return;
        }

        QFontMetrics fm(font());
        int x = cr.left();
        for (int i = 0; i < m_items.size(); ++i)
        {
            const DisplayItem& item = m_items.at(i);

            if (i > 0)
            {
                const int sepW = fm.horizontalAdvance(m_separator) + m_spacing;
                painter.drawText(QRect(x, cr.top(), sepW, cr.height()),
                                 Qt::AlignLeft | Qt::AlignVCenter, m_separator);
                x += sepW;
            }

            const QPixmap pm = item.icon.pixmap(m_iconSize, m_iconSize);
            if (!pm.isNull())
            {
                painter.drawPixmap(x, cr.top() + (cr.height() - pm.height()) / 2, pm);
                x += pm.width();
            }
            x += 2;

            const int textW = fm.horizontalAdvance(item.text);
            painter.drawText(QRect(x, cr.top(), textW, cr.height()),
                             Qt::AlignLeft | Qt::AlignVCenter, item.text);
            x += textW;

            if (x > cr.right())
                break;
        }
    }

private:
    QList<DisplayItem> m_items;
    QString m_separator;
    QString m_fallbackText;
    int m_iconSize;
    int m_spacing;
};

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
    QxtCheckComboBoxLineEdit* displayLineEdit;

public Q_SLOTS:
    void updateCheckedItems();
    void toggleCheckState(int index);

private:
    // QComboBox 默认会给当前项图标留出空间并缩窄行编辑框，这里还原为完整编辑区
    void resetLineEditGeometry();
};

#endif // QXTCHECKCOMBOBOX_P_H
