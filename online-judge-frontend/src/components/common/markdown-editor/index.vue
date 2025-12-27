<script setup lang="ts">
/**
 * OJ Markdown 编辑器组件
 *
 * 基于 v-md-editor 封装的 Markdown 编辑器
 * 支持公告、题解、题目、讨论等内容编辑
 */
import { computed, ref, useTemplateRef, watch } from "vue";

defineOptions({ name: "OjMarkdownEditor" });

interface Props {
    /** 编辑器内容（双向绑定） */
    modelValue?: string;
    /** 编辑器高度 */
    height?: string;
    /** 占位提示文字 */
    placeholder?: string;
    /** 是否禁用 */
    disabled?: boolean;
    /** 左侧工具栏配置 */
    leftToolbar?: string;
    /** 右侧工具栏配置 */
    rightToolbar?: string;
    /** 编辑模式: edit 纯编辑 / editable 编辑+预览 / preview 纯预览 */
    mode?: "edit" | "editable" | "preview";
}

const props = withDefaults(defineProps<Props>(), {
    modelValue: "",
    height: "500px",
    placeholder: "请输入内容...",
    disabled: false,
    leftToolbar: "undo redo clear | h bold italic strikethrough quote | ul ol table hr | link image code | save",
    rightToolbar: "toc   preview sync-scroll fullscreen",
    mode: "editable",
});

const emit = defineEmits<{
    "update:modelValue": [value: string];
    change: [value: string];
    save: [value: string];
}>();

// eslint-disable-next-line @typescript-eslint/no-explicit-any
const editorRef = useTemplateRef<any>("editorRef");
const isFocused = ref(false);

const content = computed({
    get: () => props.modelValue,
    set: (val: string) => {
        emit("update:modelValue", val);
        emit("change", val);
    },
});

watch(
    () => props.modelValue,
    (newVal) => {
        if (newVal !== content.value) {
            content.value = newVal;
        }
    }
);

const handleSave = () => {
    emit("save", content.value);
};

const handleFocus = () => {
    isFocused.value = true;
};

const handleBlur = () => {
    isFocused.value = false;
};

/**
 * 获取编辑器内容
 */
const getContent = (): string => {
    return content.value;
};

/**
 * 设置编辑器内容
 */
const setContent = (value: string): void => {
    content.value = value;
};

/**
 * 清空编辑器内容
 */
const clearContent = (): void => {
    content.value = "";
};

/**
 * 聚焦编辑器
 */
const focus = (): void => {
    // v-md-editor 实例的 focus 方法
    const editor = editorRef.value as unknown as { focus?: () => void } | null;
    editor?.focus?.();
};

defineExpose({
    getContent,
    setContent,
    clearContent,
    focus,
});
</script>

<template>
    <div class="oj-md-editor" :class="{ 'is-focused': isFocused, 'is-disabled': disabled }">
        <div class="editor-glow" />
        <div class="editor-container">
            <v-md-editor
                ref="editorRef"
                v-model="content"
                :height="height"
                :placeholder="placeholder"
                :disabled-menus="disabled ? [] : undefined"
                :left-toolbar="leftToolbar"
                :right-toolbar="rightToolbar"
                :mode="mode"
                @save="handleSave"
                @focus="handleFocus"
                @blur="handleBlur"
            />
        </div>
    </div>
</template>

<style scoped>
.oj-md-editor {
    position: relative;
    overflow: hidden;
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
    transition:
        border-color 0.2s ease,
        box-shadow 0.2s ease;
}

.oj-md-editor.is-focused {
    border-color: rgb(var(--oj-color-primary-rgb) / 50%);
    box-shadow:
        0 0 0 1px rgb(var(--oj-color-primary-rgb) / 20%) inset,
        0 0 24px rgb(var(--oj-color-primary-rgb) / 10%);
}

.oj-md-editor.is-disabled {
    pointer-events: none;
    opacity: 0.6;
}

.editor-glow {
    position: absolute;
    top: 0;
    left: 0;
    z-index: 0;
    width: 100%;
    height: 100%;
    pointer-events: none;
    background:
        radial-gradient(400px circle at 10% 20%, rgb(var(--oj-color-primary-rgb) / 8%) 0%, transparent 50%),
        radial-gradient(300px circle at 90% 80%, rgb(var(--oj-color-primary-rgb) / 5%) 0%, transparent 50%);
    opacity: 0;
    transition: opacity 0.3s ease;
}

.oj-md-editor.is-focused .editor-glow {
    opacity: 1;
}

.editor-container {
    position: relative;
    z-index: 1;
}

/* v-md-editor 样式覆盖 - 深色主题适配 */
.oj-md-editor :deep(.v-md-editor) {
    background: var(--oj-glass-bg);
    border: none;
    border-radius: var(--oj-radius-xl);
}

.oj-md-editor :deep(.v-md-editor__toolbar) {
    display: flex;
    flex-wrap: wrap;
    align-items: center;
    padding: var(--oj-spacing-2) var(--oj-spacing-3);
    background: var(--oj-table-header-bg);
    border-bottom: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl) var(--oj-radius-xl) 0 0;
}

.oj-md-editor :deep(.v-md-editor__toolbar-left),
.oj-md-editor :deep(.v-md-editor__toolbar-right) {
    display: flex;
    flex-wrap: wrap;
    align-items: center;
}

.oj-md-editor :deep(.v-md-editor__toolbar-item) {
    display: inline-flex;
    align-items: center;
    justify-content: center;
    width: 32px;
    height: 32px;
    padding: 0;
    color: var(--oj-text-color-secondary);
    border-radius: var(--oj-radius-md);
    transition:
        color 0.15s ease,
        background-color 0.15s ease;
}

.oj-md-editor :deep(.v-md-editor__toolbar-item:hover) {
    color: var(--oj-text-color);
    background-color: var(--oj-surface-hover);
}

.oj-md-editor :deep(.v-md-editor__toolbar-item--active) {
    color: var(--oj-color-primary);
    background-color: rgb(var(--oj-color-primary-rgb) / 15%);
}

.oj-md-editor :deep(.v-md-editor__toolbar-divider) {
    width: 1px;
    height: 20px;
    margin: 0 var(--oj-spacing-2);
    background-color: var(--oj-glass-border);
}

.oj-md-editor :deep(.v-md-editor__main) {
    background: transparent;
}

.oj-md-editor :deep(.v-md-editor__editor-wrapper) {
    background: var(--oj-glass-bg);
}

.oj-md-editor :deep(.v-md-editor__editor-wrapper .codemirror-wrapper) {
    background: var(--oj-glass-bg);
}

.oj-md-editor :deep(.CodeMirror) {
    font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace;
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-color);
    background: transparent;
}

.oj-md-editor :deep(.CodeMirror-gutters) {
    background: var(--oj-table-header-bg);
    border-right: 1px solid var(--oj-glass-border);
}

.oj-md-editor :deep(.CodeMirror-linenumber) {
    color: var(--oj-text-color-muted);
}

.oj-md-editor :deep(.CodeMirror-cursor) {
    border-left-color: var(--oj-color-primary);
}

.oj-md-editor :deep(.CodeMirror-selected) {
    background: rgb(var(--oj-color-primary-rgb) / 20%) !important;
}

.oj-md-editor :deep(.CodeMirror-focused .CodeMirror-selected) {
    background: rgb(var(--oj-color-primary-rgb) / 25%) !important;
}

.oj-md-editor :deep(.CodeMirror-line::selection),
.oj-md-editor :deep(.CodeMirror-line > span::selection),
.oj-md-editor :deep(.CodeMirror-line > span > span::selection) {
    background: rgb(var(--oj-color-primary-rgb) / 25%);
}

/* 预览区域样式 */
.oj-md-editor :deep(.v-md-editor__preview-wrapper) {
    padding: var(--oj-spacing-4);
    background: var(--oj-glass-bg);
    border-left: 1px solid var(--oj-glass-border);
}

.oj-md-editor :deep(.v-md-editor-preview) {
    color: var(--oj-text-color);
}

.oj-md-editor :deep(.v-md-editor-preview h1),
.oj-md-editor :deep(.v-md-editor-preview h2),
.oj-md-editor :deep(.v-md-editor-preview h3),
.oj-md-editor :deep(.v-md-editor-preview h4),
.oj-md-editor :deep(.v-md-editor-preview h5),
.oj-md-editor :deep(.v-md-editor-preview h6) {
    color: var(--oj-text-color);
    border-bottom-color: var(--oj-glass-border);
}

.oj-md-editor :deep(.v-md-editor-preview a) {
    color: var(--oj-color-primary);
}

.oj-md-editor :deep(.v-md-editor-preview a:hover) {
    color: var(--oj-color-primary-light);
}

.oj-md-editor :deep(.v-md-editor-preview code) {
    padding: 2px 6px;
    font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace;
    color: var(--oj-color-primary);
    background: rgb(var(--oj-color-primary-rgb) / 10%);
    border-radius: var(--oj-radius-sm);
}

.oj-md-editor :deep(.v-md-editor-preview pre) {
    background: var(--oj-editor-bg);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-lg);
}

.oj-md-editor :deep(.v-md-editor-preview pre code) {
    padding: 0;
    color: inherit;
    background: transparent;
}

.oj-md-editor :deep(.v-md-editor-preview blockquote) {
    padding-left: var(--oj-spacing-4);
    color: var(--oj-text-color-secondary);
    border-left: 4px solid var(--oj-color-primary);
}

.oj-md-editor :deep(.v-md-editor-preview table) {
    border-collapse: collapse;
}

.oj-md-editor :deep(.v-md-editor-preview table th),
.oj-md-editor :deep(.v-md-editor-preview table td) {
    padding: var(--oj-spacing-2) var(--oj-spacing-3);
    border: 1px solid var(--oj-glass-border);
}

.oj-md-editor :deep(.v-md-editor-preview table th) {
    background: var(--oj-table-header-bg);
}

.oj-md-editor :deep(.v-md-editor-preview table tr:hover) {
    background: var(--oj-table-row-hover-bg);
}

.oj-md-editor :deep(.v-md-editor-preview hr) {
    border-color: var(--oj-glass-border);
}

.oj-md-editor :deep(.v-md-editor-preview img) {
    max-width: 100%;
    border-radius: var(--oj-radius-lg);
}

/* 占位符样式 */
.oj-md-editor :deep(.CodeMirror-placeholder) {
    color: var(--oj-text-color-muted);
}

/* 滚动条样式 */
.oj-md-editor :deep(.CodeMirror-vscrollbar),
.oj-md-editor :deep(.CodeMirror-hscrollbar),
.oj-md-editor :deep(.v-md-editor__preview-wrapper) {
    &::-webkit-scrollbar {
        width: 8px;
        height: 8px;
    }

    &::-webkit-scrollbar-track {
        background: var(--oj-scrollbar-track);
        border-radius: 4px;
    }

    &::-webkit-scrollbar-thumb {
        background: var(--oj-scrollbar-thumb);
        border-radius: 4px;
    }

    &::-webkit-scrollbar-thumb:hover {
        background: var(--oj-scrollbar-thumb-hover);
    }
}

/* Dropdown 样式覆盖 */
.oj-md-editor :deep(.v-md-editor__toolbar-item-menu) {
    background: var(--oj-surface-overlay);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-lg);
    box-shadow: var(--oj-shadow-xl);
}

.oj-md-editor :deep(.v-md-editor__toolbar-item-menu li) {
    color: var(--oj-text-color-secondary);
}

.oj-md-editor :deep(.v-md-editor__toolbar-item-menu li:hover) {
    color: var(--oj-text-color);
    background: var(--oj-surface-hover);
}

/* Tip 插件样式 */
.oj-md-editor :deep(.v-md-editor-preview .v-md-plugin-tip) {
    border-radius: var(--oj-radius-lg);
}

/* Emoji 插件样式 */
.oj-md-editor :deep(.v-md-editor__emoji-wrapper) {
    background: var(--oj-surface-overlay);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-lg);
    box-shadow: var(--oj-shadow-xl);
}

/* Copy code 按钮样式 */
.oj-md-editor :deep(.v-md-editor-preview .v-md-copy-code-btn) {
    color: var(--oj-text-color-secondary);
    background: var(--oj-surface-overlay);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-md);
}

.oj-md-editor :deep(.v-md-editor-preview .v-md-copy-code-btn:hover) {
    color: var(--oj-color-primary);
    border-color: var(--oj-color-primary);
}
</style>
