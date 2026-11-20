#ifndef QCPOVERLAYWIDGET_H
#define QCPOVERLAYWIDGET_H

#include <QObject>
#include <QPainter>
#include <QFontMetrics>
#include "qcustomplot.h"

class QCPOverlayWidget : public QCPLayerable
{
    Q_OBJECT
public:
    // 悬浮窗在坐标轴矩形内的停靠位置
    enum DockPosition {
        DockTopLeft,     // 左上角
        DockTopRight,    // 右上角
        DockBottomLeft,  // 左下角
        DockBottomRight, // 右下角
    };
    Q_ENUM(DockPosition)

    explicit QCPOverlayWidget(QCustomPlot* parentPlot)
        : QCPLayerable(parentPlot, QStringLiteral("legend"))
    {
        recalcAutoSize();
    }

    // 设置悬浮窗停靠位置（相对于坐标轴矩形）
    void setDockPosition(DockPosition pos) {
        m_dockPos = pos;
    }
    DockPosition dockPosition() const { return m_dockPos; }

    // 设置背景透明度 0~255
    void setBackgroundAlpha(int alpha) {
        m_bgAlpha = qBound(0, alpha, 255);
    }

    // 添加普通文本信息
    void setText(const QString& name, const QString& text) {
        m_texts.append(qMakePair(name, text));
        recalcAutoSize();
    }

    // 清空所有内容，自动收缩回最小尺寸
    void clearAll() {
        m_texts.clear();
        recalcAutoSize();
    }

    virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const Q_DECL_OVERRIDE { Q_UNUSED(painter) }
//    virtual double selectTest(const QPointF &/*pos*/, bool /*onlySelectable*/, QVariant *details=nullptr) const Q_DECL_OVERRIDE { Q_UNUSED(details) }

protected:
    void draw(QCPPainter* painter) override {
        if (m_texts.isEmpty()) return;
        const int SPACING = 20;

        QRect absRect = QRect(calcAbsPos(), size());
        painter->setRenderHint(QPainter::Antialiasing);

        // 绘制半透明悬浮窗背景
        painter->setBrush(QBrush(QColor(255, 255, 255, m_bgAlpha)));
        painter->setPen(QPen(QColor(120, 120, 120, 180), 1));
        painter->drawRoundedRect(absRect, 3, 3);

        // 绘制所有文本信息
        painter->setPen(Qt::black);
        painter->setFont(QFont("微软雅黑", 9));
        QFontMetrics fm(QFont("微软雅黑", 9));
        int textX = absRect.x() + 10;

        for (const auto& pair : m_texts) {
            auto text = pair.first + " " + pair.second;
            painter->drawText(QRectF(textX, absRect.y(), fm.horizontalAdvance(text), m_autoHeight), Qt::AlignLeft | Qt::AlignVCenter, text);
            textX += fm.horizontalAdvance(text) + SPACING;
        }
    }

private:
    // 自动计算适配内容的宽高
    void recalcAutoSize() {
        const int MIN_PANEL_WIDTH = 100;
        const int PADDING = 20;
        const int TEXT_HEIGHT = 20;
        const int SPACING = 20;

        // 计算所有文本最大宽度
        QFontMetrics fm(QFont("微软雅黑", 9));
        // 上下布局
        if(0){
            int maxTextWidth = 0;
            for (const auto& pair : m_texts) {
                auto text = pair.first + " " + pair.second;
                maxTextWidth = qMax(maxTextWidth, fm.horizontalAdvance(text));
            }

            // 自动计算最终宽度
            m_autoWidth = qMax(MIN_PANEL_WIDTH, maxTextWidth + PADDING*2);
            // 自动计算最终高度
            m_autoHeight = 16 // 顶部边距
                           + m_texts.size() * TEXT_HEIGHT
                           + (m_texts.size() > 0 ? 8 : 0) // 文本和按钮间距
                           + PADDING; // 底部边距
        }

        // 左右布局
        {
            int textX = 10;
            int textY = qMax(10, fm.height());
            int maxTextWidth = 0;
            for (const auto& pair : m_texts) {
                auto text = pair.first + " " + pair.second;
                textX += fm.horizontalAdvance(text) + SPACING;
            }
            maxTextWidth = qMax(maxTextWidth, textX);

            // 自动计算最终宽度
            m_autoWidth = qMax(MIN_PANEL_WIDTH, maxTextWidth);
            // 自动计算最终高度
            m_autoHeight = textY + 6; // 底部边距
        }
    }

    // 计算悬浮窗在坐标轴矩形上的绝对像素坐标
    QPoint calcAbsPos() {
        const int MARGIN = 10;
        QRect axisRect = parentPlot()->axisRect()->rect();
        int px, py;
        switch (m_dockPos) {
        case DockTopLeft:
            px = axisRect.left() + MARGIN;
            py = axisRect.top() + MARGIN;
            break;
        case DockTopRight:
            px = axisRect.right() - m_autoWidth - MARGIN;
            py = axisRect.top() + MARGIN;
            break;
        case DockBottomLeft:
            px = axisRect.left() + MARGIN;
            py = axisRect.bottom() - m_autoHeight - MARGIN;
            break;
        case DockBottomRight:
        default:
            px = axisRect.right() - m_autoWidth - MARGIN;
            py = axisRect.bottom() - m_autoHeight - MARGIN;
            break;
        }
        return QPoint(px, py);
    }

    // 直接返回自适应后的面板尺寸
    QSize size() {
        return QSize(m_autoWidth, m_autoHeight);
    }

    DockPosition m_dockPos = DockBottomRight;
    int m_autoWidth = 200;
    int m_autoHeight = 50;
    int m_bgAlpha = 220;
    QList<QPair<QString, QString>> m_texts;
};

#endif // QCPOVERLAYWIDGET_H

