#ifndef QCUSTOMPLOTHELPER_H
#define QCUSTOMPLOTHELPER_H

#include <QObject>
#include "qcustomplot.h"
#include "QCPWaveformStatsPanel.h"
class XCPValueMarker : public QObject
{
    Q_OBJECT

public:
    enum Anchor
    {
        AtXAxis,//依附在x轴上显示x值
        AtYAxis,//依附在y轴上显示y值
        AtDataPoint//在图中显示x,y值
    };

    explicit XCPValueMarker(QCustomPlot *customPlot, Anchor anchor, QObject *parent = Q_NULLPTR);
    ~XCPValueMarker();

    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setText(const QString &text);
    void setTextPen(const QPen &pen);
    void updatePosition(QCPAxisRect *axisRect, double key, double value);
    void setVisible(bool visible);
    void setPointVisible(bool visible);
    void setLabelVisible(bool visible);
    void setOffset(QPoint offset);

private:
    bool mPointVisible = true;//点是否可见
    bool mLabelVisible = true;//标签是否可见
    QPoint mOffset;
    QCustomPlot *mCustomPlot = Q_NULLPTR;//图表
    Anchor mAnchor;//锚定位置
    QCPItemTracer *mPointItem = Q_NULLPTR;//数据点
    QCPItemText *mLabelItem = Q_NULLPTR;//数值标签
    QCPItemLine *mLeadLine = Q_NULLPTR;//引线
};

class XCPReferenceLine : public QObject
{
public:
    enum Orientation
    {
        Vertical,//垂直线
        Horizontal, //水平线
        Crosshair//同时显示水平和垂直线
    };

    explicit XCPReferenceLine(QCustomPlot *customPlot, Orientation orientation = Vertical, QObject *parent = Q_NULLPTR);
    ~XCPReferenceLine();

    void updatePosition(QCPAxisRect *axisRect, double key, double value);
    void setVisible(bool visible);

private:
    void initLine();
    QCustomPlot *mCustomPlot = Q_NULLPTR;//图表
    Orientation mOrientation;//方向
    QCPItemStraightLine *mVerticalLine = Q_NULLPTR; //垂直参考线
    QCPItemStraightLine *mHorizontalLine = Q_NULLPTR; //水平参考线
};

class XCPRangeMask : public QObject
{
public:
    explicit XCPRangeMask(QCustomPlot *customPlot, QObject *parent = Q_NULLPTR);
    ~XCPRangeMask();

    void setRange(QCPAxisRect *axisRect, const QCPRange& range);//更新范围
    QCPRange range() const;//获取范围
    void setVisible(bool visible);
    void setBrush(const QBrush &brush);
    void refresh();

private:
    QCPRange mRange;
    QCustomPlot *mCustomPlot = Q_NULLPTR;//图表
    QCPItemRect *mMaskRect = Q_NULLPTR; //蒙版矩形
};


class QCustomPlotHelper : public QObject
{
    Q_OBJECT
public:
    explicit QCustomPlotHelper(QCustomPlot* customPlot, QObject *parent=nullptr);
    virtual ~QCustomPlotHelper();

    // 扩展简化QCustomPlot本身接口
    void setLabel(const QString &strX, const QString &strY);
    QCPGraph *addLineGraph(QCPAxis *keyAxis, QCPAxis *valueAxis);
    QCPGraph *addScatterGraph(QCPAxis *keyAxis, QCPAxis *valueAxis);
    QCPGraph *addLineScatterGraph(QCPAxis *keyAxis, QCPAxis *valueAxis);
    QCPGraph *addColorMapGraph(QCPAxis *keyAxis, QCPAxis *valueAxis);

    void setResetActionVisible(bool visible = true);
    void setClearMarkerActionVisible(bool visible = true);
    void setStraightLineActionVisible(bool visible = true);
    void setRangeSelectActionVisible(bool visible = true);

    // 设置波形详情悬浮窗的停靠位置（坐标轴左上角/左下角/右上角/右下角）
    void setStatsPanelDockPosition(QCPWaveformStatsPanel::DockPosition pos);

    /**
    * @function name: setGraphCheckBoxList
    * @brief 设置下拉列表式复选框
    * @param[in]        customPlot
    * @param[out]       none
    * @return           void
    */
    void setGraphCheckBoxList(QCustomPlot* customPlot, const QCPAxisRect *axisRect = nullptr);

    /**
    * @function name: enableStraightLine
    * @brief 启用参考线或基准线
    * @param[in]        none
    * @param[out]       none
    * @return           void
    */
    Q_SLOT void enableStraightLine(bool enable = true);

    /**
     * @function name: enablePlotSeriesDataPicker/enablePlotSeriesDataMarker
     * @brief 启用数据点标记/数据拾取
     */
    Q_SLOT void enableAtDataPoint(bool enable = true);

    /**
    * @function name: clearMarker
    * @brief 清楚标记
    * @param[in]        none
    * @param[out]       none
    * @return           void
    */
    Q_SLOT void clearMarker();

    /**
    * @function name: enableRangeSelect
    * @brief 启用范围选取功能
    * @param[in]        none
    * @param[out]       none
    * @return           void
    */
    Q_SLOT void enableRangeSelect(bool enable = true);

    /**
    * @function name: showWaveformDetail
    * @brief 显示波形详情
    * @param[in]        none
    * @param[out]       none
    * @return           void
    */
    Q_SLOT void showWaveformDetail();

    /**
    * @function name: resetView
    * @brief 恢复视图
    * @param[in]        none
    * @param[out]       none
    * @return           void
    */
    Q_SLOT void resetView();

    /**
    * @function name: exportGraphic
    * @brief 导出图像
    * @param[in]        none
    * @param[out]       none
    * @return           void
    * @author           imalilac
    * @date             2025-12-25
    */
    Q_SLOT void exportGraphic();

    /**
    * @function name: exportData
    * @brief 导出曲线数据为 CSV
    * @param[in]        none
    * @param[out]       none
    * @return           void
    */
    Q_SLOT void exportData();

    /**
    * @function name: linearScale
    * @brief 线性缩放
    * @param[in]        none
    * @param[out]       none
    * @return           void
    * @author           imalilac
    * @date             2025-12-25
    */
    Q_SLOT void linearScale(bool enable = true);

    /**
    * @function name: logarithmicScale
    * @brief 对数缩放
    * @param[in]        none
    * @param[out]       none
    * @return           void
    * @author           imalilac
    * @date             2025-12-25
    */
    Q_SLOT void logarithmicScale(bool enable = false);

    Q_SLOT void plottableClick(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);
    Q_SLOT void mouseDoubleClick(QMouseEvent *event);
    Q_SLOT void mousePress(QMouseEvent * event);
    Q_SLOT void mouseMove(QMouseEvent * event);
    Q_SLOT void mouseRelease(QMouseEvent * event);
    Q_SLOT void beforeReplot();
    Q_SLOT void afterLayout();

    Q_SIGNAL void itemActived(QCPGraph *graph, double key, double value);//节点被鼠标点击激活
    Q_SIGNAL void autoScaleChanged(bool);
    Q_SIGNAL void fixedScaleChanged(bool);
    Q_SIGNAL void manualScaleChanged(bool);
    Q_SIGNAL void selectRangeChanged(const QCPAxisRect *axisRect, const QCPRange& range);

protected:
    QCPAxisRect *axisRectAt(const QPoint &pos) const;
    void updateWaveformDetail();

    // 波形统计缓存（避免每次 replot 重复计算）
    QCPGraph *mStatsGraph = nullptr;
    int mStatsDataCount = 0;
    double mStatsFirstKey = 0;
    double mStatsLastKey = 0;
    QCPRange mStatsXRange;
    bool mStatsValid = false;

private:
    QCustomPlot* mCustomPlot = Q_NULLPTR;
    QMenu contextMenu;

    // 参考线
    XCPValueMarker *mTracerX = Q_NULLPTR;// x轴
    XCPValueMarker *mTracerY = Q_NULLPTR;// y轴
    XCPReferenceLine  *mTracerCrossLine = Q_NULLPTR;// 十字线
    QList<XCPValueMarker *> mDataTracers;// 数据点集信息

    // 点值（支持多个点位标记）
    struct DataMarker
    {
        XCPValueMarker *tracer = Q_NULLPTR;
        QCPGraph *graph = Q_NULLPTR;
        double key = 0;
        double value = 0;
    };
    QList<DataMarker> mDataMarkers;// 选中的数据点标记集合
    void moveActiveMarker(int step);
    QAction *actPrevPoint = Q_NULLPTR;
    QAction *actNextPoint = Q_NULLPTR;

    // 鼠标框选
    XCPReferenceLine *mTracerDragLeftLine = Q_NULLPTR;
    XCPReferenceLine *mTracerDragRightLine = Q_NULLPTR;
    XCPRangeMask* mDragStraightRect = Q_NULLPTR;

    QIcon mIconUnchecked, mIconChecked;
    QAction *actResetView;// 恢复试图
    QAction *actAutoScale;// 自适应模式
    quint32 mXAxisFixedRange = 300;//最近范围最值
    QAction *actClearMarker;// 清除标签
    QAction *actExportGraphic;// 导出图像
    QAction *actExportData;// 导出数据
    QAction *actEnableStraightLine;// 启用参考线或基准线
    QAction *actEnableAtDataPoint;// 启用数据点标记
    QAction *actEnableRangeSelect;// 启用范围选取功能
    QAction *actShowWaveformDetail;// 显示波形详情
    QAction *actLinearScale;// 线性缩放
    QAction *actLogarithmicScale;// 对数缩放

    QCPWaveformStatsPanel* mStatsPanel;

    bool mResetActionVisible = true;
    bool mClearMarkerActionVisible = true;
    bool mStraightLineActionVisible = true;
    bool mRangeSelectActionVisible = true;

    QMap<QString, QList<QComboBox*>> mCbbAxisRect;

    /**
    * @function name: 生成一个圆形的图标
    * @brief
    * @param[in]        size    图标尺寸
    * @param[in]        clrIn    内圆颜色
    * @param[in]        clrOut    外圆颜色
    * @param[out]       none
    * @return           void
    */
    static QPixmap roundPixmap(QSize sz, QColor clrOut);//单圆
    static QPixmap dblroundPixmap(QSize sz, QColor clrIn, QColor clrOut);//双圆

    void setupMenu();

public:
    std::function<void(const QCPAxisRect*, bool&)> onContextMenu;
};

#endif // QCUSTOMPLOTHELPER_H