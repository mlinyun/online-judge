<script setup lang="ts">
/**
 * 响应式列组件
 * 基于 12 列栅格系统，支持响应式断点
 */
import { computed, inject } from "vue";

defineOptions({ name: "OjCol" });

interface Props {
    /** 默认跨度 (0-12，0 表示隐藏) */
    span?: number;
    /** 左偏移 */
    offset?: number;
    /** 向右推动 */
    push?: number;
    /** 向左拉动 */
    pull?: number;
    /** sm 断点 (≥640px) */
    sm?: number | { span?: number; offset?: number };
    /** md 断点 (≥768px) */
    md?: number | { span?: number; offset?: number };
    /** lg 断点 (≥1024px) */
    lg?: number | { span?: number; offset?: number };
    /** xl 断点 (≥1280px) */
    xl?: number | { span?: number; offset?: number };
    /** 2xl 断点 (≥1536px) */
    xxl?: number | { span?: number; offset?: number };
}

const props = withDefaults(defineProps<Props>(), {
    span: 24,
    offset: 0,
    push: 0,
    pull: 0,
});

// 从父组件获取 gutter
const gutter = inject<{ value: number }>("rowGutter", { value: 0 });

// 计算样式
const colStyle = computed(() => {
    const style: Record<string, string> = {};

    if (gutter.value > 0) {
        const padding = `${gutter.value / 2}px`;
        style.paddingLeft = padding;
        style.paddingRight = padding;
    }

    return style;
});

// 解析断点配置
const parseBreakpoint = (value: number | { span?: number; offset?: number } | undefined) => {
    if (value === undefined) return null;
    if (typeof value === "number") {
        return { span: value };
    }
    return value;
};

// 计算 class
const colClass = computed(() => {
    const classes: string[] = ["oj-col"];

    // 默认跨度
    if (props.span !== undefined) {
        classes.push(`oj-col-${props.span}`);
    }

    // 偏移
    if (props.offset > 0) {
        classes.push(`oj-col-offset-${props.offset}`);
    }

    // 推拉
    if (props.push > 0) {
        classes.push(`oj-col-push-${props.push}`);
    }
    if (props.pull > 0) {
        classes.push(`oj-col-pull-${props.pull}`);
    }

    // 响应式断点
    const breakpoints = [
        { name: "sm", value: props.sm },
        { name: "md", value: props.md },
        { name: "lg", value: props.lg },
        { name: "xl", value: props.xl },
        { name: "xxl", value: props.xxl },
    ];

    breakpoints.forEach(({ name, value }) => {
        const config = parseBreakpoint(value);
        if (config) {
            if (config.span !== undefined) {
                classes.push(`oj-col-${name}-${config.span}`);
            }
            if (config.offset !== undefined) {
                classes.push(`oj-col-${name}-offset-${config.offset}`);
            }
        }
    });

    return classes;
});
</script>

<template>
    <div :class="colClass" :style="colStyle">
        <slot />
    </div>
</template>

<style scoped>
.oj-col {
    box-sizing: border-box;
}

/* 生成 0-24 列宽度 */
.oj-col-0 {
    display: none;
}

.oj-col-1 {
    flex: 0 0 4.166667%;
    max-width: 4.166667%;
}

.oj-col-2 {
    flex: 0 0 8.333333%;
    max-width: 8.333333%;
}

.oj-col-3 {
    flex: 0 0 12.5%;
    max-width: 12.5%;
}

.oj-col-4 {
    flex: 0 0 16.666667%;
    max-width: 16.666667%;
}

.oj-col-5 {
    flex: 0 0 20.833333%;
    max-width: 20.833333%;
}

.oj-col-6 {
    flex: 0 0 25%;
    max-width: 25%;
}

.oj-col-7 {
    flex: 0 0 29.166667%;
    max-width: 29.166667%;
}

.oj-col-8 {
    flex: 0 0 33.333333%;
    max-width: 33.333333%;
}

.oj-col-9 {
    flex: 0 0 37.5%;
    max-width: 37.5%;
}

.oj-col-10 {
    flex: 0 0 41.666667%;
    max-width: 41.666667%;
}

.oj-col-11 {
    flex: 0 0 45.833333%;
    max-width: 45.833333%;
}

.oj-col-12 {
    flex: 0 0 50%;
    max-width: 50%;
}

.oj-col-13 {
    flex: 0 0 54.166667%;
    max-width: 54.166667%;
}

.oj-col-14 {
    flex: 0 0 58.333333%;
    max-width: 58.333333%;
}

.oj-col-15 {
    flex: 0 0 62.5%;
    max-width: 62.5%;
}

.oj-col-16 {
    flex: 0 0 66.666667%;
    max-width: 66.666667%;
}

.oj-col-17 {
    flex: 0 0 70.833333%;
    max-width: 70.833333%;
}

.oj-col-18 {
    flex: 0 0 75%;
    max-width: 75%;
}

.oj-col-19 {
    flex: 0 0 79.166667%;
    max-width: 79.166667%;
}

.oj-col-20 {
    flex: 0 0 83.333333%;
    max-width: 83.333333%;
}

.oj-col-21 {
    flex: 0 0 87.5%;
    max-width: 87.5%;
}

.oj-col-22 {
    flex: 0 0 91.666667%;
    max-width: 91.666667%;
}

.oj-col-23 {
    flex: 0 0 95.833333%;
    max-width: 95.833333%;
}

.oj-col-24 {
    flex: 0 0 100%;
    max-width: 100%;
}

/* 偏移 */
.oj-col-offset-1 {
    margin-left: 4.166667%;
}

.oj-col-offset-2 {
    margin-left: 8.333333%;
}

.oj-col-offset-3 {
    margin-left: 12.5%;
}

.oj-col-offset-4 {
    margin-left: 16.666667%;
}

.oj-col-offset-5 {
    margin-left: 20.833333%;
}

.oj-col-offset-6 {
    margin-left: 25%;
}

.oj-col-offset-7 {
    margin-left: 29.166667%;
}

.oj-col-offset-8 {
    margin-left: 33.333333%;
}

.oj-col-offset-9 {
    margin-left: 37.5%;
}

.oj-col-offset-10 {
    margin-left: 41.666667%;
}

.oj-col-offset-11 {
    margin-left: 45.833333%;
}

.oj-col-offset-12 {
    margin-left: 50%;
}

/* 响应式断点 - sm (≥640px) */
@media (width >=640px) {
    .oj-col-sm-0 {
        display: none;
    }

    .oj-col-sm-1 {
        flex: 0 0 4.166667%;
        max-width: 4.166667%;
    }

    .oj-col-sm-2 {
        flex: 0 0 8.333333%;
        max-width: 8.333333%;
    }

    .oj-col-sm-3 {
        flex: 0 0 12.5%;
        max-width: 12.5%;
    }

    .oj-col-sm-4 {
        flex: 0 0 16.666667%;
        max-width: 16.666667%;
    }

    .oj-col-sm-5 {
        flex: 0 0 20.833333%;
        max-width: 20.833333%;
    }

    .oj-col-sm-6 {
        flex: 0 0 25%;
        max-width: 25%;
    }

    .oj-col-sm-8 {
        flex: 0 0 33.333333%;
        max-width: 33.333333%;
    }

    .oj-col-sm-12 {
        flex: 0 0 50%;
        max-width: 50%;
    }

    .oj-col-sm-16 {
        flex: 0 0 66.666667%;
        max-width: 66.666667%;
    }

    .oj-col-sm-18 {
        flex: 0 0 75%;
        max-width: 75%;
    }

    .oj-col-sm-24 {
        flex: 0 0 100%;
        max-width: 100%;
    }
}

/* 响应式断点 - md (≥768px) */
@media (width >=768px) {
    .oj-col-md-0 {
        display: none;
    }

    .oj-col-md-1 {
        flex: 0 0 4.166667%;
        max-width: 4.166667%;
    }

    .oj-col-md-2 {
        flex: 0 0 8.333333%;
        max-width: 8.333333%;
    }

    .oj-col-md-3 {
        flex: 0 0 12.5%;
        max-width: 12.5%;
    }

    .oj-col-md-4 {
        flex: 0 0 16.666667%;
        max-width: 16.666667%;
    }

    .oj-col-md-5 {
        flex: 0 0 20.833333%;
        max-width: 20.833333%;
    }

    .oj-col-md-6 {
        flex: 0 0 25%;
        max-width: 25%;
    }

    .oj-col-md-8 {
        flex: 0 0 33.333333%;
        max-width: 33.333333%;
    }

    .oj-col-md-12 {
        flex: 0 0 50%;
        max-width: 50%;
    }

    .oj-col-md-16 {
        flex: 0 0 66.666667%;
        max-width: 66.666667%;
    }

    .oj-col-md-18 {
        flex: 0 0 75%;
        max-width: 75%;
    }

    .oj-col-md-24 {
        flex: 0 0 100%;
        max-width: 100%;
    }
}

/* 响应式断点 - lg (≥1024px) */
@media (width >=1024px) {
    .oj-col-lg-0 {
        display: none;
    }

    .oj-col-lg-1 {
        flex: 0 0 4.166667%;
        max-width: 4.166667%;
    }

    .oj-col-lg-2 {
        flex: 0 0 8.333333%;
        max-width: 8.333333%;
    }

    .oj-col-lg-3 {
        flex: 0 0 12.5%;
        max-width: 12.5%;
    }

    .oj-col-lg-4 {
        flex: 0 0 16.666667%;
        max-width: 16.666667%;
    }

    .oj-col-lg-5 {
        flex: 0 0 20.833333%;
        max-width: 20.833333%;
    }

    .oj-col-lg-6 {
        flex: 0 0 25%;
        max-width: 25%;
    }

    .oj-col-lg-8 {
        flex: 0 0 33.333333%;
        max-width: 33.333333%;
    }

    .oj-col-lg-12 {
        flex: 0 0 50%;
        max-width: 50%;
    }

    .oj-col-lg-16 {
        flex: 0 0 66.666667%;
        max-width: 66.666667%;
    }

    .oj-col-lg-18 {
        flex: 0 0 75%;
        max-width: 75%;
    }

    .oj-col-lg-24 {
        flex: 0 0 100%;
        max-width: 100%;
    }
}

/* 响应式断点 - xl (≥1280px) */
@media (width >=1280px) {
    .oj-col-xl-0 {
        display: none;
    }

    .oj-col-xl-1 {
        flex: 0 0 4.166667%;
        max-width: 4.166667%;
    }

    .oj-col-xl-2 {
        flex: 0 0 8.333333%;
        max-width: 8.333333%;
    }

    .oj-col-xl-3 {
        flex: 0 0 12.5%;
        max-width: 12.5%;
    }

    .oj-col-xl-4 {
        flex: 0 0 16.666667%;
        max-width: 16.666667%;
    }

    .oj-col-xl-5 {
        flex: 0 0 20.833333%;
        max-width: 20.833333%;
    }

    .oj-col-xl-6 {
        flex: 0 0 25%;
        max-width: 25%;
    }

    .oj-col-xl-8 {
        flex: 0 0 33.333333%;
        max-width: 33.333333%;
    }

    .oj-col-xl-12 {
        flex: 0 0 50%;
        max-width: 50%;
    }

    .oj-col-xl-16 {
        flex: 0 0 66.666667%;
        max-width: 66.666667%;
    }

    .oj-col-xl-18 {
        flex: 0 0 75%;
        max-width: 75%;
    }

    .oj-col-xl-24 {
        flex: 0 0 100%;
        max-width: 100%;
    }
}

/* 响应式断点 - xxl (≥1536px) */
@media (width >=1536px) {
    .oj-col-xxl-0 {
        display: none;
    }

    .oj-col-xxl-1 {
        flex: 0 0 4.166667%;
        max-width: 4.166667%;
    }

    .oj-col-xxl-2 {
        flex: 0 0 8.333333%;
        max-width: 8.333333%;
    }

    .oj-col-xxl-3 {
        flex: 0 0 12.5%;
        max-width: 12.5%;
    }

    .oj-col-xxl-4 {
        flex: 0 0 16.666667%;
        max-width: 16.666667%;
    }

    .oj-col-xxl-5 {
        flex: 0 0 20.833333%;
        max-width: 20.833333%;
    }

    .oj-col-xxl-6 {
        flex: 0 0 25%;
        max-width: 25%;
    }

    .oj-col-xxl-8 {
        flex: 0 0 33.333333%;
        max-width: 33.333333%;
    }

    .oj-col-xxl-12 {
        flex: 0 0 50%;
        max-width: 50%;
    }

    .oj-col-xxl-16 {
        flex: 0 0 66.666667%;
        max-width: 66.666667%;
    }

    .oj-col-xxl-18 {
        flex: 0 0 75%;
        max-width: 75%;
    }

    .oj-col-xxl-24 {
        flex: 0 0 100%;
        max-width: 100%;
    }
}
</style>
