<script setup lang="ts">
import { Search, PriceTag } from "@element-plus/icons-vue";

defineOptions({ name: "ProblemSearchBar" });

interface Props {
    keyword: string;
    selectedTags: string[];
    allTags: string[];
    total?: number;
    hasSearch?: boolean;
}

withDefaults(defineProps<Props>(), {
    total: 0,
    hasSearch: false,
});

const emit = defineEmits<{
    "update:keyword": [value: string];
    "update:selectedTags": [value: string[]];
    search: [];
}>();

const onEnter = () => {
    emit("search");
};
</script>

<template>
    <div class="toolbar oj-glass-panel" aria-label="题库搜索">
        <div class="toolbar-left">
            <el-input
                :model-value="keyword"
                class="toolbar-field"
                :prefix-icon="Search"
                clearable
                placeholder="搜索题目 ID 或 标题..."
                @update:model-value="(v: string) => emit('update:keyword', v)"
                @keydown.enter.prevent="onEnter"
            />

            <el-select
                :model-value="selectedTags"
                class="toolbar-field toolbar-select"
                multiple
                filterable
                collapse-tags
                collapse-tags-tooltip
                clearable
                :prefix-icon="PriceTag"
                placeholder="选择标签"
                popper-class="oj-problem-list-popper"
                @update:model-value="(v: string[]) => emit('update:selectedTags', v)"
            >
                <el-option v-for="t in allTags" :key="t" :label="t" :value="t" />
            </el-select>
        </div>

        <div class="toolbar-right">
            <div class="meta-chip" :class="{ 'is-active': hasSearch }">
                <span class="meta-k">Total</span>
                <span class="meta-v">{{ total }}</span>
            </div>
        </div>
    </div>
</template>

<style scoped>
/* Toolbar (aligned with StatusRecord toolbar) */
.toolbar {
    position: relative;
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-4);
    align-items: center;
    justify-content: space-between;
    padding: var(--oj-spacing-4);
    overflow: hidden;
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
    transition:
        border-color 0.2s ease,
        transform 0.2s ease;

    --el-color-primary: rgb(var(--oj-color-primary-rgb));
    --el-input-focus-border-color: rgb(var(--oj-color-primary-rgb));
}

.toolbar::before {
    position: absolute;
    inset: 0;
    pointer-events: none;
    content: "";
    background:
        radial-gradient(520px circle at 12% 20%, rgb(var(--oj-color-primary-rgb) / 18%) 0%, transparent 55%),
        radial-gradient(420px circle at 88% -10%, rgb(var(--oj-color-primary-rgb) / 10%) 0%, transparent 55%),
        repeating-linear-gradient(90deg, rgb(var(--oj-color-primary-rgb) / 6%) 0 1px, transparent 1px 16px);
    opacity: 0.9;
}

.toolbar::after {
    position: absolute;
    inset: 0;
    pointer-events: none;
    content: "";
    box-shadow:
        0 0 0 1px rgb(var(--oj-color-primary-rgb) / 10%) inset,
        0 18px 48px rgb(var(--oj-color-primary-rgb) / 10%);
    opacity: 0.75;
}

.toolbar:hover {
    border-color: rgb(var(--oj-color-primary-rgb) / 45%);
    transform: translateY(-1px);
}

.toolbar-left {
    display: grid;
    flex: 1;
    grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
    gap: var(--oj-spacing-3);
    align-items: center;
    min-width: 0;
}

@media (width <=1024px) {
    .toolbar-left {
        grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
    }
}

.toolbar-right {
    display: flex;
    gap: var(--oj-spacing-3);
    align-items: center;
}

.toolbar-field {
    width: 100%;
    min-width: 0;
}

.toolbar-select {
    width: 100%;
}

/* Input Adaptation */
.toolbar :deep(.el-input__wrapper) {
    background-color: var(--oj-input-bg);
    border-radius: var(--oj-radius-lg);
    box-shadow: 0 0 0 1px var(--oj-input-border) inset;
    transition: all 0.2s ease;
}

.toolbar :deep(.el-input__wrapper:hover) {
    background-color: var(--oj-bg-hover);
    box-shadow: 0 0 0 1px var(--oj-border-hover) inset;
}

.toolbar :deep(.el-input__wrapper.is-focus) {
    background-color: var(--oj-bg-hover);
    box-shadow:
        0 0 0 1px rgb(var(--oj-color-primary-rgb) / 70%) inset,
        0 0 0 4px rgb(var(--oj-color-primary-rgb) / 14%);
}

.toolbar :deep(.el-input__inner) {
    color: var(--oj-input-text);
}

.toolbar :deep(.el-input__inner::placeholder) {
    color: var(--oj-input-placeholder);
}

.toolbar :deep(.el-input__prefix-inner) {
    color: var(--oj-text-color-secondary);
}

/* Select dropdown (popper is teleported to body) */
:global(.oj-problem-list-popper) {
    --el-color-primary: rgb(var(--oj-color-primary-rgb));
}

:global(.oj-problem-list-popper .el-select-dropdown__item.is-selected) {
    color: rgb(var(--oj-color-primary-rgb));
}

:global(.oj-problem-list-popper .el-select-dropdown__item:hover) {
    color: rgb(var(--oj-color-primary-rgb));
}

.meta-chip {
    position: relative;
    z-index: 1;
    display: inline-flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    padding: var(--oj-spacing-2) var(--oj-spacing-3);
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-secondary);
    background: rgb(var(--oj-color-primary-rgb) / 8%);
    border: 1px solid rgb(var(--oj-color-primary-rgb) / 18%);
    border-radius: var(--oj-radius-lg);
}

.meta-chip.is-active {
    color: var(--oj-text-color);
    border-color: rgb(var(--oj-color-primary-rgb) / 32%);
    box-shadow: 0 0 0 1px rgb(var(--oj-color-primary-rgb) / 10%) inset;
}

.meta-k {
    text-transform: uppercase;
    letter-spacing: var(--oj-letter-spacing-wide);
    opacity: 0.9;
}

.meta-v {
    font-weight: var(--oj-font-weight-semibold);
    font-variant-numeric: tabular-nums;
    color: var(--oj-color-primary);
}
</style>
