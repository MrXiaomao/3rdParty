#ifndef QCUSTOMPLOTHELPER_H
#define QCUSTOMPLOTHELPER_H

#include <QObject>
#include "qcustomplot.h"

class XCPItemTracer : public QObject
{
    Q_OBJECT

public:
    enum TracerType
    {
        XAxisTracer,//依附在x轴上显示x值
        YAxisTracer,//依附在y轴上显示y值
        DataTracer//在图中显示x,y值
    };

    explicit XCPItemTracer(QCustomPlot *customPlot, TracerType _type, QObject *parent = Q_NULLPTR);
    ~XCPItemTracer();

    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setText(const QString &text);
    void setTextPen(const QPen &pen);
    void updatePosition(double xValue, double yValue);
    void setVisible(bool visible);
    void setTracerVisible(bool visible);
    void setTextVisible(bool visible);
    void setOffset(QPoint offset);

protected:
    bool mTracerVisible = true;//是否可见
    bool mTextVisible = true;//是否可见
    QPoint mOffset;
    QCustomPlot *mCustomPlot = Q_NULLPTR;//图表
    TracerType mTracerType;//类型
    QCPItemTracer *mTracer = Q_NULLPTR;//跟踪的点
    QCPItemText *mTracerText = Q_NULLPTR;//显示的数值
    QCPItemLine *mTracerArrow = Q_NULLPTR;//箭头
};

class XCPItemTracerLine : public QObject
{
public:
    enum LineType
    {
        VerticalLine,//垂直线
        HorizonLine, //水平线
        Both//同时显示水平和垂直线
    };

    explicit XCPItemTracerLine(QCustomPlot *customPlot, LineType _type = VerticalLine, QObject *parent = Q_NULLPTR);
    ~XCPItemTracerLine();

    void initLine();
    void updatePosition(double xValue, double yValue);
    void setVisible(bool visible);

protected:
    bool mVisible = true;//是否可见
    QCustomPlot *mCustomPlot = Q_NULLPTR;//图表
    LineType mLineType;//类型
    QCPItemStraightLine *mStraightLineV = Q_NULLPTR; //垂直线
    QCPItemStraightLine *mStraightLineH = Q_NULLPTR; //水平线
};

class XCPItemStraightRect : public QObject
{
public:
    enum StraightType
    {
        VerticalStraight,//垂直线
        HorizonStraight, //水平线
    };

    explicit XCPItemStraightRect(QCustomPlot *customPlot, StraightType _type = VerticalStraight, QObject *parent = Q_NULLPTR);
    ~XCPItemStraightRect();

    void setRange(const QCPRange& range);//更新范围
    QCPRange range() const;//更新范围
    void setVisible(bool visible);
    void setBrush(const QBrush &brush);
    void refresh();

protected:
    bool mVisible = true;//是否可见
    QCPRange mRange;
    QCustomPlot *mCustomPlot = Q_NULLPTR;//图表
    StraightType mStraightType;//类型
    QCPItemRect *mMaskRect = Q_NULLPTR; //蒙板矩形
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

    /**
    * @function name: setGraphCheckBox
    * @brief 设置graph可选项
    * @param[in]        customPlot
    * @param[out]       none
    * @return           void
    */
    static void setGraphCheckBox(QCustomPlot* customPlot);

    /**
    * @function name: enableStraightLine
    * @brief 启用参考线或基准线
    * @param[in]        none
    * @param[out]       none
    * @return           void
    */
    Q_SLOT void enableStraightLine(bool enable = true);

    /**
    * @function name: enableRangeSelect
    * @brief 启用范围选取功能
    * @param[in]        none
    * @param[out]       none
    * @return           void
    */
    Q_SLOT void enableRangeSelect(bool enable = true);

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
    Q_SLOT void afterLayout();

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QCustomPlot* mCustomPlot = Q_NULLPTR;

    // 参考线
    XCPItemTracer *mTracerX = Q_NULLPTR;// x轴
    XCPItemTracer *mTracerY = Q_NULLPTR;// y轴
    XCPItemTracerLine  *mTracerCrossLine = Q_NULLPTR;// 十字线
    QList<XCPItemTracer *> mDataTracers;// 数据点集信息

    // 点值
    XCPItemTracer *mTracerData = Q_NULLPTR;// 选中数据点信息

    // 鼠标框选
    XCPItemTracerLine *mTracerDragLeftLine = Q_NULLPTR;
    XCPItemTracerLine *mTracerDragRightLine = Q_NULLPTR;
    XCPItemStraightRect* mDragStraightRect = Q_NULLPTR;

    QIcon mIconUnchecked, mIconChecked;
    QAction *actResetView;// 恢复试图
    QAction *actExportGraphic;// 导出图像
    QAction *actEnableStraightLine;// 启用参考线或基准线
    QAction *actEnableRangeSelect;// 启用范围选取功能
    QAction *actLinearScale;// 线性缩放
    QAction *actLogarithmicScale;// 对数缩放

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
};

#endif // QCUSTOMPLOTHELPER_H
