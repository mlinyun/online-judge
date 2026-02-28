<script setup lang="ts">
/**
 * 饼图组件
 *
 * @description 基于 ECharts 的主题感知饼图，支持自动 resize
 */
import { useECharts, type EChartsOption } from "@/composables/useECharts";

defineOptions({ name: "OjPieChart" });

export interface PieDataItem {
    name: string;
    value: number;
    color?: string;
}

const props = withDefaults(
    defineProps<{
        /** 饼图数据 */
        data: PieDataItem[];
        /** 标题 */
        title?: string;
        /** 是否环形图 */
        donut?: boolean;
    }>(),
    {
        title: "",
        donut: true,
    },
);

const chartRef = shallowRef<HTMLElement | null>(null);
const { setOption } = useECharts(chartRef);

// 主题颜色
const COLORS = [
    "rgba(6, 182, 212, 0.85)", // cyan (primary)
    "rgba(248, 113, 113, 0.85)", // red (danger)
    "rgba(251, 191, 36, 0.85)", // amber (warning)
    "rgba(52, 211, 153, 0.85)", // emerald (success)
    "rgba(168, 85, 247, 0.85)", // purple
    "rgba(96, 165, 250, 0.85)", // blue
];

const buildOption = (): EChartsOption => {
    const data = props.data.map((item, idx) => ({
        name: item.name,
        value: item.value,
        itemStyle: {
            color: item.color || COLORS[idx % COLORS.length],
        },
    }));

    return {
        title: props.title
            ? {
                  text: props.title,
                  left: "center",
                  textStyle: {
                      color: "rgba(255, 255, 255, 0.85)",
                      fontSize: 14,
                      fontWeight: 500,
                  },
              }
            : undefined,
        tooltip: {
            trigger: "item",
            backgroundColor: "rgba(15, 23, 42, 0.9)",
            borderColor: "rgba(6, 182, 212, 0.3)",
            textStyle: { color: "rgba(255, 255, 255, 0.85)", fontSize: 13 },
            formatter: "{b}: {c} ({d}%)",
        },
        legend: {
            orient: "horizontal",
            bottom: 0,
            itemWidth: 12,
            itemHeight: 12,
            itemGap: 16,
            textStyle: {
                color: "rgba(255, 255, 255, 0.6)",
                fontSize: 12,
            },
        },
        series: [
            {
                type: "pie",
                radius: props.donut ? ["40%", "70%"] : ["0%", "70%"],
                center: ["50%", "45%"],
                avoidLabelOverlap: true,
                padAngle: 2,
                itemStyle: {
                    borderRadius: 6,
                },
                label: {
                    show: false,
                },
                emphasis: {
                    label: {
                        show: true,
                        fontSize: 14,
                        fontWeight: "bold",
                        color: "rgba(255, 255, 255, 0.9)",
                    },
                    itemStyle: {
                        shadowBlur: 20,
                        shadowColor: "rgba(6, 182, 212, 0.4)",
                    },
                },
                data,
            },
        ],
    };
};

watch(
    () => props.data,
    () => {
        if (props.data.length > 0) {
            nextTick(() => setOption(buildOption()));
        }
    },
    { deep: true, immediate: true },
);

// 初始化后刷新
onMounted(() => {
    if (props.data.length > 0) {
        nextTick(() => setOption(buildOption()));
    }
});
</script>

<template>
    <div ref="chartRef" class="oj-pie-chart" />
</template>

<style scoped>
.oj-pie-chart {
    width: 100%;
    min-height: 280px;
}
</style>
