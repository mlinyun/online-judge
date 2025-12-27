<script setup lang="ts">
/**
 * OJ Markdown 预览组件
 *
 * 基于 v-md-editor 封装的 Markdown 预览组件
 * 用于展示公告、题解、题目、讨论等内容
 */
defineOptions({ name: "OjMarkdownPreview" });

interface Props {
    /** 要预览的 Markdown 内容 */
    content?: string;
    /** 是否显示边框 */
    bordered?: boolean;
    /** 是否使用玻璃面板样式 */
    glass?: boolean;
}

withDefaults(defineProps<Props>(), {
    content: "",
    bordered: false,
    glass: false,
});
</script>

<template>
    <div class="oj-md-preview" :class="{ 'is-bordered': bordered, 'is-glass': glass }">
        <v-md-preview :text="content" />
    </div>
</template>

<style scoped>
.oj-md-preview {
    color: var(--oj-text-color);
}

.oj-md-preview.is-bordered {
    padding: var(--oj-spacing-4);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
}

.oj-md-preview.is-glass {
    padding: var(--oj-spacing-4);
    background: var(--oj-glass-bg);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
    backdrop-filter: blur(var(--oj-glass-blur));
}

/* v-md-preview 样式覆盖 */
.oj-md-preview :deep(.v-md-editor-preview) {
    padding: 0;
    color: var(--oj-text-color);
    background: transparent;
}

.oj-md-preview :deep(.v-md-editor-preview h1),
.oj-md-preview :deep(.v-md-editor-preview h2),
.oj-md-preview :deep(.v-md-editor-preview h3),
.oj-md-preview :deep(.v-md-editor-preview h4),
.oj-md-preview :deep(.v-md-editor-preview h5),
.oj-md-preview :deep(.v-md-editor-preview h6) {
    margin-top: var(--oj-spacing-6);
    margin-bottom: var(--oj-spacing-4);
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
    border-bottom-color: var(--oj-glass-border);
}

.oj-md-preview :deep(.v-md-editor-preview h1) {
    padding-bottom: var(--oj-spacing-2);
    font-size: var(--oj-font-size-2xl);
    border-bottom: 1px solid var(--oj-glass-border);
}

.oj-md-preview :deep(.v-md-editor-preview h2) {
    padding-bottom: var(--oj-spacing-2);
    font-size: var(--oj-font-size-xl);
    border-bottom: 1px solid var(--oj-glass-border);
}

.oj-md-preview :deep(.v-md-editor-preview h3) {
    font-size: var(--oj-font-size-lg);
}

.oj-md-preview :deep(.v-md-editor-preview p) {
    margin: var(--oj-spacing-4) 0;
    line-height: var(--oj-line-height-relaxed);
    color: var(--oj-text-color-secondary);
}

.oj-md-preview :deep(.v-md-editor-preview a) {
    color: var(--oj-color-primary);
    text-decoration: none;
    transition: color 0.15s ease;
}

.oj-md-preview :deep(.v-md-editor-preview a:hover) {
    color: var(--oj-color-primary-light);
    text-decoration: underline;
}

.oj-md-preview :deep(.v-md-editor-preview code) {
    padding: 2px 6px;
    font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace;
    font-size: 0.875em;
    color: var(--oj-color-primary);
    background: rgb(var(--oj-color-primary-rgb) / 10%);
    border-radius: var(--oj-radius-sm);
}

.oj-md-preview :deep(.v-md-editor-preview pre) {
    margin: var(--oj-spacing-4) 0;
    overflow-x: auto;
    background: var(--oj-editor-bg);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-lg);
}

.oj-md-preview :deep(.v-md-editor-preview pre code) {
    display: block;
    padding: var(--oj-spacing-4);
    overflow-x: auto;
    font-size: var(--oj-font-size-sm);
    line-height: var(--oj-line-height-relaxed);
    color: var(--oj-text-color);
    background: transparent;
    border-radius: 0;
}

.oj-md-preview :deep(.v-md-editor-preview blockquote) {
    padding: var(--oj-spacing-3) var(--oj-spacing-4);
    margin: var(--oj-spacing-4) 0;
    color: var(--oj-text-color-secondary);
    background: rgb(var(--oj-color-primary-rgb) / 5%);
    border-left: 4px solid var(--oj-color-primary);
    border-radius: 0 var(--oj-radius-md) var(--oj-radius-md) 0;
}

.oj-md-preview :deep(.v-md-editor-preview blockquote p) {
    margin: 0;
}

.oj-md-preview :deep(.v-md-editor-preview ul),
.oj-md-preview :deep(.v-md-editor-preview ol) {
    padding-left: var(--oj-spacing-6);
    margin: var(--oj-spacing-4) 0;
    color: var(--oj-text-color-secondary);
}

.oj-md-preview :deep(.v-md-editor-preview li) {
    margin: var(--oj-spacing-2) 0;
    line-height: var(--oj-line-height-relaxed);
}

.oj-md-preview :deep(.v-md-editor-preview table) {
    width: 100%;
    margin: var(--oj-spacing-4) 0;
    overflow: hidden;
    border-collapse: collapse;
    border-radius: var(--oj-radius-lg);
}

.oj-md-preview :deep(.v-md-editor-preview table th),
.oj-md-preview :deep(.v-md-editor-preview table td) {
    padding: var(--oj-spacing-3) var(--oj-spacing-4);
    text-align: left;
    border: 1px solid var(--oj-glass-border);
}

.oj-md-preview :deep(.v-md-editor-preview table th) {
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
    background: var(--oj-table-header-bg);
}

.oj-md-preview :deep(.v-md-editor-preview table td) {
    color: var(--oj-text-color-secondary);
}

.oj-md-preview :deep(.v-md-editor-preview table tr:hover td) {
    background: var(--oj-table-row-hover-bg);
}

.oj-md-preview :deep(.v-md-editor-preview hr) {
    margin: var(--oj-spacing-6) 0;
    border: none;
    border-top: 1px solid var(--oj-glass-border);
}

.oj-md-preview :deep(.v-md-editor-preview img) {
    max-width: 100%;
    border-radius: var(--oj-radius-lg);
    box-shadow: var(--oj-shadow-md);
}

/* Tip 插件样式 */
.oj-md-preview :deep(.v-md-editor-preview .v-md-plugin-tip) {
    margin: var(--oj-spacing-4) 0;
    border-radius: var(--oj-radius-lg);
}

/* Copy code 按钮样式 */
.oj-md-preview :deep(.v-md-editor-preview .v-md-copy-code-btn) {
    padding: var(--oj-spacing-1) var(--oj-spacing-2);
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-secondary);
    background: var(--oj-surface-overlay);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-md);
    transition:
        color 0.15s ease,
        border-color 0.15s ease;
}

.oj-md-preview :deep(.v-md-editor-preview .v-md-copy-code-btn:hover) {
    color: var(--oj-color-primary);
    border-color: var(--oj-color-primary);
}

/* 滚动条样式 */
.oj-md-preview :deep(pre)::-webkit-scrollbar {
    height: 6px;
}

.oj-md-preview :deep(pre)::-webkit-scrollbar-track {
    background: var(--oj-scrollbar-track);
    border-radius: 3px;
}

.oj-md-preview :deep(pre)::-webkit-scrollbar-thumb {
    background: var(--oj-scrollbar-thumb);
    border-radius: 3px;
}

.oj-md-preview :deep(pre)::-webkit-scrollbar-thumb:hover {
    background: var(--oj-scrollbar-thumb-hover);
}
</style>
