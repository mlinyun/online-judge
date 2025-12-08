<script setup lang="ts">
/**
 * 响应式行组件
 * 用于包裹 OjCol 列组件，提供 flex 布局
 */
import { computed, provide } from "vue";

defineOptions({ name: "OjRow" });

interface Props {
    /** 列间距 (px) */
    gutter?: number | [number, number];
    /** 水平对齐方式 */
    justify?: "start" | "center" | "end" | "space-between" | "space-around" | "space-evenly";
    /** 垂直对齐方式 */
    align?: "top" | "middle" | "bottom" | "stretch";
    /** 是否自动换行 */
    wrap?: boolean;
}

const props = withDefaults(defineProps<Props>(), {
    gutter: 0,
    justify: "start",
    align: "top",
    wrap: true,
});

// 计算 gutter
const gutterX = computed(() => (Array.isArray(props.gutter) ? props.gutter[0] : props.gutter));
const gutterY = computed(() => (Array.isArray(props.gutter) ? props.gutter[1] : 0));

// 提供给子组件
provide("rowGutter", gutterX);

// 计算样式
const rowStyle = computed(() => {
    const style: Record<string, string> = {};

    if (gutterX.value > 0) {
        const margin = `-${gutterX.value / 2}px`;
        style.marginLeft = margin;
        style.marginRight = margin;
    }

    if (gutterY.value > 0) {
        style.rowGap = `${gutterY.value}px`;
    }

    return style;
});

// 计算 class
const rowClass = computed(() => {
    return {
        "oj-row": true,
        [`oj-row--justify-${props.justify}`]: props.justify !== "start",
        [`oj-row--align-${props.align}`]: props.align !== "top",
        "oj-row--nowrap": !props.wrap,
    };
});
</script>

<template>
    <div :class="rowClass" :style="rowStyle">
        <slot />
    </div>
</template>

<style scoped>
.oj-row {
    display: flex;
    flex-wrap: wrap;
}

.oj-row--nowrap {
    flex-wrap: nowrap;
}

/* 水平对齐 */
.oj-row--justify-center {
    justify-content: center;
}

.oj-row--justify-end {
    justify-content: flex-end;
}

.oj-row--justify-space-between {
    justify-content: space-between;
}

.oj-row--justify-space-around {
    justify-content: space-around;
}

.oj-row--justify-space-evenly {
    justify-content: space-evenly;
}

/* 垂直对齐 */
.oj-row--align-middle {
    align-items: center;
}

.oj-row--align-bottom {
    align-items: flex-end;
}

.oj-row--align-stretch {
    align-items: stretch;
}
</style>
