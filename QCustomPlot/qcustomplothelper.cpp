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
        mTracer->setStyle(QCPItemTracer::tsCircle);
        mTracer->setPen(penDefault);
        mTracer->setBrush(brushDefault);

        mTracerText = new QCPItemText(mCustomPlot);
        mTracerText->setLayer("overlay");
        mTracerText->setClipToAxisRect(false);
        mTracerText->setPadding(QMargins(5, 5, 5, 5));
        mTracerText->setBrush(brushDefault);
        mTracerText->setPen(penDefault);
        mTracerText->position->setParentAnchor(mTracer->position);
        //        mTracerText->setFont(QFont("宋体", 8));
        mTracerText->setFont(QFont("Arial", 8));
        mTracerText->setColor(clrDefault);
        mTracerText->setText("");

        mTracerArrow = new QCPItemLine(mCustomPlot);
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
        mTracerText->setBrush(Qt::NoBrush);
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

void XCPItemTracer::updatePosition(double xValue, double yValue)
{
    if (!mTextVisible && !mTracerVisible)
        return;

    if (yValue > mCustomPlot->yAxis->range().upper)
        yValue = mCustomPlot->yAxis->range().upper;

    int xNumberPrecision = 3;//mCustomPlot->xAxis->numberPrecision();
    int yNumberPrecision = 0;//mCustomPlot->yAxis->numberPrecision();
    switch (mTracerType)
    {
    case XAxisTracer:
    {        
        mTracer->position->setCoords(xValue, 1);
        mTracerText->position->setCoords(0, 15);
        mTracerArrow->start->setCoords(0, 15);
        mTracerArrow->end->setCoords(0, 0);
        setText(QString::number(xValue, 'f', xNumberPrecision));
        break;
    }
    case YAxisTracer:
    {
        mTracer->position->setCoords(0, yValue);
        mTracerText->position->setCoords(-20, 0);
        //        mTracerArrow->start->setCoords(20, 0);
        //        mTracerArrow->end->setCoords(0, 0);
        setText(QString::number(yValue, 'f', yNumberPrecision));
        break;
    }
    case DataTracer:
    {
        mTracer->position->setCoords(xValue, yValue);
        mTracerText->position->setCoords(20 + mOffset.x(), mOffset.y());
        mTracerArrow->start->setCoords(20 + mOffset.x(), mOffset.y() == 0 ? 0 : (mOffset.y() / 2 + 2));
        setText(QString("x:%1,y:%2").arg(QString::number(xValue, 'f', xNumberPrecision)).arg(QString::number(yValue, 'f', yNumberPrecision)));
        break;
    }
    default:
        break;
    }
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

void XCPItemTracerLine::updatePosition(double xValue, double yValue)
{
    if(VerticalLine == mLineType || Both == mLineType)
    {
        if(mStraightLineV)
        {
            mStraightLineV->point1->setCoords(xValue, mCustomPlot->yAxis->range().lower);
            mStraightLineV->point2->setCoords(xValue, mCustomPlot->yAxis->range().upper);
        }
    }

    if(HorizonLine == mLineType || Both == mLineType)
    {
        if(mStraightLineH)
        {
            mStraightLineH->point1->setCoords(mCustomPlot->xAxis->range().lower, yValue);
            mStraightLineH->point2->setCoords(mCustomPlot->xAxis->range().upper, yValue);
        }
    }
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

void XCPItemStraightRect::setRange(const QCPRange& range)
{
    if(mMaskRect)
    {
        if (VerticalStraight == mStraightType)
        {
            mMaskRect->topLeft->setCoords(range.lower, mCustomPlot->yAxis->range().upper);
            mMaskRect->bottomRight->setCoords(range.upper, mCustomPlot->yAxis->range().lower);
        }
        else
        {
            mMaskRect->topLeft->setCoords(mCustomPlot->xAxis->range().lower, range.lower);
            mMaskRect->bottomRight->setCoords(mCustomPlot->xAxis->range().upper, range.upper);
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
    if(mMaskRect)
    {
        mMaskRect->topLeft->setCoords(mMaskRect->topLeft->key(), mCustomPlot->yAxis->range().upper);
        mMaskRect->bottomRight->setCoords(mMaskRect->bottomRight->key(), mCustomPlot->yAxis->range().lower);
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
    actExportGraphic = new QAction(tr("导出图像..."), this);
    connect(actExportGraphic, &QAction::triggered, this, &QCustomPlotHelper::exportGraphic);

    actLinearScale = new QAction(mIconChecked, tr("线性缩放"), this);
    connect(actLinearScale, &QAction::triggered, this, &QCustomPlotHelper::linearScale);
    actLogarithmicScale = new QAction(mIconUnchecked, tr("对数缩放"), this);
    connect(actLogarithmicScale, &QAction::triggered, this, &QCustomPlotHelper::logarithmicScale);

    actEnableStraightLine = new QAction(mIconUnchecked, tr("轴参考线"), this);
    connect(actEnableStraightLine, &QAction::triggered, this, &QCustomPlotHelper::enableStraightLine);
    actEnableRangeSelect = new QAction(mIconUnchecked, tr("选择范围"), this);
    connect(actEnableRangeSelect, &QAction::triggered, this, &QCustomPlotHelper::enableRangeSelect);

    customPlot->setAntialiasedElements(QCP::aeAll);//设置所有图形元素使用抗锯齿渲染
    customPlot->legend->setVisible(true);//设置图例可见
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);//设置图形可移动、缩放、选中
    customPlot->xAxis->setTickLabelRotation(-45);//x轴文字倾斜45°角，方便显示长文本
    customPlot->rescaleAxes(true);//设置试图自由缩放
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
    mCustomPlot->xAxis->setLabel(strX);
    mCustomPlot->yAxis->setLabel(strY);
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

QPixmap QCustomPlotHelper::roundPixmap(QSize sz, QColor clrOut)
{
    QPixmap result(sz);
    result.fill(Qt::transparent);

    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing,true);

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

void QCustomPlotHelper::plottableClick(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent */*event*/)
{
    QCPGraph *graph = qobject_cast<QCPGraph*>(plottable);
    if (graph) {
        double key = graph->dataMainKey(dataIndex); // 获取X轴的值
        double value = graph->dataMainValue(dataIndex); // 获取Y轴的值

        //标记点和标签
        if(!mTracerData)
            mTracerData = new XCPItemTracer(mCustomPlot, XCPItemTracer::DataTracer);

        mTracerData->setPen(graph->pen());
        mTracerData->setBrush(Qt::NoBrush);
        mTracerData->setTextPen(graph->pen());
        mTracerData->setTextVisible(true);
        mTracerData->updatePosition(key, value);

        mCustomPlot->replot(QCustomPlot::rpQueuedReplot);//曲线重绘
    }
}

void QCustomPlotHelper::mouseDoubleClick(QMouseEvent */*event*/)
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
            customPlot->xAxis->rescale(false);
            customPlot->xAxis->setRange(mDragStraightRect->range());
            customPlot->yAxis->rescale(true);
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

    // 启用参考线或基准线
    if (customPlot->property("enable-straightLine").toBool())
    {
        //像素坐标转成实际的x,y轴的坐标
        float key = customPlot->xAxis->pixelToCoord(event->pos().x());
        float value = customPlot->yAxis->pixelToCoord(event->pos().y());

        if(Q_NULLPTR == mTracerX)
        {
            mTracerX = new XCPItemTracer(customPlot, XCPItemTracer::XAxisTracer);//x轴
            mTracerX->setTextVisible(false);//轴线上不显示值
        }
        mTracerX->updatePosition(key, value);

        if(Q_NULLPTR == mTracerY)
        {
            mTracerY = new XCPItemTracer(customPlot, XCPItemTracer::YAxisTracer);//y轴
            mTracerY->setTextVisible(false);//轴线上不显示值
        }
        mTracerY->updatePosition(key, value);

        int nTracerCount = mDataTracers.count();
        int nGraphCount = customPlot->graphCount();
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
                tracer->setPen(customPlot->graph(i)->pen());
                tracer->setBrush(Qt::NoBrush);
                tracer->setTextPen(customPlot->graph(i)->pen());
                tracer->setTextVisible(true);
                tracer->updatePosition(key, value);
            }
        }

        if(Q_NULLPTR == mTracerCrossLine)
            mTracerCrossLine = new XCPItemTracerLine(customPlot, XCPItemTracerLine::Both);//直线
        mTracerCrossLine->updatePosition(key, value);

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
                double key = customPlot->xAxis->pixelToCoord(event->pos().x());

                if(Q_NULLPTR == mTracerDragLeftLine)
                    mTracerDragLeftLine = new XCPItemTracerLine(customPlot, XCPItemTracerLine::Both);
                mTracerDragLeftLine->updatePosition(key, customPlot->yAxis->range().lower);
                mTracerDragLeftLine->setProperty("key", key);
                customPlot->setCursor(Qt::SplitHCursor);
                customPlot->replot(QCustomPlot::rpQueuedReplot);
            }
            else
            {
                double key = customPlot->xAxis->pixelToCoord(event->pos().x());
                if(Q_NULLPTR == mTracerDragRightLine)
                    mTracerDragRightLine = new XCPItemTracerLine(customPlot, XCPItemTracerLine::Both);//直线
                mTracerDragRightLine->updatePosition(key, customPlot->yAxis->range().lower);
                mTracerDragRightLine->setProperty("key", key);

                //框选完毕，将选中的点颜色更新
                double key_from = mTracerDragLeftLine->property("key").toDouble();
                double key_to = mTracerDragRightLine->property("key").toDouble();
                double key_temp = key_from;
                key_from = qMin(key_from, key_to);
                key_to = qMax(key_temp, key_to);

                if(Q_NULLPTR == mDragStraightRect)
                    mDragStraightRect = new XCPItemStraightRect(customPlot);//蒙版
                mDragStraightRect->setRange(QCPRange(key_from, key_to));
                mDragStraightRect->setVisible(true);

                for (int i = 0; i < mCustomPlot->graphCount(); ++i)
                {
                    QCPGraph *graph = mCustomPlot->graph(i);
                    QColor ivertColor = graph->scatterStyle().pen().color();
                    ivertColor.setRgb(0xff - ivertColor.red(),
                                      0xff - ivertColor.green(),
                                      0xff - ivertColor.blue());

                    QVector<double> keys, values;
                    QVector<QColor> colors;
                    for (int i=0; i<graph->data()->size(); ++i){
                        if (graph->data()->at(i)->key>=key_from && graph->data()->at(i)->key<=key_to){// && graph->data()->at(i)->value>=value_to) {
                            keys << (double)graph->data()->at(i)->key;
                            values << (double)graph->data()->at(i)->value;
                            colors << ivertColor;
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

    }
    // 右键
    else if (event->button() == Qt::RightButton)
    {
        QMenu contextMenu(customPlot);
        contextMenu.addAction(actResetView);
        contextMenu.addSeparator();
        contextMenu.addAction(actEnableStraightLine);
        contextMenu.addAction(actEnableRangeSelect);
        contextMenu.addSeparator();
        contextMenu.addAction(actLinearScale);
        contextMenu.addAction(actLogarithmicScale);
        contextMenu.addSeparator();
        contextMenu.addAction(actExportGraphic);
        contextMenu.exec(QCursor::pos());
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

void QCustomPlotHelper::setGraphCheckBox(QCustomPlot* customPlot)
{
    // 隐藏图例
    customPlot->legend->setVisible(false);

    // 添加可选项
    for (int i=0; i<2; ++i){
        QCheckBox* checkBox = new QCheckBox(customPlot);
        checkBox->setText(customPlot->graph(i)->name());
        checkBox->setObjectName(tr(""));
        QIcon actionIcon = roundPixmap(QSize(16,16), customPlot->graph(i)->pen().color());
        checkBox->setIcon(actionIcon);
        checkBox->setChecked(true);
        connect(checkBox, &QCheckBox::stateChanged, [=](int state){
            QCPGraph *graph = customPlot->graph(i);
            if (graph){
                graph->setVisible(Qt::CheckState::Checked == state ? true : false);
                customPlot->replot(QCustomPlot::rpQueuedReplot);
            }
        });
    }
}

void QCustomPlotHelper::afterLayout()
{
    QCustomPlot* customPlot = qobject_cast<QCustomPlot*>(sender());

    // 布局改变之前重新设定位置
    {
        QList<QCheckBox*> checkBoxs = customPlot->findChildren<QCheckBox*>();
        QFontMetrics fontMetrics(customPlot->font());
        int avg_height = fontMetrics.ascent() + fontMetrics.descent();
        int i = 0;
        for (auto checkBox : checkBoxs){
            checkBox->move(customPlot->axisRect()->left() + 10, customPlot->axisRect()->topRight().y() + i++ * checkBox->height() + 5);
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

#include <QMessageBox>
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

    QSharedPointer<QCPAxisTicker> ticker(new QCPAxisTicker);
    mCustomPlot->yAxis->setTicker(ticker);

    mCustomPlot->yAxis->setScaleType(QCPAxis::ScaleType::stLinear);
    mCustomPlot->yAxis->setNumberFormat("f");
    mCustomPlot->yAxis->setNumberPrecision(0);

    mCustomPlot->yAxis->rescale(true);
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

    QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
    mCustomPlot->yAxis->setTicker(logTicker);

    mCustomPlot->yAxis->setScaleType(QCPAxis::ScaleType::stLogarithmic);
    mCustomPlot->yAxis->setNumberFormat("eb");//使用科学计数法表示刻度
    mCustomPlot->yAxis->setNumberPrecision(0);//小数点后面小数位数

    mCustomPlot->yAxis->rescale(true);
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
