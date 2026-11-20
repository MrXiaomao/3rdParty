#ifndef WAVEFORMSTATS_H
#define WAVEFORMSTATS_H

#include <QVector>
#include <QtMath>
#include "globalsettings.h"

class WaveformStats
{
public:
    struct Result {
        double max = 0.0;               // 最大值
        double min = 0.0;               // 最小值
        double peak2peak = 0.0;         // 峰峰值
        double mean = 0.0;              // 均值
        double absMean = 0.0;           // 绝对平均值
        double median = 0.0;            // 中位数
        double rms = 0.0;               // 均方根RMS
        double variance = 0.0;          // 方差
        double stdDev = 0.0;            // 标准差
        double crestFactor = 0.0;       // 峰值因子
        double formFactor = 0.0;        // 波形因子
        double skewness = 0.0;          // 偏度
        double kurtosis = 0.0;          // 峭度
        int zeroCrossCount = 0;         // 过零计数
        double baseline = 0.0;          // 新增：自动计算的波形基线

        // 新增：半高宽相关指标
        double fwhm = 0.0;              // 半高宽（半高全宽，浮点索引差）
        double fwhmArea = 0.0;          // 半高宽区间内的净面积（已减基线）
        double fwhmMeanLevel = 0.0;     // 新增：半高宽区间内的平均幅值
        double halfLevel = 0.0;         // 半高阈值电平（= 基线 + 峰高/2），便于调试/绘图标注
        int fwhmLeftIndex = -1;         // 左侧半高交点索引（插值所在左点）
        int fwhmRightIndex = -1;        // 右侧半高交点索引（插值所在左点）
        bool fwhmValid = false;         // 半高宽是否有效（两侧都成功找到交点）
    };

    // sampleInterval：相邻采样点的时间/物理间隔，默认 1.0（即索引差作为宽度）
    static Result calculate(const QVector<double>& data,
                            int baselineFrom = 0, /*基线采样起点*/
                            int stableLen = 200, /*基线采样长度*/
                            double sampleInterval = 1.0)
    {
        Result res;
        const int n = data.size();
        if (n == 0) return res;

        // ===== 自动计算基线逻辑（保持原实现不变）=====
        int baselineTo = baselineFrom + stableLen;
        double baselineSum = 0.0;
        int baselineCount = 0;
        for (int i = baselineFrom; i < baselineTo && i < data.size(); i++) {
            baselineSum += data[i];
            baselineCount++;
        }
        res.baseline = baselineSum / baselineCount;

        // ===== 原有基础极值与累加计算 =====
        double sum = 0.0;
        double absSum = 0.0;
        double squareSum = 0.0;
        res.max = data.first();
        res.min = data.first();
        int maxIndex = 0;
        for (int i = 0; i < n; i++) {
            double val = data[i];
            sum += val;
            absSum += qAbs(val);
            squareSum += val * val;
            if (val > res.max) { res.max = val; maxIndex = i; }
            if (val < res.min) res.min = val;
        }

        // ===== 原有一阶基础指标 =====
        res.peak2peak = res.max - res.min;
        res.mean = sum / n;
        res.absMean = absSum / n;
        res.rms = qSqrt(squareSum / n);

        // ===== 原有方差、标准差 =====
        double devSquareSum = 0.0;
        double devCubeSum = 0.0;
        double devFourthSum = 0.0;
        for (double val : data) {
            double dev = val - res.mean;
            devSquareSum += dev * dev;
            devCubeSum += dev * dev * dev;
            devFourthSum += dev * dev * dev * dev;
        }
        res.variance = devSquareSum / n;
        res.stdDev = qSqrt(res.variance);

        // ===== 原有中位数 =====
        QVector<double> sorted = data;
        std::sort(sorted.begin(), sorted.end());
        if (n % 2 == 0) {
            res.median = (sorted[n/2 - 1] + sorted[n/2]) / 2.0;
        } else {
            res.median = sorted[n/2];
        }

        // ===== 原有波形因子、峰值因子，除零防护 =====
        if (res.absMean > 1e-12) {
            res.formFactor = res.rms / res.absMean;
        }
        if (res.rms > 1e-12) {
            res.crestFactor = res.max / res.rms;
        }

        // ===== 原有偏度、峭度，除零防护 =====
        if (res.stdDev > 1e-12) {
            double std3 = qPow(res.stdDev, 3);
            double std4 = qPow(res.stdDev, 4);
            res.skewness = (devCubeSum / n) / std3;
            res.kurtosis = (devFourthSum / n) / std4;
        }

        // ===== 原有过零计数 =====
        for (int i = 1; i < n; i++) {
            if ((data[i-1] * data[i]) < 0) {
                res.zeroCrossCount++;
            }
        }

        // ===== 新增：半高宽 FWHM + 半高宽面积 =====
        calculateFwhm(data, res, maxIndex, sampleInterval);

        return res;
    }

private:
    // 线性插值辅助：已知 i1、i2 两点跨越 halfLevel，返回交点处的浮点索引
    static double interpolateIndex(const QVector<double>& data,
                                   int i1, int i2, double halfLevel)
    {
        const double y1 = data[i1];
        const double y2 = data[i2];
        const double dy = y2 - y1;
        if (qAbs(dy) < 1e-12) return static_cast<double>(i1);
        const double t = (halfLevel - y1) / dy;
        return i1 + t * (i2 - i1);
    }

    // 计算半高宽及其区间净面积
    static void calculateFwhm(const QVector<double>& data,
                              Result& res,
                              int peakIndex,
                              double sampleInterval)
    {
        const int n = data.size();
        const double peakHeight = res.max - res.baseline;

        // 峰高不明显（小于噪声量级）时判定无效，避免误报
        if (peakHeight <= 1e-12) {
            res.fwhmValid = false;
            return;
        }

        const double halfLevel = res.baseline + peakHeight * 0.5;
        res.halfLevel = halfLevel;

        // 从峰值处向左右两侧搜索，分别找第一次穿过 halfLevel 的位置
        bool isInvalidWaveform = false;// 根据半高宽判断是否是无效波形
#if LOWERXRAGY_VERSION
        // 左侧搜索
        double leftPos = -1.0;
        int leftIdx = -1;
        for (int i = peakIndex; i > 0; --i) {
            if (data[i-1] <= halfLevel && data[i] >= halfLevel) {
                leftPos = interpolateIndex(data, i-1, i, halfLevel);
                leftIdx = i - 1;
                break;
            }
        }
        // 右侧搜索
        double rightPos = -1.0;
        int rightIdx = -1;
        for (int i = peakIndex; i < n - 1; ++i) {
            if (data[i] >= halfLevel && data[i+1] <= halfLevel) {
                rightPos = interpolateIndex(data, i, i+1, halfLevel);
                rightIdx = i;
                break;
            }
        }

        // 半高宽信息
        res.fwhmLeftIndex = leftIdx;
        res.fwhmRightIndex = rightIdx;
        res.fwhm = (rightIdx - leftIdx) * sampleInterval;

        // 半高宽最小值        
        const double min_fwhm = 50.0;
        if ((rightPos - leftPos) < min_fwhm)
        {
            isInvalidWaveform = true;
        }
        else{
            // 重置半高宽位置，默认值改为前5，后20
            qint32 fwhmLeftPoints = 5;
            qint32 fwhmRightPoints = 20;
            GlobalSettings settings(CONFIG_FILENAME);
            bool groupExists = settings.childGroups().contains("Unfold");
            if (!groupExists)
            {
                // 2. 分组不存在，写入默认配置
                settings.beginGroup("Unfold");
                settings.setValue("MAX_VALID_STD_THRESHOLD", 30.0);
                settings.setValue("PULSE_SCALE_FACTOR", "1,1,1,1,1,1,1,1,1,1");
                settings.setValue("PULSE_OFFSET_CALIB", "0,0,0,0,0,0,0,0,0,0");
                settings.setValue("HalfHeightWidth_LeftPoints", fwhmLeftPoints);
                settings.setValue("HalfHeightWidth_RightPoints", fwhmRightPoints);
                settings.endGroup();
                // 强制写入磁盘，避免缓存丢失
                settings.sync();
            }
            else{
                fwhmLeftPoints = settings.value("HalfHeightWidth_LeftPoints", fwhmLeftPoints).toUInt();
                fwhmRightPoints = settings.value("HalfHeightWidth_RightPoints", fwhmRightPoints).toUInt();
            }

            leftPos = peakIndex - fwhmLeftPoints;
            rightPos = peakIndex + fwhmRightPoints;
        }
#else
        // 左侧搜索
        double leftPos = -1.0;
        int leftIdx = -1;
        for (int i = peakIndex; i > 0; --i) {
            if (data[i-1] <= halfLevel && data[i] >= halfLevel) {
                leftPos = interpolateIndex(data, i-1, i, halfLevel);
                leftIdx = i - 1;
                break;
            }
        }
        // 右侧搜索
        double rightPos = -1.0;
        int rightIdx = -1;
        for (int i = peakIndex; i < n - 1; ++i) {
            if (data[i] >= halfLevel && data[i+1] <= halfLevel) {
                rightPos = interpolateIndex(data, i, i+1, halfLevel);
                rightIdx = i;
                break;
            }
        }

        // 半高宽最小值
        const double min_fwhm = 50.0;
        if ((rightPos - leftPos) < min_fwhm)
            rightPos = leftPos + 70.0;
#endif //(1)

        // 两侧都找到交点，半高宽有效
        if (leftPos >= 0.0 && rightPos >= 0.0 && rightPos > leftPos) {
            res.fwhmValid = true;            

            // ===== 半高宽区间净面积：梯形积分（仅累加减基线后的正值部分）=====
            double area = 0.0;
            for (int i = 0; i < n - 1; ++i) {
                // 跳过完全落在区间外的段
                const double x1 = static_cast<double>(i);
                const double x2 = static_cast<double>(i + 1);
                if (x2 <= leftPos || x1 >= rightPos) continue;

                // 段内被 [leftPos, rightPos] 截断后的有效区间
                const double segL = qMax(x1, leftPos);
                const double segR = qMin(x2, rightPos);
                if (segR <= segL) continue;

                // 段两端在截断区间内的线性插值值（相对基线取正）
                const double y1raw = data[i];
                const double y2raw = data[i+1];
                const double dy = y2raw - y1raw;
                const double yL = y1raw + (segL - x1) * dy;
                const double yR = y1raw + (segR - x1) * dy;
                const double netL = qMax(0.0, yL - res.baseline);
                const double netR = qMax(0.0, yR - res.baseline);

                area += 0.5 * (netL + netR) * (segR - segL) * sampleInterval;
            }
            res.fwhmArea = area;

            // ===== 新增：半高宽区间净均值（扣除基线后）=====
            // 用插值交点 leftPos ~ rightPos 圈定区间，与半高宽/面积口径严格对齐
            // 累加时扣除基线并对负值截断，避免噪声负值拉低均值
            double sumInRange = 0.0;
            int cntInRange = 0;
            for (int i = 0; i < n; ++i) {
                const double xi = static_cast<double>(i);
                if (xi < leftPos || xi > rightPos) continue;
                sumInRange += qMax(0.0, data[i] - res.baseline);
                cntInRange++;
            }
            res.fwhmMeanLevel = (cntInRange > 0) ? (sumInRange / cntInRange) : 0.0;

            if (isInvalidWaveform)
            {
                res.fwhmValid = false;
            }
        }
    }

};

#endif // WAVEFORMSTATS_H
