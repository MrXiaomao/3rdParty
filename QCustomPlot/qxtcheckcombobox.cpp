#include "qxtcheckcombobox.h"
#include "qxtcheckcombobox_p.h"
#include <QLineEdit>
#include <QKeyEvent>
#include <QStylePainter>
#include <QStyleOption>

QxtCheckComboBoxPrivate::QxtCheckComboBoxPrivate()
    : containerMousePress(false)
    , displayLineEdit(nullptr)
{
    separator = QLatin1String(",");
}

bool QxtCheckComboBoxPrivate::eventFilter(QObject* receiver, QEvent* event)
{
    switch (event->type())
    {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (receiver == &qxt_p() && (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down))
        {
            qxt_p().showPopup();
            return true;
        }
        else if (keyEvent->key() == Qt::Key_Enter ||
                 keyEvent->key() == Qt::Key_Return ||
                 keyEvent->key() == Qt::Key_Escape)
        {
            // it is important to call QComboBox implementation
            qxt_p().QComboBox::hidePopup();
            if (keyEvent->key() != Qt::Key_Escape)
                return true;
        }
    }
    case QEvent::MouseButtonPress:
        containerMousePress = (receiver == qxt_p().view()->window());
        break;
    case QEvent::MouseButtonRelease:
        containerMousePress = false;;
        break;
    default:
        break;
    }
    return false;
}

void QxtCheckComboBoxPrivate::resetLineEditGeometry()
{
    QLineEdit* lineEdit = qxt_p().lineEdit();
    if (!lineEdit)
        return;

    QStyleOptionComboBox opt;
    qxt_p().initStyleOption(&opt);
    QRect editRect = qxt_p().style()->subControlRect(
        QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, &qxt_p());
    lineEdit->setGeometry(editRect);
}

void QxtCheckComboBoxPrivate::updateCheckedItems()
{
    QStringList items = qxt_p().checkedItems();

    QString displayText;
    if (items.isEmpty())
        displayText = defaultText;
    else
        displayText = items.join(separator);

    // 给悬浮Tooltip赋值完整的所有选中内容，用户鼠标一悬停就能看全
    qxt_p().setToolTip(displayText);

    // 收集每个勾选项的图标，按勾选顺序一一对应文本
    QList<QxtCheckComboBoxLineEdit::DisplayItem> displayItems;
    QAbstractItemModel* model = qxt_p().model();
    if (model && !items.isEmpty())
    {
        QModelIndex start = model->index(0, qxt_p().modelColumn(), qxt_p().rootModelIndex());
        QModelIndexList indexes = model->match(start, Qt::CheckStateRole, Qt::Checked, -1, Qt::MatchExactly);
        for (const QModelIndex& index : indexes)
        {
            QxtCheckComboBoxLineEdit::DisplayItem item;
            item.text = index.data(Qt::DisplayRole).toString();
            item.icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
            displayItems.append(item);
        }
    }

    // 先设置内容，再按"图标+文本"的实际像素宽度自适应控件宽度，钳位到 100~400
    if (displayLineEdit)
        displayLineEdit->setDisplayItems(displayItems, separator, defaultText);
    else
        qxt_p().setEditText(displayText);

    int contentWidth = displayLineEdit ? displayLineEdit->contentWidth() : 0;
    int newControlWidth = contentWidth + 35;
    if (newControlWidth > 400)
        newControlWidth = 400;
    else if (newControlWidth < 100)
        newControlWidth = 100;
    qxt_p().setFixedWidth(newControlWidth);
    // setFixedWidth 在宽度未变时不会触发 resizeEvent，这里主动还原编辑区，去掉左侧图标留白
    resetLineEditGeometry();

    emit qxt_p().checkedItemsChanged(items);
}

void QxtCheckComboBoxPrivate::toggleCheckState(int index)
{
    QVariant value = qxt_p().itemData(index, Qt::CheckStateRole);
    if (value.isValid())
    {
        Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
        qxt_p().setItemData(index, (state == Qt::Unchecked ? Qt::Checked : Qt::Unchecked), Qt::CheckStateRole);
    }
}

QxtCheckComboModel::QxtCheckComboModel(QObject* parent)
        : QStandardItemModel(0, 1, parent) // rows,cols
{
}

Qt::ItemFlags QxtCheckComboModel::flags(const QModelIndex& index) const
{
    return QStandardItemModel::flags(index) | Qt::ItemIsUserCheckable;
}

QVariant QxtCheckComboModel::data(const QModelIndex& index, int role) const
{
    QVariant value = QStandardItemModel::data(index, role);
    if (index.isValid() && role == Qt::CheckStateRole && !value.isValid())
        value = Qt::Unchecked;
    return value;
}

bool QxtCheckComboModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    bool ok = QStandardItemModel::setData(index, value, role);
    if (ok && role == Qt::CheckStateRole)
    {
        emit dataChanged(index, index);
        emit checkStateChanged();
    }
    return ok;
}

/*!
    \class QxtCheckComboBox
    \inmodule QxtWidgets
    \brief The QxtCheckComboBox widget is an extended QComboBox with checkable items.

    QxtComboBox is a specialized combo box with checkable items.
    Checked items are collected together in the line edit.

    \code
    QxtCheckComboBox* comboBox = new QxtCheckComboBox(this);
    comboBox->addItems(...);
    comboBox->setItemCheckState(2, Qt::Checked);
    comboBox->setItemCheckState(4, Qt::Checked);
    // OR
    comboBox->setCheckedItems(QStringList() << "dolor" << "amet");
    \endcode

    \image qxtcheckcombobox.png "QxtCheckComboBox in Plastique style."
 */

/*!
    \fn QxtCheckComboBox::checkedItemsChanged(const QStringList& items)

    This signal is emitted whenever the checked \a items have been changed.
 */

/*!
    Constructs a new QxtCheckComboBox with \a parent.
 */
QxtCheckComboBox::QxtCheckComboBox(QWidget* parent) : QComboBox(parent)
{
    QXT_INIT_PRIVATE(QxtCheckComboBox);
    setModel(new QxtCheckComboModel(this));
    connect(this, SIGNAL(activated(int)), &qxt_d(), SLOT(toggleCheckState(int)));
    connect(model(), SIGNAL(checkStateChanged()), &qxt_d(), SLOT(updateCheckedItems()));
    connect(model(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), &qxt_d(), SLOT(updateCheckedItems()));
    connect(model(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), &qxt_d(), SLOT(updateCheckedItems()));

    // read-only contents
    QxtCheckComboBoxLineEdit* lineEdit = new QxtCheckComboBoxLineEdit(this);
    lineEdit->setReadOnly(true);
    setLineEdit(lineEdit);
    qxt_d().displayLineEdit = lineEdit;
    lineEdit->disconnect(this);
    setInsertPolicy(QComboBox::NoInsert);

    view()->installEventFilter(&qxt_d());
    view()->window()->installEventFilter(&qxt_d());
    view()->viewport()->installEventFilter(&qxt_d());
    this->installEventFilter(&qxt_d());
}

/*!
    \reimp
 */
void QxtCheckComboBox::paintEvent(QPaintEvent*)
{
    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));

    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    // 当前项图标与文本都由行编辑框自绘，这里清空，避免左侧额外出现一个当前项图标
    opt.currentIcon = QIcon();
    opt.currentText = QString();

    painter.drawComplexControl(QStyle::CC_ComboBox, opt);
    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}

/*!
    \reimp
 */
void QxtCheckComboBox::resizeEvent(QResizeEvent* event)
{
    QComboBox::resizeEvent(event);
    // QComboBox 会按当前项图标缩窄行编辑框，这里还原为完整编辑区
    qxt_d().resetLineEditGeometry();
}

/*!
    Destructs the combo box.
 */
QxtCheckComboBox::~QxtCheckComboBox()
{
}

/*!
    \reimp
 */
void QxtCheckComboBox::hidePopup()
{
    if (qxt_d().containerMousePress)
        QComboBox::hidePopup();
}

/*!
    Returns the check state of the item at \a index.
 */
Qt::CheckState QxtCheckComboBox::itemCheckState(int index) const
{
    return static_cast<Qt::CheckState>(itemData(index, Qt::CheckStateRole).toInt());
}

/*!
    Sets the check \a state of the item at \a index.
 */
void QxtCheckComboBox::setItemCheckState(int index, Qt::CheckState state)
{
    setItemData(index, state, Qt::CheckStateRole);
}

/*!
    \property QxtCheckComboBox::checkedItems
    \brief the checked items.
 */
QStringList QxtCheckComboBox::checkedItems() const
{
    QStringList items;
    if (model())
    {
        QModelIndex index = model()->index(0, modelColumn(), rootModelIndex());
        QModelIndexList indexes = model()->match(index, Qt::CheckStateRole, Qt::Checked, -1, Qt::MatchExactly);
        foreach(const QModelIndex& index, indexes)
            items += index.data().toString();
    }
    return items;
}

void QxtCheckComboBox::setCheckedItems(const QStringList& items)
{
    // not the most efficient solution but most likely nobody
    // will put too many items into a combo box anyway so...
    foreach(const QString& text, items)
    {
        const int index = findText(text);
        setItemCheckState(index, index != -1 ? Qt::Checked : Qt::Unchecked);
    }
}

/*!
    \property QxtCheckComboBox::defaultText
    \brief the default text.

    The default text is shown when there are no checked items.
    The default value is an empty string.
 */
QString QxtCheckComboBox::defaultText() const
{
    return qxt_d().defaultText;
}

void QxtCheckComboBox::setDefaultText(const QString& text)
{
    if (qxt_d().defaultText != text)
    {
        qxt_d().defaultText = text;
        qxt_d().updateCheckedItems();
    }
}

/*!
    \property QxtCheckComboBox::separator
    \brief the default separator.

    The checked items are joined together with the separator string.
    The default value is a comma (",").
 */
QString QxtCheckComboBox::separator() const
{
    return qxt_d().separator;
}

void QxtCheckComboBox::setSeparator(const QString& separator)
{
    if (qxt_d().separator != separator)
    {
        qxt_d().separator = separator;
        qxt_d().updateCheckedItems();
    }
}
