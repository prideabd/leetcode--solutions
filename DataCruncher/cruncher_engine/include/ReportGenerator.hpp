#ifndef REPORT_GENERATOR_HPP
#define REPORT_GENERATOR_HPP

#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

namespace dc {
    class ReportGenerator {
    public:
        /**
         * @brief 生成交互式 HTML 报表
         * @param filename 输出文件名 (如 "report.html")
         * @param x_data 横坐标数据 (桶中心值)
         * @param y_data 纵坐标数据 (频率/个数)
         */
        static void generateHtml(const std::string& filename, 
                               const std::vector<double>& x_data, 
                               const std::vector<double>& y_data) {
            std::ofstream file(filename);
            if (!file.is_open()) return;

            // 高效将 vector 转为 JS 数组字符串 [1.1, 2.2, ...]
            auto vectorToString = [](const std::vector<double>& vec) {
                std::ostringstream oss;
                oss << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    // 固定两位小数，减小文件体积并防止 JS 解析精度问题
                    oss << std::fixed << std::setprecision(2) << vec[i];
                    if (i < vec.size() - 1) oss << ",";
                }
                oss << "]";
                return oss.str();
            };

            std::string x_str = vectorToString(x_data);
            std::string y_str = vectorToString(y_data);

            // 注入 HTML 模板
            file << R"(<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>DataCruncher 分析报告</title>
    <!-- 修复：使用标准的 .js 文件 -->
    <script src="https://cdn.jsdelivr.net/npm/echarts@5.4.3/dist/echarts.min.js"></script>
    <style>
        body { 
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; 
            display: flex; 
            flex-direction: column; 
            align-items: center; 
            background: #f4f7f6; 
            margin-top: 50px;
        }
        .chart-container {
            width: 900px;
            height: 600px;
            background: white; 
            padding: 30px; 
            border-radius: 12px; 
            box-shadow: 0 10px 25px rgba(0,0,0,0.1);
        }
        h2 { color: #333; margin-bottom: 20px; }
    </style>
</head>
<body>
    <h2>DataCruncher 统计分析报告</h2>
    <div id="main" class="chart-container"></div>
    
    <script type="text/javascript">
        // 初始化 ECharts 实例
        var myChart = echarts.init(document.getElementById('main'));

        var option = {
            title: { text: '数据分布频率', left: 'center' },
            tooltip: { 
                trigger: 'axis',
                axisPointer: { type: 'shadow' } 
            },
            grid: { bottom: '10%', containLabel: true },
            xAxis: { 
                type: 'category', 
                data: )" << x_str << R"(,
                name: '数值区间'
            },
            yAxis: { 
                type: 'value',
                name: '频数'
            },
            series: [{
                name: '出现次数',
                data: )" << y_str << R"(,
                type: 'bar',
                itemStyle: { 
                    color: '#5470c6',
                    borderRadius: [5, 5, 0, 0]
                },
                emphasis: {
                    itemStyle: { color: '#73c0de' }
                }
            }]
        };

        myChart.setOption(option);

        // 响应式窗口大小调整
        window.onresize = function() {
            myChart.resize();
        };
    </script>
</body>
</html>)";
            file.close();
        }
    };
}

#endif // REPORT_GENERATOR_HPP