#include "qcustomplothelper.h"
#include <QMouseEvent>

//////////////////////////////////////////////////////////////////////////////////
XCPItemTracer::XCPItemTracer(QCustomPlot *customPlot, TracerType _type, QObject *parent)
    : QObject(parent)
    , mOffset(QPoint(0, 0))
    , mCustomPlot(customPlot)
    , mTracerType(_type)
{
    {
        QColor clrDefault(Qt::red);
        QBrush brushDefault(Qt::NoBrush);
        QPen penDefault(clrDefault);
        //        penDefault.setBrush(brushDefault);
        penDefault.setWidthF(0.5);

        mTracer = new QCPItemTracer(mCustomPlot);
        mTracer->setObjectName("mTracer");
        mTracer->setLayer("overlay");
        mTracer->setStyle(QCPItemTracer::tsCircle);
        mTracer->setPen(penDefault);
        mTracer->setBrush(brushDefault);

        mTracerText = new QCPItemText(mCustomPlot);
        mTracerText->setObjectName("mTracerText");
        mTracerText->setLayer("overlay");
        mTracerText->setClipToAxisRect(false);
        mTracerText->setPadding(QMargins(6, 4, 6, 4));
        mTracerText->setBrush(QBrush(QColor(255, 255, 255, 230)));
        mTracerText->setPen(penDefault);
        mTracerText->position->setParentAnchor(mTracer->position);
        mTracerText->setFont(QFont("Arial", 8, QFont::Weight::Thin));
        mTracerText->setColor(clrDefault);
        mTracerText->setText("");

        mTracerArrow = new QCPItemLine(mCustomPlot);
        mTracerArrow->setObjectName("mTracerArrow");
        QPen  arrowPen(clrDefault, 1);
        mTracerArrow->setPen(penDefault);
        mTracerArrow->setLayer("overlay");
        mTracerArrow->setClipToAxisRect(false);
        mTracerArrow->setHead(QCPLineEnding::esSpikeArrow);//设置头部为箭头形状

        switch (mTracerType)
        {
        case XAxisTracer:
        {
            mTracer->position->setTypeX(QCPItemPosition::ptPlotCoords);
            mTracer->position->setTypeY(QCPItemPosition::ptAxisRectRatio);
            mTracer->setSize(7);
            mTracerText->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);

            mTracerArrow->end->setParentAnchor(mTracer->position);
            mTracerArrow->start->setParentAnchor(mTracerArrow->end);
            mTracerArrow->start->setCoords(0, 20);//偏移量
            break;
        }
        case YAxisTracer:
        {
            mTracer->position->setTypeX(QCPItemPosition::ptAxisRectRatio);
            mTracer->position->setTypeY(QCPItemPosition::ptPlotCoords);
            mTracer->setSize(7);
            mTracerText->setPositionAlignment(Qt::AlignRight | Qt::AlignHCenter);

            mTracerArrow->end->setParentAnchor(mTracer->position);
            mTracerArrow->start->setParentAnchor(mTracerText->position);
            mTracerArrow->start->setCoords(-20, 0);//偏移量
            break;
        }
        case DataTracer:
        {
            mTracer->position->setTypeX(QCPItemPosition::ptPlotCoords);
            mTracer->position->setTypeY(QCPItemPosition::ptPlotCoords);
            mTracer->setSize(5);

            mTracerText->setPositionAlignment(Qt::AlignLeft | Qt::AlignVCenter);

            mTracerArrow->end->setParentAnchor(mTracer->position);
            mTracerArrow->start->setParentAnchor(mTracerArrow->end);
            mTracerArrow->start->setCoords(20 + mOffset.x(), mOffset.y());
            break;
        }
        default:
            break;
        }

        //setVisible(false);
    }
}

XCPItemTracer::~XCPItemTracer()
{
    if(mCustomPlot)
    {
        if (mTracer)
            mCustomPlot->removeItem(mTracer);
        if (mTracerText)
            mCustomPlot->removeItem(mTracerText);
        if (mTracerArrow)
            mCustomPlot->removeItem(mTracerArrow);
    }
}

void XCPItemTracer::setPen(const QPen &pen)
{
    if(mTracer)
        mTracer->setPen(pen);
    if(mTracerArrow)
        mTracerArrow->setPen(pen);
}

void XCPItemTracer::setBrush(const QBrush &brush)
{
    if(mTracer)
        mTracer->setBrush(brush);
}

void XCPItemTracer::setTextPen(const QPen &pen)
{
    if(mTracerText)
    {
        mTracerText->setPen(pen);
        mTracerText->setBrush(QBrush(QColor(255, 255, 255, 230)));
        mTracerText->setColor(pen.color());
    }
}

void XCPItemTracer::setText(const QString &text)
{
    if(mTracerText)
        mTracerText->setText(text);
}

void XCPItemTracer::setVisible(bool visible)
{
    setTracerVisible(visible);
    setTextVisible(visible);
}

void XCPItemTracer::setTracerVisible(bool visible)
{
    mTracerVisible = visible;
    if(mTracer)
        mTracer->setVisible(mTracerVisible);
}

void XCPItemTracer::setTextVisible(bool visible)
{
    mTextVisible = visible;
    if(mTracerText)
        mTracerText->setVisible(mTextVisible);
    if(mTracerArrow)
        mTracerArrow->setVisible(mTextVisible);
}

void XCPItemTracer::setOffset(QPoint offset)
{
    mOffset = offset;
}

void XCPItemTracer::updatePosition(QCPAxisRect *axisRect, double xValue, double yValue)
{
    if (!mTextVisible && !mTracerVisible)
        return;

    QCPAxis* xAxis = axisRect->axis(QCPAxis::AxisType::atBottom);
    QCPAxis* yAxis = axisRect->axis(QCPAxis::AxisType::atLeft);
    if (yValue > yAxis->range().upper)
        yValue = yAxis->range().upper;

    int xNumberPrecision = 1;//xAxis->numberPrecision();
    int yNumberPrecision = 1;//yAxis->numberPrecision();
    switch (mTracerType)
    {
    case XAxisTracer:
    {
        mTracer->position->setAxisRect(axisRect);
        mTracer->position->setAxes(xAxis, yAxis);
        mTracer->position->setCoords(xValue, 1);
        mTracerText->position->setCoords(0, 15);
        mTracerArrow->start->setCoords(0, 15);
        mTracerArrow->end->setCoords(0, 0);
        setText(QString::number(xValue, 'f', xNumberPrecision));
        break;
    }
    case YAxisTracer:
    {
        mTracer->position->setAxisRect(axisRect);
        mTracer->position->setAxes(xAxis, yAxis);
        mTracer->position->setCoords(0, yValue);
        mTracerText->position->setCoords(-20, 0);
        //        mTracerArrow->start->setCoords(20, 0);
        //        mTracerArrow->end->setCoords(0, 0);
        setText(QString::number(yValue, 'f', yNumberPrecision));
        break;
    }
    case DataTracer:
    {
        mTracer->position->setAxisRect(axisRect);
        mTracer->position->setAxes(xAxis, yAxis);
        mTracer->position->setCoords(xValue, yValue);
        mTracerText->position->setCoords(20 + mOffset.x(), mOffset.y());
        mTracerArrow->start->setCoords(20 + mOffset.x(), mOffset.y() == 0 ? 0 : (mOffset.y() / 2 + 2));
        setText(QString("x:%1,y:%2").arg(QString::number(xValue, 'f', xNumberPrecision)).arg(QString::number(yValue, 'f', yNumberPrecision)));
        break;
    }
    default:
        break;
    }

    //设置裁剪区域，超出边界就不显示了
    mTracer->setClipToAxisRect(true);
    mTracer->setClipAxisRect(axisRect);
    mTracerText->setClipToAxisRect(true);
    mTracerText->setClipAxisRect(axisRect);
    mTracerArrow->setClipToAxisRect(true);
    mTracerArrow->setClipAxisRect(axisRect);
}

//////////////////////////////////////////////////////////////////////////////////
XCPItemTracerLine::XCPItemTracerLine(QCustomPlot *customPlot, LineType _type, QObject *parent)
    : QObject(parent)
    , mCustomPlot(customPlot)
    , mLineType(_type)
{
    initLine();
}

XCPItemTracerLine::~XCPItemTracerLine()
{
    if(mCustomPlot)
    {
        if (mStraightLineV)
            mCustomPlot->removeItem(mStraightLineV);
        if (mStraightLineH)
            mCustomPlot->removeItem(mStraightLineH);
    }
}

void XCPItemTracerLine::initLine()
{
    if(mCustomPlot)
    {
        QPen linesPen(Qt::red, 1, Qt::DashLine);

        if(VerticalLine == mLineType || Both == mLineType)
        {
            mStraightLineV = new QCPItemStraightLine(mCustomPlot);//垂直线
            mStraightLineV->setLayer("overlay");
            mStraightLineV->setPen(linesPen);
            mStraightLineV->setClipToAxisRect(true);
            mStraightLineV->point1->setCoords(0, 0);
            mStraightLineV->point2->setCoords(0, 0);
        }

        if(HorizonLine == mLineType || Both == mLineType)
        {
            mStraightLineH = new QCPItemStraightLine(mCustomPlot);//水平线
            mStraightLineH->setLayer("overlay");
            mStraightLineH->setPen(linesPen);
            mStraightLineH->setClipToAxisRect(true);
            mStraightLineH->point1->setCoords(0, 0);
            mStraightLineH->point2->setCoords(0, 0);
        }
    }
}

void XCPItemTracerLine::updatePosition(QCPAxisRect *axisRect, double xValue, double yValue)
{
    QCPAxis* yAxis = axisRect->axis(QCPAxis::AxisType::atLeft);
    QCPAxis* xAxis = axisRect->axis(QCPAxis::AxisType::atBottom);

    if(VerticalLine == mLineType || Both == mLineType)
    {
        if(mStraightLineV)
        {
            mStraightLineV->point1->setCoords(xValue, yAxis->range().lower);
            mStraightLineV->point2->setCoords(xValue, yAxis->range().upper);
        }
    }

    if(HorizonLine == mLineType || Both == mLineType)
    {
        if(mStraightLineH)
        {
            mStraightLineH->point1->setCoords(xAxis->range().lower, yValue);
            mStraightLineH->point2->setCoords(xAxis->range().upper, yValue);
        }
    }

    //设置裁剪区域，超出边界就不显示了
    mStraightLineH->point1->setAxisRect(axisRect);
    mStraightLineH->point2->setAxisRect(axisRect);
    mStraightLineH->point1->setAxes(xAxis, yAxis);
    mStraightLineH->point2->setAxes(xAxis, yAxis);
    mStraightLineV->point1->setAxisRect(axisRect);
    mStraightLineV->point2->setAxisRect(axisRect);
    mStraightLineV->point1->setAxes(xAxis, yAxis);
    mStraightLineV->point2->setAxes(xAxis, yAxis);
    mStraightLineH->setClipToAxisRect(true);
    mStraightLineH->setClipAxisRect(axisRect);
    mStraightLineV->setClipToAxisRect(true);
    mStraightLineV->setClipAxisRect(axisRect);
}

void XCPItemTracerLine::setVisible(bool visible)
{
    if(mStraightLineV)
        mStraightLineV->setVisible(visible);
    if(mStraightLineH)
        mStraightLineH->setVisible(visible);
}

//////////////////////////////////////////////////////////////////////////////////
XCPItemStraightRect::XCPItemStraightRect(QCustomPlot *customPlot, StraightType _type, QObject *parent)
    : QObject(parent)
    , mCustomPlot(customPlot)
    , mStraightType(_type)
{
    mMaskRect = new QCPItemRect(mCustomPlot);
    mMaskRect->setLayer("overlay");
    mMaskRect->setPen(QColor::fromRgb(0x00, 0xff, 0xff, 0x2f));
    mMaskRect->setBrush(QColor::fromRgb(0x00, 0xff, 0xff, 0x2f));
    mMaskRect->setClipToAxisRect(true);
    mMaskRect->topLeft->setCoords(0, 0);
    mMaskRect->bottomRight->setCoords(0, 0);
}

XCPItemStraightRect::~XCPItemStraightRect()
{
    if(mCustomPlot)
    {
        if (mMaskRect)
            mCustomPlot->removeItem(mMaskRect);
    }
}

void XCPItemStraightRect::setRange(QCPAxisRect *axisRect, const QCPRange& range)
{
    if(mMaskRect)
    {
        QCPAxis* yAxis = axisRect->axis(QCPAxis::AxisType::atLeft);
        QCPAxis* xAxis = axisRect->axis(QCPAxis::AxisType::atBottom);

        if (VerticalStraight == mStraightType)
        {
            mMaskRect->topLeft->setCoords(range.lower, yAxis->range().upper);
            mMaskRect->bottomRight->setCoords(range.upper, yAxis->range().lower);
        }
        else
        {
            mMaskRect->topLeft->setCoords(xAxis->range().lower, range.lower);
            mMaskRect->bottomRight->setCoords(xAxis->range().upper, range.upper);
        }

        this->mRange = range;
    }
}

QCPRange XCPItemStraightRect::range() const
{
    return this->mRange;
}

void XCPItemStraightRect::setVisible(bool visible)
{
    if(mMaskRect)
        mMaskRect->setVisible(visible);
}

void XCPItemStraightRect::setBrush(const QBrush &brush)
{
    mMaskRect->setBrush(brush);
}

void XCPItemStraightRect::refresh()
{
    return;//
    if(mMaskRect)
    {
        QCPAxisRect *axisRect = mCustomPlot->axisRect();
        QCPAxis* yAxis = axisRect->axis(QCPAxis::AxisType::atLeft);
        QCPAxis* xAxis = axisRect->axis(QCPAxis::AxisType::atBottom);

        mMaskRect->topLeft->setCoords(mMaskRect->topLeft->key(), yAxis->range().upper);
        mMaskRect->bottomRight->setCoords(mMaskRect->bottomRight->key(), yAxis->range().lower);
    }
}

//////////////////////////////////////////////////////////////////////////////////

QCustomPlotHelper::QCustomPlotHelper(QCustomPlot* customPlot, QObject *parent)
    : QObject(parent)
    , mCustomPlot(customPlot)
    , mDataTracers(QList<XCPItemTracer *>())
{
    // 关联右键菜单事件
    mIconUnchecked = QIcon(dblroundPixmap(QSize(12, 12), Qt::gray, Qt::gray));
    mIconChecked = QIcon(dblroundPixmap(QSize(12, 12), Qt::green, Qt::green));

    actResetView = new QAction(tr("恢复视图"), this);
    connect(actResetView, &QAction::triggered, this, &QCustomPlotHelper::resetView);
    actClearMarker = new QAction(tr("清除标签"), this);
    connect(actClearMarker, &QAction::triggered, this, &QCustomPlotHelper::clearMarker);
    actExportGraphic = new QAction(tr("导出图像..."), this);
    connect(actExportGraphic, &QAction::triggered, this, &QCustomPlotHelper::exportGraphic);
    actExportData = new QAction(tr("导出数据..."), this);
    connect(actExportData, &QAction::triggered, this, &QCustomPlotHelper::exportData);

    actLinearScale = new QAction(mIconChecked, tr("线性缩放"), this);
    connect(actLinearScale, &QAction::triggered, this, &QCustomPlotHelper::linearScale);
    actLogarithmicScale = new QAction(mIconUnchecked, tr("对数缩放"), this);
    connect(actLogarithmicScale, &QAction::triggered, this, &QCustomPlotHelper::logarithmicScale);

    actEnableStraightLine = new QAction(mIconUnchecked, tr("轴参考线"), this);
    connect(actEnableStraightLine, &QAction::triggered, this, &QCustomPlotHelper::enableStraightLine);
    actEnableDataTracer = new QAction(mIconUnchecked, tr("点位标记"), this);
    connect(actEnableDataTracer, &QAction::triggered, this, &QCustomPlotHelper::enableDataTracer);
    actEnableRangeSelect = new QAction(mIconUnchecked, tr("选择范围"), this);
    connect(actEnableRangeSelect, &QAction::triggered, this, &QCustomPlotHelper::enableRangeSelect);

    customPlot->setProperty("enableAutoScale", true);//图像启用自缩放功能
    customPlot->setProperty("enableFixedScale", false);//图像禁用自定义范围
    customPlot->setProperty("enableManualScale", false);//图像启用缩放控制功能
    customPlot->setProperty("xAxisFixedRange", mXAxisFixedRange);
    customPlot->rescaleAxes(true);//设置试图自由缩放
    customPlot->setAntialiasedElements(QCP::aeAll);//设置所有图形元素使用抗锯齿渲染
    customPlot->legend->setVisible(true);//设置图例可见
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
    connect(mCustomPlot, &QCustomPlot::afterLayout, this, &QCustomPlotHelper::afterLayout);

    // 坐标轴同步缩放
    connect(mCustomPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), mCustomPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(mCustomPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), mCustomPlot->yAxis2, SLOT(setRange(QCPRange)));
}

QCustomPlotHelper::~QCustomPlotHelper()
{

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

        if (customPlot->property("enable-dataTracer").toBool())
        {
            //标记点和标签
            if(!mTracerData)
                mTracerData = new XCPItemTracer(mCustomPlot, XCPItemTracer::DataTracer);

            QColor fill = graph->pen().color();
            fill.setAlpha(255);
            mTracerData->setPen(graph->pen());
            mTracerData->setBrush(QBrush(fill));
            mTracerData->setTextPen(graph->pen());
            mTracerData->setVisible(true);
            mTracerData->updatePosition(axisRect, key, value);

            mCustomPlot->replot(QCustomPlot::rpQueuedReplot);//曲线重绘
        }

        emit itemActived(graph, key, value);
    }
}

void QCustomPlotHelper::mouseDoubleClick(QMouseEvent *event)
{
    QCustomPlot* customPlot = qobject_cast<QCustomPlot*>(sender());

    if (customPlot->property("enable-rangeSelect").toBool())
    {
        // 缩放到指定范围
        if (mDragStraightRect)
        {
            mTracerDragLeftLine->setVisible(false);
            mTracerDragRightLine->setVisible(false);
            mDragStraightRect->setVisible(false);

            QCPAxisRect *axisRect = mCustomPlot->axisRect();
            QList<QCPAxisRect*> axisRects = mCustomPlot->axisRects();
            for (auto iter : axisRects)
            {
                if (iter == axisRect)
                    continue;

                if (iter->rect().contains(event->pos()))
                {
                    axisRect = iter;
                    break;
                }
            }

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

        // 启用参考线或基准线
        if (customPlot->property("enable-straightLine").toBool())
        {

        }

        // 启用范围选取功能
        if (customPlot->property("enable-rangeSelect").toBool())
        {
            customPlot->setProperty("graph-draging", false);

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
        if (!customPlot->property("enable-rangeSelect").toBool())
            customPlot->setCursor(Qt::SizeAllCursor);
        else
            customPlot->setCursor(Qt::ArrowCursor);
    }
    // 右键
    else if (event->buttons() & Qt::RightButton)
    {
    }

    QCPAxisRect *axisRect = mCustomPlot->axisRect();
    QList<QCPAxisRect*> axisRects = mCustomPlot->axisRects();
    for (auto iter : axisRects)
    {
        if (iter == axisRect)
            continue;

        if (iter->rect().contains(event->pos()))
        {
            axisRect = iter;
            break;
        }
    }

    QCPAxis* yAxis = axisRect->axis(QCPAxis::AxisType::atLeft);
    QCPAxis* xAxis = axisRect->axis(QCPAxis::AxisType::atBottom);

    // 启用参考线或基准线
    if (customPlot->property("enable-straightLine").toBool())
    {
        //像素坐标转成实际的x,y轴的坐标
        float key = xAxis->pixelToCoord(event->pos().x());
        float value = yAxis->pixelToCoord(event->pos().y());

        if(Q_NULLPTR == mTracerX)
        {
            mTracerX = new XCPItemTracer(customPlot, XCPItemTracer::XAxisTracer);//x轴
            mTracerX->setTextVisible(false);//轴线上不显示值
        }
        mTracerX->updatePosition(axisRect, key, value);

        if(Q_NULLPTR == mTracerY)
        {
            mTracerY = new XCPItemTracer(customPlot, XCPItemTracer::YAxisTracer);//y轴
            mTracerY->setTextVisible(false);//轴线上不显示值
        }
        mTracerY->updatePosition(axisRect, key, value);

        int nTracerCount = mDataTracers.count();
        int nGraphCount = customPlot->graphCount(axisRect);
        if(nTracerCount < nGraphCount)
        {
            // 根据线条的个数，调整可显示数据点的个数
            for(int i = nTracerCount; i < nGraphCount; ++i)
            {
                XCPItemTracer *tracer = new XCPItemTracer(customPlot, XCPItemTracer::DataTracer);
                tracer->setOffset(QPoint(0, -20));//加个偏移量，让值显示在右上角，免得被横线遮挡
                mDataTracers.append(tracer);
            }
        }

        else if(nTracerCount > nGraphCount)
        {
            // 将超出线条个数的数据点隐藏起来
            for(int i = nGraphCount; i < nTracerCount; ++i)
            {
                XCPItemTracer *tracer = mDataTracers[i];
                if(tracer)
                {
                    tracer->setTextVisible(false);
                }
            }
        }

        for (int i = 0; i < nGraphCount; ++i)
        {
            XCPItemTracer *tracer = mDataTracers[i];
            if(tracer)
            {
                QColor fill = customPlot->graph(axisRect, i)->pen().color();
                fill.setAlpha(255);
                tracer->setPen(customPlot->graph(axisRect, i)->pen());
                tracer->setBrush(QBrush(fill));
                tracer->setTextPen(customPlot->graph(axisRect, i)->pen());
                tracer->setTextVisible(true);
                tracer->updatePosition(axisRect, key, value);
            }
        }

        if(Q_NULLPTR == mTracerCrossLine)
            mTracerCrossLine = new XCPItemTracerLine(customPlot, XCPItemTracerLine::Both);//直线
        mTracerCrossLine->updatePosition(axisRect, key, value);

        customPlot->replot(QCustomPlot::rpQueuedReplot);//曲线重绘
    }

    // 启用范围选取功能
    if (customPlot->property("enable-rangeSelect").toBool())
    {
        if (customPlot->property("left-clicked").toBool()){
            if (!customPlot->property("graph-draging").toBool())
            {
                // 设置鼠标拖拽开始
                customPlot->setProperty("graph-draging", true);

                // 记录拖拽起始点值
                double key = xAxis->pixelToCoord(event->pos().x());

                if(Q_NULLPTR == mTracerDragLeftLine)
                    mTracerDragLeftLine = new XCPItemTracerLine(customPlot, XCPItemTracerLine::Both);
                mTracerDragLeftLine->updatePosition(axisRect, key, yAxis->range().lower);
                mTracerDragLeftLine->setProperty("key", key);
                customPlot->setCursor(Qt::SplitHCursor);
                customPlot->replot(QCustomPlot::rpQueuedReplot);
            }
            else
            {
                double key = xAxis->pixelToCoord(event->pos().x());
                if(Q_NULLPTR == mTracerDragRightLine)
                    mTracerDragRightLine = new XCPItemTracerLine(customPlot, XCPItemTracerLine::Both);//直线
                mTracerDragRightLine->updatePosition(axisRect, key, yAxis->range().lower);
                mTracerDragRightLine->setProperty("key", key);

                //框选完毕，将选中的点颜色更新
                double key_from = mTracerDragLeftLine->property("key").toDouble();
                double key_to = mTracerDragRightLine->property("key").toDouble();
                double key_temp = key_from;
                key_from = qMin(key_from, key_to);
                key_to = qMax(key_temp, key_to);

                if(Q_NULLPTR == mDragStraightRect)
                    mDragStraightRect = new XCPItemStraightRect(customPlot);//蒙版
                mDragStraightRect->setRange(axisRect, QCPRange(key_from, key_to));
                mDragStraightRect->setVisible(true);

                for (int i = 0; i < mCustomPlot->graphCount(axisRect); ++i)
                {
                    QCPGraph *graph = mCustomPlot->graph(axisRect, i);
                    QVector<double> keys, values;
                    QVector<QColor> colors;
                    for (int i=0; i<graph->data()->size(); ++i){
                        if ((qFuzzyCompare(graph->data()->at(i)->key, key_from) || graph->data()->at(i)->key>=key_from) && (qFuzzyCompare(graph->data()->at(i)->key,key_to) || graph->data()->at(i)->key<=key_to)){// && graph->data()->at(i)->value>=value_to) {
                            keys << (double)graph->data()->at(i)->key;
                            values << (double)graph->data()->at(i)->value;
                            colors << graph->scatterStyle().pen().color().darker();
                        } else {
                            keys << (double)graph->data()->at(i)->key;
                            values << (double)graph->data()->at(i)->value;
                            colors << graph->scatterStyle().pen().color();
                        }
                    }

                    graph->setData(keys, values, colors);
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

#include <QToolTip>
void QCustomPlotHelper::mouseRelease(QMouseEvent * event)
{
    QCustomPlot* customPlot = qobject_cast<QCustomPlot*>(sender());

    // 左键
    if (event->button() == Qt::LeftButton)
    {
        if (customPlot->property("enable-rangeSelect").toBool())
        {
            QCPAxisRect *axisRect = mCustomPlot->axisRect();
            QList<QCPAxisRect*> axisRects = mCustomPlot->axisRects();
            for (auto iter : axisRects)
            {
                if (iter == axisRect)
                    continue;

                if (iter->rect().contains(event->pos()))
                {
                    axisRect = iter;
                    break;
                }
            }

            if (mDragStraightRect)
                emit selectRangeChanged(axisRect, mDragStraightRect->range());
        }
    }
    // 右键
    else if (event->button() == Qt::RightButton)
    {
        QCPAxisRect *axisRect = mCustomPlot->axisRect();
        QList<QCPAxisRect*> axisRects = mCustomPlot->axisRects();
        for (auto iter : axisRects)
        {
            if (iter == axisRect)
                continue;

            if (iter->rect().contains(event->pos()))
            {
                axisRect = iter;
                break;
            }
        }

        // 检查是否允许弹出右键菜单
        bool allow = true;
        if (onContextMenu)
            onContextMenu(axisRect, allow);
        if (!allow)
            return;

        QMenu contextMenu;
        QMenu *subMenu = contextMenu.addMenu(tr("适应模式"));
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

        contextMenu.addSeparator();
        if (mResetActionVisible)
        {
            contextMenu.addAction(actResetView);
            contextMenu.addSeparator();
        }
        if (mClearMarkerActionVisible)
            contextMenu.addAction(actClearMarker);
        if (mStraightLineActionVisible)
            contextMenu.addAction(actEnableStraightLine);
        if (mRangeSelectActionVisible)
            contextMenu.addAction(actEnableRangeSelect);
        if (mClearMarkerActionVisible || mStraightLineActionVisible || mRangeSelectActionVisible)
            contextMenu.addSeparator();
        contextMenu.addAction(actEnableDataTracer);
        contextMenu.addAction(actLinearScale);
        contextMenu.addAction(actLogarithmicScale);
        contextMenu.addSeparator();
        contextMenu.addAction(actExportGraphic);
        contextMenu.addAction(actExportData);
        contextMenu.exec(QCursor::pos());
        connect(&contextMenu, &QMenu::destroyed, [&](){
            container->deleteLater();
        });
    }

    if (customPlot->property("enable-straightLine").toBool())
    {

    }

    if (customPlot->property("enable-rangeSelect").toBool()
        && customPlot->property("left-clicked").toBool()
        && customPlot->property("graph-draging").toBool())
    {
        QToolTip::showText(event->globalPos(), tr("双击可缩放至可视范围。"), customPlot);
    }

    customPlot->setCursor(Qt::ArrowCursor);
    customPlot->setInteraction(QCP::iRangeDrag, true);
    customPlot->setProperty("left-clicked", false);
}

#include "qxtcheckcombobox.h"
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
    comboBox->setLayoutDirection(Qt::LeftToRight);

    for (int i=0; i<graphCount; ++i){
        comboBox->addItem(customPlot->graph(axisRect, i)->name());
    }
    for(int i=0; i<comboBox->count(); i++){
        comboBox->setItemData(i, Qt::Checked, Qt::CheckStateRole);
        QIcon actionIcon = roundPixmap(QSize(16,16), customPlot->graph(axisRect, i)->pen().color());
        comboBox->setItemIcon(i, actionIcon);
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

void QCustomPlotHelper::afterLayout()
{
    QCustomPlot* customPlot = qobject_cast<QCustomPlot*>(sender());
    QList<QCPAxisRect*> axisRects = customPlot->axisRects();
    QFontMetrics fontMetrics(customPlot->font());
    int avg_height = fontMetrics.ascent() + fontMetrics.descent();

    foreach (QCPAxisRect *axisRect, axisRects)
    {
        if (mCbbAxisRect.contains(axisRect->objectName())){
            QList<QComboBox*> comboBoxs = mCbbAxisRect[axisRect->objectName()];
            // 布局改变之前重新设定位置
            {
                int i = 0;
                for (auto comboBox : comboBoxs){
                    int h = 20;//checkBox->height();
                    if (comboBox->layoutDirection() == Qt::LeftToRight)
                        comboBox->move(axisRect->left() + 10, axisRect->topRight().y() + i++ * h + 5);// 显示左上角
                    else
                        comboBox->move(axisRect->right() - comboBox->width() - 10, axisRect->topRight().y() + i++ * h + 5);// 显示右上角
                }
            }
        }
    }

    if (mCustomPlot->property("enable-rangeSelect").toBool())
    {
        if (mDragStraightRect)
        {
            mDragStraightRect->refresh();
        }
    }
}

void QCustomPlotHelper::enableStraightLine(bool enable)
{
    if (mCustomPlot->property("enable-straightLine").toBool())
        enable = false;
    else
        enable = true;
    mCustomPlot->setProperty("enable-straightLine", enable);
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

void QCustomPlotHelper::enableDataTracer(bool enable/* = true*/)
{
    if (mCustomPlot->property("enable-dataTracer").toBool())
        enable = false;
    else
        enable = true;
    mCustomPlot->setProperty("enable-dataTracer", enable);
    actEnableDataTracer->setIcon(enable ? mIconChecked : mIconUnchecked);

    {
        //mTracerData->setVisible(false);
        //mCustomPlot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void QCustomPlotHelper::clearMarker()
{
    for (auto tracer : mDataTracers)
        tracer->setVisible(false);
    if (mTracerData)
        mTracerData->setVisible(false);
    mCustomPlot->replot(QCustomPlot::rpQueuedReplot);
}

void QCustomPlotHelper::enableRangeSelect(bool enable)
{
    if (mCustomPlot->property("enable-rangeSelect").toBool())
        enable = false;
    else
        enable = true;
    mCustomPlot->setProperty("enable-rangeSelect", enable);
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
            QVector<double> keys, values;
            QVector<QColor> colors;
            for (int i=0; i<graph->data()->size(); ++i){
                keys << (double)graph->data()->at(i)->key;
                values << (double)graph->data()->at(i)->value;
                colors << graph->scatterStyle().pen().color();
            }

            graph->setData(keys, values, colors);
            mCustomPlot->replot(QCustomPlot::rpQueuedReplot);
        }
    }
}

void QCustomPlotHelper::resetView()
{
    mCustomPlot->rescaleAxes(true);
    mCustomPlot->replot(QCustomPlot::rpQueuedReplot);
}

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringConverter>
#endif

namespace {
QString csvField(const QString &text)
{
    if (text.contains(',') || text.contains('"') || text.contains('\n'))
        return '"' + QString(text).replace('"', "\"\"") + '"';
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

    struct GraphInfo {
        QString name;
        QVector<double> keys;
        QVector<double> values;
    };
    QList<GraphInfo> graphs;
    int maxRows = 0;

    for (int i = 0; i < mCustomPlot->graphCount(); ++i) {
        QCPGraph *graph = mCustomPlot->graph(i);
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
    if (mCustomPlot->property("enable-linearScale").toBool())
        enable = false;
    else
        enable = true;

    mCustomPlot->setProperty("enable-linearScale", enable);
    mCustomPlot->setProperty("enable-logarithmicScale", !enable);
    actLinearScale->setIcon(mIconChecked);
    actLogarithmicScale->setIcon(mIconUnchecked);

    QCPAxisRect *axisRect = mCustomPlot->axisRect();
    QCPAxis* yAxis = axisRect->axis(QCPAxis::AxisType::atLeft);
    QCPAxis* xAxis = axisRect->axis(QCPAxis::AxisType::atBottom);

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
    if (mCustomPlot->property("enable-logarithmicScale").toBool())
        enable = false;
    else
        enable = true;
    mCustomPlot->setProperty("enable-logarithmicScale", enable);
    mCustomPlot->setProperty("enable-linearScale", !enable);
    actLogarithmicScale->setIcon(mIconChecked);
    actLinearScale->setIcon(mIconUnchecked);

    QCPAxisRect *axisRect = mCustomPlot->axisRect();
    QCPAxis* yAxis = axisRect->axis(QCPAxis::AxisType::atLeft);
    QCPAxis* xAxis = axisRect->axis(QCPAxis::AxisType::atBottom);

    QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
    yAxis->setTicker(logTicker);

    yAxis->setScaleType(QCPAxis::ScaleType::stLogarithmic);
    yAxis->setNumberFormat("eb");//使用科学计数法表示刻度
    yAxis->setNumberPrecision(0);//小数点后面小数位数

    yAxis->rescale(true);
    mCustomPlot->replot();
}

bool QCustomPlotHelper::eventFilter(QObject *watched, QEvent *event)
{
    if (watched != this && watched->inherits("QCustomPlot"))
    {
        // 鼠标单击
        if (event->type() == QEvent::MouseButtonPress)
            mousePress(reinterpret_cast<QMouseEvent*>(event));

        // 鼠标移动
        else if (event->type() == QEvent::MouseMove)
            mouseMove(reinterpret_cast<QMouseEvent*>(event));

        // 松开鼠标
        else if (event->type() == QEvent::MouseButtonRelease)
            mouseRelease(reinterpret_cast<QMouseEvent*>(event));
    }

    return QObject::eventFilter(watched, event);
}
