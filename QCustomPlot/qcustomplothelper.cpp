#include "qcustomplothelper.h"
#include "WaveformStats.h"
#include "qxtcheckcombobox.h"
#include <QMouseEvent>
#include <QToolTip>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringConverter>
#endif

//////////////////////////////////////////////////////////////////////////////////
XCPValueMarker::XCPValueMarker(QCustomPlot *customPlot, Anchor anchor, QObject *parent)
    : QObject(parent)
    , mOffset(QPoint(0, 0))
    , mCustomPlot(customPlot)
    , mAnchor(anchor)
{
    {
        QColor clrDefault(Qt::red);
        QBrush brushDefault(Qt::NoBrush);
        QPen penDefault(clrDefault);
        //        penDefault.setBrush(brushDefault);
        penDefault.setWidthF(0.5);

        mPointItem = new QCPItemTracer(mCustomPlot);
        mPointItem->setObjectName("mPointItem");
        mPointItem->setLayer("overlay");
        mPointItem->setStyle(QCPItemTracer::tsCircle);
        mPointItem->setPen(penDefault);
        mPointItem->setBrush(brushDefault);

        mLabelItem = new QCPItemText(mCustomPlot);
        mLabelItem->setObjectName("mLabelItem");
        mLabelItem->setLayer("overlay");
        mLabelItem->setClipToAxisRect(false);
        mLabelItem->setPadding(QMargins(6, 4, 6, 4));
        mLabelItem->setBrush(QBrush(QColor(255, 255, 255, 230)));
        mLabelItem->setPen(penDefault);
        mLabelItem->position->setParentAnchor(mPointItem->position);
        mLabelItem->setFont(QFont("Arial", 8, QFont::Weight::Thin));
        mLabelItem->setColor(clrDefault);
        mLabelItem->setText("");

        mLeadLine = new QCPItemLine(mCustomPlot);
        mLeadLine->setObjectName("mLeadLine");
        mLeadLine->setPen(penDefault);
        mLeadLine->setLayer("overlay");
        mLeadLine->setClipToAxisRect(false);
        mLeadLine->setHead(QCPLineEnding::esSpikeArrow);//设置头部为箭头形状

        switch (mAnchor)
        {
        case AtXAxis:
        {
            mPointItem->position->setTypeX(QCPItemPosition::ptPlotCoords);
            mPointItem->position->setTypeY(QCPItemPosition::ptAxisRectRatio);
            mPointItem->setSize(7);
            mLabelItem->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);

            mLeadLine->end->setParentAnchor(mPointItem->position);
            mLeadLine->start->setParentAnchor(mLeadLine->end);
            mLeadLine->start->setCoords(0, 20);//偏移量
            break;
        }
        case AtYAxis:
        {
            mPointItem->position->setTypeX(QCPItemPosition::ptAxisRectRatio);
            mPointItem->position->setTypeY(QCPItemPosition::ptPlotCoords);
            mPointItem->setSize(7);
            mLabelItem->setPositionAlignment(Qt::AlignRight | Qt::AlignHCenter);

            mLeadLine->end->setParentAnchor(mPointItem->position);
            mLeadLine->start->setParentAnchor(mLabelItem->position);
            mLeadLine->start->setCoords(-20, 0);//偏移量
            break;
        }
        case AtDataPoint:
        {
            mPointItem->position->setTypeX(QCPItemPosition::ptPlotCoords);
            mPointItem->position->setTypeY(QCPItemPosition::ptPlotCoords);
            mPointItem->setSize(5);

            mLabelItem->setPositionAlignment(Qt::AlignLeft | Qt::AlignVCenter);

            mLeadLine->end->setParentAnchor(mPointItem->position);
            mLeadLine->start->setParentAnchor(mLeadLine->end);
            mLeadLine->start->setCoords(20 + mOffset.x(), mOffset.y());
            break;
        }
        default:
            break;
        }

        //setVisible(false);
    }
}

XCPValueMarker::~XCPValueMarker()
{
    if(mCustomPlot)
    {
        if (mPointItem)
            mCustomPlot->removeItem(mPointItem);
        if (mLabelItem)
            mCustomPlot->removeItem(mLabelItem);
        if (mLeadLine)
            mCustomPlot->removeItem(mLeadLine);
    }
}

void XCPValueMarker::setPen(const QPen &pen)
{
    if(mPointItem)
        mPointItem->setPen(pen);
    if(mLeadLine)
        mLeadLine->setPen(pen);
}

void XCPValueMarker::setBrush(const QBrush &brush)
{
    if(mPointItem)
        mPointItem->setBrush(brush);
}

void XCPValueMarker::setTextPen(const QPen &pen)
{
    if(mLabelItem)
    {
        mLabelItem->setPen(pen);
        mLabelItem->setBrush(QBrush(QColor(255, 255, 255, 230)));
        mLabelItem->setColor(pen.color());
    }
}

void XCPValueMarker::setText(const QString &text)
{
    if(mLabelItem)
        mLabelItem->setText(text);
}

void XCPValueMarker::setVisible(bool visible)
{
    setPointVisible(visible);
    setLabelVisible(visible);
}

void XCPValueMarker::setPointVisible(bool visible)
{
    mPointVisible = visible;
    if(mPointItem)
        mPointItem->setVisible(mPointVisible);
}

void XCPValueMarker::setLabelVisible(bool visible)
{
    mLabelVisible = visible;
    if(mLabelItem)
        mLabelItem->setVisible(mLabelVisible);
    if(mLeadLine)
        mLeadLine->setVisible(mLabelVisible);
}

void XCPValueMarker::setOffset(QPoint offset)
{
    mOffset = offset;
}

void XCPValueMarker::updatePosition(QCPAxisRect *axisRect, double key, double value)
{
    if (!mLabelVisible && !mPointVisible)
        return;

    QCPAxis* xAxis = axisRect->axis(QCPAxis::AxisType::atBottom);
    QCPAxis* yAxis = axisRect->axis(QCPAxis::AxisType::atLeft);
    if (value > yAxis->range().upper)
        value = yAxis->range().upper;

    int xNumberPrecision = 1;//xAxis->numberPrecision();
    int yNumberPrecision = 1;//yAxis->numberPrecision();
    switch (mAnchor)
    {
    case AtXAxis:
    {
        mPointItem->position->setAxisRect(axisRect);
        mPointItem->position->setAxes(xAxis, yAxis);
        mPointItem->position->setCoords(key, 1);
        mLabelItem->position->setCoords(0, 15);
        mLeadLine->start->setCoords(0, 15);
        mLeadLine->end->setCoords(0, 0);

        QString keyText = QString("%1").arg(key);
        QSharedPointer<QCPAxisTicker> ticker = xAxis->ticker();
        // 类型判断分支
        if (auto dateTimeTicker = qSharedPointerDynamicCast<QCPAxisTickerDateTime>(ticker))
        {
            // QCPAxisTickerDateTime的原始key是Unix时间戳（秒为单位），直接转成QDateTime
            keyText = QDateTime::fromMSecsSinceEpoch(qint64(key * 1000)).toString(dateTimeTicker->dateTimeFormat());
        }
        else if (auto timeTicker = qSharedPointerDynamicCast<QCPAxisTickerTime>(ticker))
        {
            keyText = QTime(0,0,0).addSecs(qRound(key)).toString(timeTicker->timeFormat());
        }
        else if (auto textTicker = qSharedPointerDynamicCast<QCPAxisTickerText>(ticker))
        {
            keyText = textTicker->ticks()[key];
        }
        else if (auto fixedTicker = qSharedPointerDynamicCast<QCPAxisTickerFixed>(ticker))
        {
            keyText = QString::number(key, 'f', xNumberPrecision);
        }

        setText(keyText);
        break;
    }
    case AtYAxis:
    {
        mPointItem->position->setAxisRect(axisRect);
        mPointItem->position->setAxes(xAxis, yAxis);
        mPointItem->position->setCoords(0, value);
        mLabelItem->position->setCoords(-20, 0);
        //        mLeadLine->start->setCoords(20, 0);
        //        mLeadLine->end->setCoords(0, 0);
        setText(QString::number(value, 'f', yNumberPrecision));
        break;
    }
    case AtDataPoint:
    {
        mPointItem->position->setAxisRect(axisRect);
        mPointItem->position->setAxes(xAxis, yAxis);
        mPointItem->position->setCoords(key, value);
        mLabelItem->position->setCoords(20 + mOffset.x(), mOffset.y());
        mLeadLine->start->setCoords(20 + mOffset.x(), mOffset.y() == 0 ? 0 : (mOffset.y() / 2 + 2));

        QString keyText = QString("%1").arg(key);
        QSharedPointer<QCPAxisTicker> ticker = xAxis->ticker();
        // 类型判断分支
        if (auto dateTimeTicker = qSharedPointerDynamicCast<QCPAxisTickerDateTime>(ticker))
        {
            // QCPAxisTickerDateTime的原始key是Unix时间戳（秒为单位），直接转成QDateTime
            keyText = QDateTime::fromMSecsSinceEpoch(qint64(key * 1000)).toString(dateTimeTicker->dateTimeFormat());
        }
        else if (auto timeTicker = qSharedPointerDynamicCast<QCPAxisTickerTime>(ticker))
        {
            keyText = QTime(0,0,0).addSecs(qRound(key)).toString(timeTicker->timeFormat());
        }
        else if (auto textTicker = qSharedPointerDynamicCast<QCPAxisTickerText>(ticker))
        {
            keyText = textTicker->ticks()[key];
        }
        else if (auto fixedTicker = qSharedPointerDynamicCast<QCPAxisTickerFixed>(ticker))
        {
            keyText = QString::number(key, 'f', xNumberPrecision);
        }

        setText(QString("x:%1,y:%2").arg(keyText, QString::number(value, 'f', yNumberPrecision)));
        break;
    }
    default:
        break;
    }

    //设置裁剪区域，超出边界就不显示了
    mPointItem->setClipToAxisRect(true);
    mPointItem->setClipAxisRect(axisRect);
    mLabelItem->setClipToAxisRect(true);
    mLabelItem->setClipAxisRect(axisRect);
    mLeadLine->setClipToAxisRect(true);
    mLeadLine->setClipAxisRect(axisRect);
}

//////////////////////////////////////////////////////////////////////////////////
XCPReferenceLine::XCPReferenceLine(QCustomPlot *customPlot, Orientation orientation, QObject *parent)
    : QObject(parent)
    , mCustomPlot(customPlot)
    , mOrientation(orientation)
{
    initLine();
}

XCPReferenceLine::~XCPReferenceLine()
{
    if(mCustomPlot)
    {
        if (mVerticalLine)
            mCustomPlot->removeItem(mVerticalLine);
        if (mHorizontalLine)
            mCustomPlot->removeItem(mHorizontalLine);
    }
}

void XCPReferenceLine::initLine()
{
    if(mCustomPlot)
    {
        QPen linesPen(Qt::red, 1, Qt::DashLine);

        if(Vertical == mOrientation || Crosshair == mOrientation)
        {
            mVerticalLine = new QCPItemStraightLine(mCustomPlot);//垂直线
            mVerticalLine->setLayer("overlay");
            mVerticalLine->setPen(linesPen);
            mVerticalLine->setClipToAxisRect(true);
            mVerticalLine->point1->setCoords(0, 0);
            mVerticalLine->point2->setCoords(0, 0);
        }

        if(Horizontal == mOrientation || Crosshair == mOrientation)
        {
            mHorizontalLine = new QCPItemStraightLine(mCustomPlot);//水平线
            mHorizontalLine->setLayer("overlay");
            mHorizontalLine->setPen(linesPen);
            mHorizontalLine->setClipToAxisRect(true);
            mHorizontalLine->point1->setCoords(0, 0);
            mHorizontalLine->point2->setCoords(0, 0);
        }
    }
}

void XCPReferenceLine::updatePosition(QCPAxisRect *axisRect, double key, double value)
{
    QCPAxis* yAxis = axisRect->axis(QCPAxis::AxisType::atLeft);
    QCPAxis* xAxis = axisRect->axis(QCPAxis::AxisType::atBottom);

    if(Vertical == mOrientation || Crosshair == mOrientation)
    {
        if(mVerticalLine)
        {
            mVerticalLine->point1->setCoords(key, yAxis->range().lower);
            mVerticalLine->point2->setCoords(key, yAxis->range().upper);
        }
    }

    if(Horizontal == mOrientation || Crosshair == mOrientation)
    {
        if(mHorizontalLine)
        {
            mHorizontalLine->point1->setCoords(xAxis->range().lower, value);
            mHorizontalLine->point2->setCoords(xAxis->range().upper, value);
        }
    }

    //设置裁剪区域，超出边界就不显示了
    if (mHorizontalLine)
    {
        mHorizontalLine->point1->setAxisRect(axisRect);
        mHorizontalLine->point2->setAxisRect(axisRect);
        mHorizontalLine->point1->setAxes(xAxis, yAxis);
        mHorizontalLine->point2->setAxes(xAxis, yAxis);
        mHorizontalLine->setClipToAxisRect(true);
        mHorizontalLine->setClipAxisRect(axisRect);
    }
    if (mVerticalLine)
    {
        mVerticalLine->point1->setAxisRect(axisRect);
        mVerticalLine->point2->setAxisRect(axisRect);
        mVerticalLine->point1->setAxes(xAxis, yAxis);
        mVerticalLine->point2->setAxes(xAxis, yAxis);
        mVerticalLine->setClipToAxisRect(true);
        mVerticalLine->setClipAxisRect(axisRect);
    }
}

void XCPReferenceLine::setVisible(bool visible)
{
    if(mVerticalLine)
        mVerticalLine->setVisible(visible);
    if(mHorizontalLine)
        mHorizontalLine->setVisible(visible);
}

//////////////////////////////////////////////////////////////////////////////////
XCPRangeMask::XCPRangeMask(QCustomPlot *customPlot, QObject *parent)
    : QObject(parent)
    , mCustomPlot(customPlot)
{
    mMaskRect = new QCPItemRect(mCustomPlot);
    mMaskRect->setLayer("overlay");
    mMaskRect->setPen(QColor::fromRgb(0x00, 0xff, 0xff, 0x2f));
    mMaskRect->setBrush(QColor::fromRgb(0x00, 0xff, 0xff, 0x2f));
    mMaskRect->setClipToAxisRect(true);
    mMaskRect->topLeft->setCoords(0, 0);
    mMaskRect->bottomRight->setCoords(0, 0);
}

XCPRangeMask::~XCPRangeMask()
{
    if(mCustomPlot)
    {
        if (mMaskRect)
            mCustomPlot->removeItem(mMaskRect);
    }
}

void XCPRangeMask::setRange(QCPAxisRect *axisRect, const QCPRange& range)
{
    if(mMaskRect)
    {
        QCPAxis* yAxis = axisRect->axis(QCPAxis::AxisType::atLeft);
        mMaskRect->topLeft->setCoords(range.lower, yAxis->range().upper);
        mMaskRect->bottomRight->setCoords(range.upper, yAxis->range().lower);

        this->mRange = range;
    }
}

QCPRange XCPRangeMask::range() const
{
    return this->mRange;
}

void XCPRangeMask::setVisible(bool visible)
{
    if(mMaskRect)
        mMaskRect->setVisible(visible);
}

void XCPRangeMask::setBrush(const QBrush &brush)
{
    mMaskRect->setBrush(brush);
}

void XCPRangeMask::refresh()
{
}

//////////////////////////////////////////////////////////////////////////////////

QCustomPlotHelper::QCustomPlotHelper(QCustomPlot* customPlot, QObject *parent)
    : QObject(parent)
    , mCustomPlot(customPlot)
    , mDataTracers(QList<XCPValueMarker *>())
{
    // 关联右键菜单事件
    setupMenu();

    customPlot->setProperty("enableAutoScale", true);//图像启用自缩放功能
    customPlot->setProperty("enableFixedScale", false);//图像禁用自定义范围
    customPlot->setProperty("enableManualScale", false);//图像启用缩放控制功能
    customPlot->setProperty("xAxisFixedRange", mXAxisFixedRange);
    customPlot->rescaleAxes(true);//设置试图自由缩放
    customPlot->setAntialiasedElements(QCP::aeAll);//设置所有图形元素使用抗锯齿渲染
    //customPlot->legend->setVisible(true);//设置图例可见
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);//设置图形可移动、缩放、选中
    customPlot->xAxis->setTickLabelRotation(-45);//x轴文字倾斜45°角，方便显示长文本
    // customPlot->yAxis->ticker()->setTickCount(5);
    // customPlot->xAxis->ticker()->setTickCount(10);
    // customPlot->yAxis2->ticker()->setTickCount(5);
    // customPlot->xAxis2->ticker()->setTickCount(10);

    // 四边安装坐标轴
    customPlot->axisRect()->setupFullAxesBox(true);

    // 关键鼠标事件
    connect(mCustomPlot, &QCustomPlot::plottableClick, this, &QCustomPlotHelper::plottableClick);
    connect(mCustomPlot, &QCustomPlot::mouseDoubleClick, this, &QCustomPlotHelper::mouseDoubleClick);
    connect(mCustomPlot, &QCustomPlot::mousePress, this, &QCustomPlotHelper::mousePress);
    connect(mCustomPlot, &QCustomPlot::mouseMove, this, &QCustomPlotHelper::mouseMove);
    connect(mCustomPlot, &QCustomPlot::mouseRelease, this, &QCustomPlotHelper::mouseRelease);
    connect(mCustomPlot, &QCustomPlot::beforeReplot, this, &QCustomPlotHelper::beforeReplot);
    connect(mCustomPlot, &QCustomPlot::afterLayout, this, &QCustomPlotHelper::afterLayout);

    // 坐标轴同步缩放
    connect(mCustomPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), mCustomPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(mCustomPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), mCustomPlot->yAxis2, SLOT(setRange(QCPRange)));
    // 左右方向键切换当前点位标记
    actPrevPoint = new QAction(this);
    actPrevPoint->setShortcut(QKeySequence(Qt::Key_Left));
    actPrevPoint->setShortcutContext(Qt::WidgetShortcut);
    connect(actPrevPoint, &QAction::triggered, this, [this](){ moveActiveMarker(-1); });
    mCustomPlot->addAction(actPrevPoint);

    actNextPoint = new QAction(this);
    actNextPoint->setShortcut(QKeySequence(Qt::Key_Right));
    actNextPoint->setShortcutContext(Qt::WidgetShortcut);
    connect(actNextPoint, &QAction::triggered, this, [this](){ moveActiveMarker(1); });
    mCustomPlot->addAction(actNextPoint);
}

QCustomPlotHelper::~QCustomPlotHelper()
{
    // 释放未挂父对象的 tracer 包装对象（其 QCP item 由图表托管，此处通过 removeItem 移除）
    qDeleteAll(mDataTracers);
    mDataTracers.clear();

    for (const DataMarker &marker : mDataMarkers)
        delete marker.tracer;
    mDataMarkers.clear();

    delete mTracerX;      mTracerX = nullptr;
    delete mTracerY;      mTracerY = nullptr;
    delete mTracerCrossLine; mTracerCrossLine = nullptr;
    delete mTracerDragLeftLine;  mTracerDragLeftLine = nullptr;
    delete mTracerDragRightLine; mTracerDragRightLine = nullptr;
    delete mDragStraightRect; mDragStraightRect = nullptr;
}

void QCustomPlotHelper::setupMenu()
{
    // mIconUnchecked = QIcon(dblroundPixmap(QSize(12, 12), Qt::gray, Qt::gray));
    // mIconChecked = QIcon(dblroundPixmap(QSize(12, 12), Qt::green, Qt::green));
    mIconUnchecked = QIcon(":/qss_icons/light/light_icons/radio_unchecked_disable.png");
    mIconChecked = QIcon(":/qss_icons/light/light_icons/radio_checked_focus.png");

    actResetView = new QAction(tr("恢复视图"), this);
    connect(actResetView, &QAction::triggered, this, &QCustomPlotHelper::resetView);
    actClearMarker = new QAction(tr("清除标签"), this);
    connect(actClearMarker, &QAction::triggered, this, &QCustomPlotHelper::clearMarker);
    actExportGraphic = new QAction(tr("导出图像...   "), this);
    connect(actExportGraphic, &QAction::triggered, this, &QCustomPlotHelper::exportGraphic);
    actExportData = new QAction(tr("导出数据...   "), this);
    connect(actExportData, &QAction::triggered, this, &QCustomPlotHelper::exportData);

    actLinearScale = new QAction(mIconChecked, tr("线性缩放"), this);
    connect(actLinearScale, &QAction::triggered, this, [this](){ linearScale(true); });
    actLogarithmicScale = new QAction(mIconUnchecked, tr("对数缩放"), this);
    connect(actLogarithmicScale, &QAction::triggered, this, [this](){ logarithmicScale(true); });

    actEnableStraightLine = new QAction(mIconUnchecked, tr("轴参考线 "), this);
    connect(actEnableStraightLine, &QAction::triggered, this, [this](){ enableStraightLine(!actEnableStraightLine->property("isChecked").toBool()); });
    actEnableAtDataPoint = new QAction(mIconUnchecked, tr("点位标记"), this);
    connect(actEnableAtDataPoint, &QAction::triggered, this, [this](){ enableAtDataPoint(!actEnableAtDataPoint->property("isChecked").toBool()); });
    actEnableRangeSelect = new QAction(mIconUnchecked, tr("选择范围"), this);
    connect(actEnableRangeSelect, &QAction::triggered, this, [this](){ enableRangeSelect(!actEnableRangeSelect->property("isChecked").toBool()); });
    actShowWaveformDetail = new QAction(tr("波形特征值"), this);
    connect(actShowWaveformDetail, &QAction::triggered, this, &QCustomPlotHelper::showWaveformDetail);

    // 创建悬浮窗实例
    actShowWaveformDetail->setIcon(actShowWaveformDetail->property("isChecked").toBool() ? mIconChecked : mIconUnchecked);
    mStatsPanel = new QCPWaveformStatsPanel(mCustomPlot);
    mStatsPanel->setVisible(false);

    QMenu *subMenu = new QMenu(tr("适应模式"));
    QWidgetAction* actFixedRange = new QWidgetAction(subMenu);

    QWidget* container = new QWidget(subMenu);
    QGridLayout* gridLayout = new QGridLayout(container);
    gridLayout->setContentsMargins(5, 2, 5, 2); // 调整内边距
    QCheckBox* checkBox1 = new QCheckBox(tr("自动适应"), container);
    QCheckBox* checkBox2 = new QCheckBox(tr("限定范围"), container);

    if (mCustomPlot->property("enableAutoScale").toBool())
        checkBox1->setChecked(true);
    else if (mCustomPlot->property("enableFixedScale").toBool())
        checkBox2->setChecked(true);

    QSpinBox* spinBoxFixed = new QSpinBox(container);
    spinBoxFixed->setRange(60, 65536);
    spinBoxFixed->setValue(mXAxisFixedRange);
    connect(spinBoxFixed, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value){
        mXAxisFixedRange = value;
        mCustomPlot->setProperty("xAxisFixedRange", mXAxisFixedRange);
    });
    connect(checkBox1, &QRadioButton::toggled, this, [=](bool checked){
        mCustomPlot->setProperty("enableAutoScale", checked);
        mCustomPlot->rescaleAxes(checked);//设置试图自由缩放
        if (checked){
            mCustomPlot->setProperty("enableFixedScale", false);
            mCustomPlot->setProperty("enableManualScale", false);
            checkBox2->setChecked(false);
        }

        emit autoScaleChanged(checked);
    });
    connect(checkBox2, &QRadioButton::toggled, this, [=](bool checked){
        mCustomPlot->setProperty("enableFixedScale", checked);
        if (checked){
            mCustomPlot->setProperty("enableAutoScale", false);
            mCustomPlot->setProperty("enableManualScale", false);
            mCustomPlot->rescaleAxes(false);//设置试图自由缩放
            checkBox1->setChecked(false);
        }

        emit fixedScaleChanged(checked);
    });

    gridLayout->addWidget(checkBox1, 0, 0);
    gridLayout->addWidget(checkBox2, 1, 0);
    gridLayout->addWidget(new QLabel(tr("显示最近记录数："), container), 1, 1);
    gridLayout->addWidget(spinBoxFixed, 1, 2);

    actFixedRange->setDefaultWidget(container);
    subMenu->addAction(actFixedRange);

    /* 新菜单模式
         *  恢复视图
         *  点位标记
         *  清除标签
         *  -
         *  导出图像
         *  导出数据
         *  -
         *  更多功能 - 轴参考线
         *            选择范围
         *            -
         *            线性缩放
         *            对数缩放
         *            -
         *            适应模式（只有在线测量实时刷新才会用到，所以优先级最低）
         */
    contextMenu.addAction(actResetView);
    contextMenu.addAction(actEnableAtDataPoint);
    if (mClearMarkerActionVisible)
        contextMenu.addAction(actClearMarker);

    contextMenu.addSeparator();
    contextMenu.addAction(actExportGraphic);
    contextMenu.addAction(actExportData);

    contextMenu.addSeparator();
    QMenu *otherMenu = contextMenu.addMenu(tr("更多功能"));

    if (mStraightLineActionVisible)
        otherMenu->addAction(actEnableStraightLine);
    if (mRangeSelectActionVisible)
        otherMenu->addAction(actEnableRangeSelect);

    otherMenu->addSeparator();
    otherMenu->addAction(actLinearScale);
    otherMenu->addAction(actLogarithmicScale);

    otherMenu->addSeparator();
    otherMenu->addAction(actShowWaveformDetail);
    otherMenu->addMenu(subMenu);
}

QCPAxisRect *QCustomPlotHelper::axisRectAt(const QPoint &pos) const
{
    QCPAxisRect *axisRect = mCustomPlot->axisRect();
    for (QCPAxisRect *rect : mCustomPlot->axisRects())
    {
        if (rect != axisRect && rect->rect().contains(pos))
            return rect;
    }
    return axisRect;
}
void QCustomPlotHelper::setLabel(const QString &strX, const QString &strY)
{
    QCPAxisRect *axisRect = mCustomPlot->axisRect();
    QCPAxis* yAxis = axisRect->axis(QCPAxis::AxisType::atLeft);
    QCPAxis* xAxis = axisRect->axis(QCPAxis::AxisType::atBottom);

    xAxis->setLabel(strX);
    yAxis->setLabel(strY);
}

QCPGraph *QCustomPlotHelper::addLineGraph(QCPAxis *keyAxis, QCPAxis *valueAxis)
{
    QCPGraph * graph = mCustomPlot->addGraph(keyAxis, valueAxis);
    //graph->setAntialiased(true);
    graph->setLineStyle(QCPGraph::lsLine);
    graph->setSelectable(QCP::SelectionType::stSingleData);
    return graph;
}

QCPGraph *QCustomPlotHelper::addScatterGraph(QCPAxis *keyAxis, QCPAxis *valueAxis)
{
    QCPGraph * graph = mCustomPlot->addGraph(keyAxis, valueAxis);
    graph->setLineStyle(QCPGraph::lsNone);
    graph->setSelectable(QCP::SelectionType::stSingleData);
    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::red, 5));
    return graph;
}

QCPGraph *QCustomPlotHelper::addLineScatterGraph(QCPAxis *keyAxis, QCPAxis *valueAxis)
{
    QCPGraph * graph = mCustomPlot->addGraph(keyAxis, valueAxis);
    graph->setLineStyle(QCPGraph::lsLine);
    graph->setSelectable(QCP::SelectionType::stSingleData);
    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::red, 5));
    return graph;
}

QCPGraph *QCustomPlotHelper::addColorMapGraph(QCPAxis *keyAxis, QCPAxis *valueAxis)
{
    QCPGraph * graph = mCustomPlot->addGraph(keyAxis, valueAxis);
    graph->setLineStyle(QCPGraph::lsLine);
    graph->setSelectable(QCP::SelectionType::stSingleData);
    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::red, 5));

    QCPColorMap *colorMap = new QCPColorMap(keyAxis, valueAxis);
    colorMap->setName("colorMap");

    colorMap->data()->setSize(100, 100);// 设置网格维度
    colorMap->data()->setRange(QCPRange(0, 10000), QCPRange(0, 1.0));// 设置网格数据范围
    colorMap->data()->fillAlpha(0);

    QCPColorScale *colorScale = new QCPColorScale(mCustomPlot);
    mCustomPlot->plotLayout()->addElement(0, 1, colorScale);
    colorScale->setType(QCPAxis::atRight);
    colorScale->setDataRange(QCPRange(0, 100));//颜色值取值范围

    //重新定义色带 （蓝绿黄红）
    QCPColorGradient gradient = QCPColorGradient::gpJet;
    colorScale->setGradient(gradient);

    colorMap->setColorScale(colorScale);// 色图与颜色条关联
    colorMap->setGradient(gradient/*QCPColorGradient::gpSpectrum*/);
    colorMap->rescaleDataRange();

    QCPMarginGroup *marginGroup = new QCPMarginGroup(mCustomPlot);
    mCustomPlot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setRangeDrag(false);
    colorScale->setRangeZoom(false);

    return graph;
}

void QCustomPlotHelper::setResetActionVisible(bool visible/* = true*/)
{
    mResetActionVisible = visible;
}

void QCustomPlotHelper::setClearMarkerActionVisible(bool visible/* = true*/)
{
    mClearMarkerActionVisible = visible;
}

void QCustomPlotHelper::setStraightLineActionVisible(bool visible/* = true*/)
{
    mStraightLineActionVisible = visible;
}

void QCustomPlotHelper::setRangeSelectActionVisible(bool visible/* = true*/)
{
    mRangeSelectActionVisible = visible;
}

void QCustomPlotHelper::setStatsPanelDockPosition(QCPWaveformStatsPanel::DockPosition pos)
{
    if (mStatsPanel)
    {
        mStatsPanel->setDockPosition(pos);
        mCustomPlot->replot(QCustomPlot::rpQueuedReplot);
    }
}

QPixmap QCustomPlotHelper::roundPixmap(QSize sz, QColor clrOut)
{
    QPixmap result(sz);
    result.fill(Qt::transparent);

    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform,true);

    QPainterPath bigCirclePath;
    bigCirclePath.addEllipse(0, 0, sz.width(), sz.height());
    painter.fillPath(bigCirclePath, QBrush(clrOut));

    return result;
}

QPixmap QCustomPlotHelper::dblroundPixmap(QSize sz, QColor clrIn, QColor clrOut)
{
    QPixmap result(sz);
    result.fill(Qt::transparent);

    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing,true);

    QPainterPath bigCirclePath;
    bigCirclePath.addEllipse(1, 1, sz.width()-2, sz.height()-2);
    painter.setPen(QPen(QBrush(clrOut), 2, Qt::SolidLine));
    painter.drawPath(bigCirclePath);

    QPainterPath smallCirclePath;
    smallCirclePath.addEllipse(4, 4, sz.width() - 8, sz.height() - 8);
    painter.fillPath(smallCirclePath, QBrush(clrIn));

    return result;
}

void QCustomPlotHelper::plottableClick(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent* event)
{
    QCustomPlot* customPlot = qobject_cast<QCustomPlot*>(sender());

    QCPGraph *graph = qobject_cast<QCPGraph*>(plottable);
    if (graph) {
        QCPAxis *axis = graph->keyAxis(); // 或者 graph->valueAxis();
        QCPAxisRect *axisRect = axis->axisRect();
        double key = graph->dataMainKey(dataIndex); // 获取X轴的值
        double value = graph->dataMainValue(dataIndex); // 获取Y轴的值

        if (actEnableAtDataPoint->property("isChecked").toBool())
        {
            // toggle: clicking a marked point again cancels its marker
            int markerIndex = -1;
            for (int i = 0; i < mDataMarkers.size(); ++i)
            {
                const DataMarker &marker = mDataMarkers[i];
                if (marker.graph == graph
                    && qFuzzyCompare(marker.key, key)
                    && qFuzzyCompare(marker.value, value))
                {
                    markerIndex = i;
                    break;
                }
            }

            if (markerIndex >= 0)
            {
                XCPValueMarker *tracer = mDataMarkers[markerIndex].tracer;
                if (tracer)
                    delete tracer;   // destructor removes the related items from the plot
                mDataMarkers.removeAt(markerIndex);
            }
            else
            {
                XCPValueMarker *tracer = new XCPValueMarker(mCustomPlot, XCPValueMarker::AtDataPoint);
                QColor fill = graph->pen().color();
                fill.setAlpha(255);
                tracer->setPen(graph->pen());
                tracer->setBrush(QBrush(fill));
                tracer->setTextPen(graph->pen());
                tracer->setVisible(true);
                tracer->updatePosition(axisRect, key, value);

                DataMarker marker;
                marker.tracer = tracer;
                marker.graph = graph;
                marker.key = key;
                marker.value = value;
                mDataMarkers.append(marker);
            }

            mCustomPlot->replot(QCustomPlot::rpQueuedReplot);
        }

        emit itemActived(graph, key, value);
    }
}

void QCustomPlotHelper::moveActiveMarker(int step)
{
    if (mDataMarkers.isEmpty())
        return;

    // 最近点击/标记的点位作为当前点位
    DataMarker &marker = mDataMarkers.last();
    QCPGraph *graph = marker.graph;
    if (!graph || !mCustomPlot->hasPlottable(graph))
        return;

    QSharedPointer<QCPGraphDataContainer> data = graph->data();
    if (data.isNull() || data->isEmpty())
        return;

    QCPGraphDataContainer::const_iterator it = data->findBegin(marker.key, false);
    if (it == data->constEnd() || !qFuzzyCompare(it->key, marker.key))
        return;   // 数据已变化，原标记不再对应有效数据点

    const int count = data->size();
    int index = int(it - data->constBegin()) + step;
    if (index < 0)
        index = 0;
    else if (index >= count)
        index = count - 1;

    const QCPGraphData &target = *(data->constBegin() + index);
    if (qFuzzyCompare(target.key, marker.key) && qFuzzyCompare(target.value, marker.value))
        return;   // 已到数据边界

    if (!marker.tracer)
        return;

    marker.key = target.key;
    marker.value = target.value;
    marker.tracer->updatePosition(graph->keyAxis()->axisRect(), target.key, target.value);

    mCustomPlot->replot(QCustomPlot::rpQueuedReplot);
    emit itemActived(graph, target.key, target.value);
}

void QCustomPlotHelper::mouseDoubleClick(QMouseEvent *event)
{
    QCustomPlot* customPlot = qobject_cast<QCustomPlot*>(sender());

    if (actEnableRangeSelect->property("isChecked").toBool())
    {
        // 缩放到指定范围
        if (mDragStraightRect)
        {
            mTracerDragLeftLine->setVisible(false);
            mTracerDragRightLine->setVisible(false);
            mDragStraightRect->setVisible(false);

            QCPAxisRect *axisRect = axisRectAt(event->pos());

            QCPAxis* yAxis = axisRect->axis(QCPAxis::AxisType::atLeft);
            QCPAxis* xAxis = axisRect->axis(QCPAxis::AxisType::atBottom);

            xAxis->rescale(false);
            xAxis->setRange(mDragStraightRect->range());
            yAxis->rescale(true);
            customPlot->replot(QCustomPlot::rpQueuedReplot);//曲线重绘
        }
    }
}

void QCustomPlotHelper::mousePress(QMouseEvent * event)
{
    QCustomPlot* customPlot = qobject_cast<QCustomPlot*>(sender());

    // 左键
    if (event->buttons() & Qt::LeftButton)
    {
        customPlot->setProperty("left-clicked", true);
        customPlot->setProperty("click-pos", event->pos());


        // 启用范围选取功能
        if (actEnableRangeSelect->property("isChecked").toBool())
        {
            customPlot->setProperty("graph-dragging", false);

            // 先禁用鼠标拖拽功能，防止正在拖动的时候图像发生移动
            customPlot->setInteraction(QCP::iRangeDrag, false);
        }
    }

    // 右键
    else if (event->buttons() & Qt::RightButton)
    {
    }
}

void QCustomPlotHelper::mouseMove(QMouseEvent * event)
{
    QCustomPlot* customPlot = qobject_cast<QCustomPlot*>(sender());

    // 左键
    if (event->buttons() & Qt::LeftButton)
    {
        if (!actEnableRangeSelect->property("isChecked").toBool())
            customPlot->setCursor(Qt::SizeAllCursor);
        else
            customPlot->setCursor(Qt::ArrowCursor);
    }
    // 右键
    else if (event->buttons() & Qt::RightButton)
    {
    }

    QCPAxisRect *axisRect = axisRectAt(event->pos());

    QCPAxis* yAxis = axisRect->axis(QCPAxis::AxisType::atLeft);
    QCPAxis* xAxis = axisRect->axis(QCPAxis::AxisType::atBottom);

    // 启用参考线或基准线
    if (actEnableStraightLine->property("isChecked").toBool())
    {
        //像素坐标转成实际的x,y轴的坐标
        double key = xAxis->pixelToCoord(event->pos().x());
        double value = yAxis->pixelToCoord(event->pos().y());

        if(Q_NULLPTR == mTracerX)
        {
            mTracerX = new XCPValueMarker(customPlot, XCPValueMarker::AtXAxis);//x轴
            mTracerX->setLabelVisible(false);//轴线上不显示值
        }
        mTracerX->updatePosition(axisRect, key, value);

        if(Q_NULLPTR == mTracerY)
        {
            mTracerY = new XCPValueMarker(customPlot, XCPValueMarker::AtYAxis);//y轴
            mTracerY->setLabelVisible(false);//轴线上不显示值
        }
        mTracerY->updatePosition(axisRect, key, value);

        int tracerCount = mDataTracers.count();
        int graphCount = customPlot->graphCount(axisRect);
        if(tracerCount < graphCount)
        {
            // 根据线条的个数，调整可显示数据点的个数
            for(int i = tracerCount; i < graphCount; ++i)
            {
                XCPValueMarker *tracer = new XCPValueMarker(customPlot, XCPValueMarker::AtDataPoint);
                tracer->setOffset(QPoint(0, -20));//加个偏移量，让值显示在右上角，免得被横线遮挡
                mDataTracers.append(tracer);
            }
        }

        else if(tracerCount > graphCount)
        {
            // 释放超出曲线数量的 tracer，避免列表无限增长
            for(int i = tracerCount - 1; i >= graphCount; --i)
            {
                XCPValueMarker *tracer = mDataTracers.takeAt(i);
                delete tracer;
            }
        }

        for (int i = 0; i < graphCount; ++i)
        {
            XCPValueMarker *tracer = mDataTracers[i];
            QCPGraph *graph = customPlot->graph(axisRect, i);
            if (tracer && graph)
            {
                const QPen pen = graph->pen();
                QColor fill = pen.color();
                fill.setAlpha(255);
                tracer->setPen(pen);
                tracer->setBrush(QBrush(fill));
                tracer->setTextPen(pen);
                tracer->setLabelVisible(true);
                tracer->updatePosition(axisRect, key, value);
            }
        }

        if(Q_NULLPTR == mTracerCrossLine)
            mTracerCrossLine = new XCPReferenceLine(customPlot, XCPReferenceLine::Crosshair);//直线
        mTracerCrossLine->updatePosition(axisRect, key, value);

        customPlot->replot(QCustomPlot::rpQueuedReplot);//曲线重绘
    }

    // 启用范围选取功能
    if (actEnableRangeSelect->property("isChecked").toBool())
    {
        if (customPlot->property("left-clicked").toBool()){
            if (!customPlot->property("graph-dragging").toBool())
            {
                // 设置鼠标拖拽开始
                customPlot->setProperty("graph-dragging", true);

                // 记录拖拽起始点值
                double key = xAxis->pixelToCoord(event->pos().x());

                if(Q_NULLPTR == mTracerDragLeftLine)
                    mTracerDragLeftLine = new XCPReferenceLine(customPlot, XCPReferenceLine::Crosshair);
                mTracerDragLeftLine->updatePosition(axisRect, key, yAxis->range().lower);
                mTracerDragLeftLine->setProperty("key", key);
                customPlot->setCursor(Qt::SplitHCursor);
                customPlot->replot(QCustomPlot::rpQueuedReplot);
            }
            else
            {
                double key = xAxis->pixelToCoord(event->pos().x());
                if(Q_NULLPTR == mTracerDragRightLine)
                    mTracerDragRightLine = new XCPReferenceLine(customPlot, XCPReferenceLine::Crosshair);//直线
                mTracerDragRightLine->updatePosition(axisRect, key, yAxis->range().lower);
                mTracerDragRightLine->setProperty("key", key);

                //框选完毕，将选中的点颜色更新
                double keyFrom = mTracerDragLeftLine->property("key").toDouble();
                double keyTo = mTracerDragRightLine->property("key").toDouble();
                double keyTemp = keyFrom;
                keyFrom = qMin(keyFrom, keyTo);
                keyTo = qMax(keyTemp, keyTo);

                if(Q_NULLPTR == mDragStraightRect)
                    mDragStraightRect = new XCPRangeMask(customPlot);//蒙版
                mDragStraightRect->setRange(axisRect, QCPRange(keyFrom, keyTo));
                mDragStraightRect->setVisible(true);

                for (int i = 0; i < mCustomPlot->graphCount(axisRect); ++i)
                {
                    QCPGraph *graph = mCustomPlot->graph(axisRect, i);
                    // recolor only: mutate the data container directly instead of setData (no data rebuild)
                    QSharedPointer<QCPGraphDataContainer> graphData = graph->data();
                    const QColor clrNormal = graph->scatterStyle().pen().color();
                    const QColor clrSelected = clrNormal.darker();
                    for (QCPGraphDataContainer::iterator it = graphData->begin(); it != graphData->end(); ++it)
                    {
                        const bool inRange = (qFuzzyCompare(it->key, keyFrom) || it->key >= keyFrom)
                                          && (qFuzzyCompare(it->key, keyTo) || it->key <= keyTo);
                        it->color = inRange ? clrSelected : clrNormal;
                    }
                }

                mTracerDragLeftLine->setVisible(true);
                mTracerDragRightLine->setVisible(true);
                customPlot->setCursor(Qt::SplitHCursor);
                customPlot->replot(QCustomPlot::rpQueuedReplot);                
            }
        }
        else
        {
            customPlot->setCursor(Qt::ArrowCursor);
        }
    }
}

void QCustomPlotHelper::mouseRelease(QMouseEvent * event)
{
    QCustomPlot* customPlot = qobject_cast<QCustomPlot*>(sender());

    // 左键
    if (event->button() == Qt::LeftButton)
    {
        if (actEnableRangeSelect->property("isChecked").toBool())
        {
            QCPAxisRect *axisRect = axisRectAt(event->pos());

            if (mDragStraightRect)
                emit selectRangeChanged(axisRect, mDragStraightRect->range());
        }
    }
    // 右键
    else if (event->button() == Qt::RightButton)
    {
        QCPAxisRect *axisRect = axisRectAt(event->pos());

        // 检查是否允许弹出右键菜单
        bool allow = true;
        if (onContextMenu)
            onContextMenu(axisRect, allow);
        if (!allow)
            return;

        contextMenu.exec(QCursor::pos());
    }


    if (actEnableRangeSelect->property("isChecked").toBool()
        && customPlot->property("left-clicked").toBool()
        && customPlot->property("graph-dragging").toBool())
    {
        QToolTip::showText(event->globalPos(), tr("双击可缩放至可视范围。"), customPlot);
    }

    customPlot->setCursor(Qt::ArrowCursor);
    customPlot->setInteraction(QCP::iRangeDrag, true);
    customPlot->setProperty("left-clicked", false);
}

void QCustomPlotHelper::setGraphCheckBoxList(QCustomPlot* customPlot, const QCPAxisRect *axisRect)
{
    // 隐藏图例
    customPlot->legend->setVisible(false);
    if (axisRect == nullptr)
        axisRect = customPlot->axisRect();

    // 在你的主窗口构造函数里，customPlot初始化后加这几行
    // 1. 在最顶层（overlay层上方）插入新图层
    customPlot->addLayer("custom_widget_layer", customPlot->layer("overlay"));
    // 2. 把这个图层设为缓冲模式，独立重绘不刷新其他绘图内容
    customPlot->layer("custom_widget_layer")->setMode(QCPLayer::lmBuffered);
    // 3. 把当前默认层切到这个新层，后续生成的QCPItemWidget自动落在这个层
    customPlot->setCurrentLayer("custom_widget_layer");

    int graphCount = customPlot->graphCount(axisRect);

    // 初始化QComboBox
    QxtCheckComboBox *comboBox = new QxtCheckComboBox(customPlot);
    comboBox->setMinimumWidth(180);
    comboBox->setMaximumWidth(400);
    comboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    comboBox->setLayoutDirection(Qt::LeftToRight);
    // 开启内容自动适配模式
    comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    for (int i = 0; i < graphCount; ++i){
        QCPGraph *graph = customPlot->graph(axisRect, i);
        comboBox->addItem(graph->name());
        comboBox->setItemIcon(i, roundPixmap(QSize(16,16), graph->pen().color()));
        comboBox->setItemCheckState(i, Qt::Checked);
    }
    connect(qobject_cast<QStandardItemModel*>(comboBox->model()), &QStandardItemModel::itemChanged, this, [=](QStandardItem* item){
        QCPGraph *graph = customPlot->graph(axisRect, item->row());
        if (graph){
            graph->setVisible(item->checkState() == Qt::Checked);
            customPlot->replot(QCustomPlot::rpQueuedReplot);
        }
    });
    comboBox->setDefaultText(QStringLiteral("图像通道列表(空)"));
    mCbbAxisRect[axisRect->objectName()].push_back(comboBox);
}

void QCustomPlotHelper::updateWaveformDetail()
{
    if (!mStatsPanel) return;

    const int graphCount = mCustomPlot->graphCount();
    if (graphCount == 0) return;

    // 一次遍历：优先所选波形，否则取首个可视波形
    QCPGraph *mainGraph = mCustomPlot->graph(0);
    for (int i = 0; i < graphCount; ++i)
    {
        QCPGraph *g = mCustomPlot->graph(i);
        if (g && g->selected()) { mainGraph = g; break; }
    }
    if (!mainGraph || mainGraph->data()->isEmpty())
    {
        for (int i = 0; i < graphCount; ++i)
        {
            QCPGraph *g = mCustomPlot->graph(i);
            if (g && g->visible() && !g->data()->isEmpty()) { mainGraph = g; break; }
        }
    }
    if (!mainGraph || mainGraph->data()->isEmpty()) return;

    const QCPRange xr = mCustomPlot->xAxis->range();
    auto dataPtr = mainGraph->data();
    const int dataCount = dataPtr->size();
    const double firstKey = dataCount > 0 ? dataPtr->constBegin()->key : 0;
    const double lastKey = dataCount > 0 ? (dataPtr->constEnd() - 1)->key : 0;

    // 数据、范围均未变化则跳过计算
    if (mStatsValid && mStatsGraph == mainGraph && mStatsDataCount == dataCount
        && qFuzzyCompare(mStatsFirstKey, firstKey) && qFuzzyCompare(mStatsLastKey, lastKey)
        && qFuzzyCompare(mStatsXRange.lower, xr.lower) && qFuzzyCompare(mStatsXRange.upper, xr.upper))
        return;

    mStatsGraph = mainGraph;
    mStatsDataCount = dataCount;
    mStatsFirstKey = firstKey;
    mStatsLastKey = lastKey;
    mStatsXRange = xr;
    mStatsValid = true;

    auto itBegin = dataPtr->findBegin(xr.lower, true);
    auto itEnd = dataPtr->findEnd(xr.upper, true);
    QVector<double> data;
    data.reserve(itEnd - itBegin);
    for (auto it = itBegin; it != dataPtr->constEnd() && it->key <= xr.upper; ++it)
        data.append(it->value);

    auto stats = WaveformStats::calculate(data);

    mStatsPanel->clearAll();
    mStatsPanel->addText("点数：", QString::number(data.size()));
    mStatsPanel->addText("峰值：", QString::number(stats.max, 'f', 2));
    mStatsPanel->addText("谷值：", QString::number(stats.min, 'f', 2));
    mStatsPanel->addText("基线：", QString::number(stats.baseline, 'f', 2));
    mStatsPanel->addText("峰峰值：", QString::number(stats.peak2peak, 'f', 2));
    mStatsPanel->addText("平均值：", QString::number(stats.mean, 'f', 2));
    mStatsPanel->addText("标准差：", QString::number(stats.stdDev, 'f', 2));
}

void QCustomPlotHelper::beforeReplot()
{
    QCustomPlot* customPlot = qobject_cast<QCustomPlot*>(sender());
    if (!customPlot) return; // 必须做空指针防御，避免信号来自其他对象触发崩溃

    if (customPlot->graphCount() == 0) return;

    if (!actShowWaveformDetail->property("isChecked").toBool())
        return;

    updateWaveformDetail();
}


void QCustomPlotHelper::afterLayout()
{
    QCustomPlot* customPlot = qobject_cast<QCustomPlot*>(sender());
    QList<QCPAxisRect*> axisRects = customPlot->axisRects();

    foreach (QCPAxisRect *axisRect, axisRects)
    {
        auto it = mCbbAxisRect.constFind(axisRect->objectName());
        if (it != mCbbAxisRect.constEnd()){
            int i = 0;
            for (QComboBox *comboBox : it.value()){
                if (comboBox->layoutDirection() == Qt::LeftToRight)
                    comboBox->move(axisRect->left() + 10, axisRect->topRight().y() + i++ * 20 + 5);// 显示左上角
                else
                    comboBox->move(axisRect->right() - comboBox->width() - 10, axisRect->topRight().y() + i++ * 20 + 5);// 显示右上角
            }
        }
    }

    if (actEnableRangeSelect->property("isChecked").toBool())
    {
        if (mDragStraightRect)
        {
            mDragStraightRect->refresh();
        }
    }
}

void QCustomPlotHelper::enableStraightLine(bool enable)
{
    actEnableStraightLine->setProperty("isChecked", enable);
    actEnableStraightLine->setIcon(enable ? mIconChecked : mIconUnchecked);

    {
        if (mTracerX)
            mTracerX->setVisible(enable);
        if (mTracerY)
            mTracerY->setVisible(enable);
        if (mTracerCrossLine)
            mTracerCrossLine->setVisible(enable);
        for (auto tracer : mDataTracers)
            tracer->setVisible(enable);
        mCustomPlot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void QCustomPlotHelper::enableAtDataPoint(bool enable/* = true*/)
{
    actEnableAtDataPoint->setProperty("isChecked", enable);
    actEnableAtDataPoint->setIcon(enable ? mIconChecked : mIconUnchecked);

    {
        //mCustomPlot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void QCustomPlotHelper::clearMarker()
{
    for (auto tracer : mDataTracers)
        tracer->setVisible(false);

    for (const DataMarker &marker : mDataMarkers)
    {
        if (marker.tracer)
            delete marker.tracer;
    }
    mDataMarkers.clear();

    mCustomPlot->replot(QCustomPlot::rpQueuedReplot);
}

void QCustomPlotHelper::enableRangeSelect(bool enable/* = true*/)
{
    actEnableRangeSelect->setProperty("isChecked", enable);
    actEnableRangeSelect->setIcon(enable ? mIconChecked : mIconUnchecked);

    if (mTracerDragLeftLine)
        mTracerDragLeftLine->setVisible(enable);
    if (mTracerDragRightLine)
        mTracerDragRightLine->setVisible(enable);
    if (mDragStraightRect)
        mDragStraightRect->setVisible(enable);

    if (!enable)
    {
        // 恢复值的颜色
        for (int i = 0; i < mCustomPlot->graphCount(); ++i)
        {
            QCPGraph *graph = mCustomPlot->graph(i);
            QSharedPointer<QCPGraphDataContainer> graphData = graph->data();
            const QColor clrNormal = graph->scatterStyle().pen().color();
            for (QCPGraphDataContainer::iterator it = graphData->begin(); it != graphData->end(); ++it)
                it->color = clrNormal;
        }
        mCustomPlot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void QCustomPlotHelper::showWaveformDetail()
{
    bool isChecked = actShowWaveformDetail->property("isChecked").toBool();
    actShowWaveformDetail->setProperty("isChecked", !isChecked);
    actShowWaveformDetail->setIcon(!isChecked ? mIconChecked : mIconUnchecked);
    mStatsPanel->setVisible(!isChecked);

    if (mStatsPanel->visible()){
        mStatsValid = false;   // 强制重新计算一次
        updateWaveformDetail();
        mCustomPlot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void QCustomPlotHelper::resetView()
{
    mCustomPlot->rescaleAxes(true);
    mCustomPlot->replot(QCustomPlot::rpQueuedReplot);
}


namespace {
QString csvField(const QString &text)
{
    if (text.contains(',') || text.contains('"') || text.contains('\n'))
    {
        QString escaped = text;
        escaped.replace('"', "\"\"");
        return '"' + escaped + '"';
    }
    return text;
}
}

void QCustomPlotHelper::exportGraphic()
{
    QString filePath = QFileDialog::getSaveFileName(nullptr, tr("导出图像"), "", tr("图像文件 (*.png);;所有文件 (*.*)"));
    if (!filePath.isEmpty()){
        if (!filePath.endsWith(".png"))
            filePath += ".png";
        if (!mCustomPlot->savePng(filePath, 1920, 1080))
            QMessageBox::information(nullptr, tr("提示"), tr("导出失败！"));
    }
}

void QCustomPlotHelper::exportData()
{
    QString filePath = QFileDialog::getSaveFileName(nullptr, tr("导出数据"), "", tr("CSV 文件 (*.csv);;所有文件 (*.*)"));
    if (filePath.isEmpty())
        return;

    if (!filePath.endsWith(".csv", Qt::CaseInsensitive))
        filePath += ".csv";

    QCPAxisRect *axisRect = axisRectAt(QCursor::pos());

    struct GraphInfo {
        QString name;
        QVector<double> keys;
        QVector<double> values;
    };
    QList<GraphInfo> graphs;
    int maxRows = 0;

    for (int i = 0; i < mCustomPlot->graphCount(axisRect); ++i) {
        QCPGraph *graph = mCustomPlot->graph(axisRect, i);
        if (!graph || !graph->visible())
            continue;

        GraphInfo info;
        info.name = graph->name();
        auto data = graph->data();
        for (auto it = data->constBegin(); it != data->constEnd(); ++it) {
            info.keys.append(it->key);
            info.values.append(it->value);
        }
        maxRows = qMax(maxRows, info.keys.size());
        graphs.append(info);
    }

    if (graphs.isEmpty()) {
        QMessageBox::information(nullptr, tr("提示"), tr("没有可导出的曲线数据！"));
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::information(nullptr, tr("提示"), tr("导出失败！"));
        return;
    }

    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::System);
#else
    out.setCodec("GBK");
#endif

    for (int col = 0; col < graphs.size(); ++col) {
        if (col > 0)
            out << ',';
        out << csvField(graphs[col].name + "_X") << ',' << csvField(graphs[col].name + "_Y");
    }
    out << '\n';

    for (int row = 0; row < maxRows; ++row) {
        for (int col = 0; col < graphs.size(); ++col) {
            if (col > 0)
                out << ',';
            if (row < graphs[col].keys.size()) {
                out << QString::number(graphs[col].keys.at(row), 'g', 12)
                    << ','
                    << QString::number(graphs[col].values.at(row), 'g', 12);
            } else {
                out << ',';
            }
        }
        out << '\n';
    }
}


void QCustomPlotHelper::linearScale(bool enable)
{
    actLinearScale->setProperty("isChecked", enable);
    actLogarithmicScale->setProperty("isChecked", !enable);
    actLinearScale->setIcon(mIconChecked);
    actLogarithmicScale->setIcon(mIconUnchecked);

    QCPAxisRect *axisRect = mCustomPlot->axisRect();
    QCPAxis* yAxis = axisRect->axis(QCPAxis::AxisType::atLeft);
    QSharedPointer<QCPAxisTicker> ticker(new QCPAxisTicker);
    yAxis->setTicker(ticker);

    yAxis->setScaleType(QCPAxis::ScaleType::stLinear);
    yAxis->setNumberFormat("f");
    yAxis->setNumberPrecision(0);

    yAxis->rescale(true);
    mCustomPlot->replot();
}

void QCustomPlotHelper::logarithmicScale(bool enable)
{
    actLogarithmicScale->setProperty("isChecked", enable);
    actLinearScale->setProperty("isChecked", !enable);
    actLogarithmicScale->setIcon(mIconChecked);
    actLinearScale->setIcon(mIconUnchecked);

    QCPAxisRect *axisRect = mCustomPlot->axisRect();
    QCPAxis* yAxis = axisRect->axis(QCPAxis::AxisType::atLeft);
    QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
    yAxis->setTicker(logTicker);

    yAxis->setScaleType(QCPAxis::ScaleType::stLogarithmic);
    yAxis->setNumberFormat("eb");//使用科学计数法表示刻度
    yAxis->setNumberPrecision(0);//小数点后面小数位数

    yAxis->rescale(true);
    mCustomPlot->replot();
}