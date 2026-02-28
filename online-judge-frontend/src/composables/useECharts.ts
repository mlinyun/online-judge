/**
 * ECharts 组合式函数
 *
 * @description 提供 ECharts 实例的响应式管理，支持自动 resize 和主题感知
 */
import type { ShallowRef } from "vue";
import * as echarts from "echarts/core";
import { PieChart } from "echarts/charts";
import { TitleComponent, TooltipComponent, LegendComponent } from "echarts/components";
import { LabelLayout } from "echarts/features";
import { CanvasRenderer } from "echarts/renderers";

// 注册 ECharts 组件（Tree-shaking）
echarts.use([PieChart, TitleComponent, TooltipComponent, LegendComponent, LabelLayout, CanvasRenderer]);

export type EChartsOption = echarts.ComposeOption<
    | import("echarts/charts").PieSeriesOption
    | import("echarts/components").TitleComponentOption
    | import("echarts/components").TooltipComponentOption
    | import("echarts/components").LegendComponentOption
>;

/**
 * ECharts 组合式函数
 *
 * @param chartRef - DOM 元素引用 (ShallowRef)
 * @returns chart 实例和更新方法
 */
export function useECharts(chartRef: ShallowRef<HTMLElement | null>) {
    let chartInstance: echarts.ECharts | null = null;

    const initChart = () => {
        if (!chartRef.value) return;
        chartInstance = echarts.init(chartRef.value);
    };

    const setOption = (option: EChartsOption) => {
        if (!chartInstance) initChart();
        chartInstance?.setOption(option, { notMerge: true });
    };

    const resize = () => {
        chartInstance?.resize({ animation: { duration: 300 } });
    };

    // 监听 DOM 元素
    watch(
        () => chartRef.value,
        (el) => {
            if (el) {
                initChart();
            }
        },
        { flush: "post" }
    );

    // 监听窗口 resize
    let resizeObserver: ResizeObserver | null = null;
    onMounted(() => {
        window.addEventListener("resize", resize);
        // 使用 ResizeObserver 感知容器大小变化
        if (chartRef.value) {
            resizeObserver = new ResizeObserver(() => resize());
            resizeObserver.observe(chartRef.value);
        }
    });

    onUnmounted(() => {
        window.removeEventListener("resize", resize);
        resizeObserver?.disconnect();
        chartInstance?.dispose();
        chartInstance = null;
    });

    return { setOption, resize, getInstance: () => chartInstance };
}
