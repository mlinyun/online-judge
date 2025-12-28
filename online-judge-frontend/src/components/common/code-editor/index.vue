<script setup lang="ts">
/**
 * OJ Code Editor
 *
 * 基于 monaco-editor 的代码编辑器封装：
 * - v-model 双向绑定代码内容
 * - language 切换（OJ 语言 -> Monaco languageId）
 * - 跟随站点主题切换（dark/light）
 */

import { computed, onBeforeUnmount, onMounted, ref, watch } from "vue";
import { storeToRefs } from "pinia";

import { useSettingStore } from "@/stores/modules/setting";
import { loadMonaco } from "@/utils/monaco";
import { toMonacoLanguageId, type OjLanguage } from "@/utils/monaco/language";

defineOptions({ name: "OjCodeEditor" });

interface Props {
    /** 代码内容（双向绑定） */
    modelValue?: string;
    /** OJ 语言（用于高亮/智能提示） */
    language?: OjLanguage | string;
    /** 是否只读 */
    readonly?: boolean;
    /** 编辑器高度 */
    height?: string;
    /** 编辑器宽度 */
    width?: string;
    /** 是否自动布局（容器尺寸变化时自动 layout） */
    autoLayout?: boolean;
}

const props = withDefaults(defineProps<Props>(), {
    modelValue: "",
    language: "cpp",
    readonly: false,
    height: "360px",
    width: "100%",
    autoLayout: true,
});

const emit = defineEmits<{
    "update:modelValue": [value: string];
    change: [value: string];
    ready: [];
}>();

const settingStore = useSettingStore();
const { isDark } = storeToRefs(settingStore);

const containerRef = ref<HTMLDivElement | null>(null);

// eslint-disable-next-line @typescript-eslint/no-explicit-any
let monaco: any | null = null;
// eslint-disable-next-line @typescript-eslint/no-explicit-any
let editor: any | null = null;
// eslint-disable-next-line @typescript-eslint/no-explicit-any
let model: any | null = null;

let isApplyingExternalValue = false;
let resizeObserver: ResizeObserver | null = null;
let disposeContentListener: { dispose: () => void } | null = null;

const monacoTheme = computed(() => (isDark.value ? "vs-dark" : "vs"));

const applyTheme = () => {
    if (!monaco) return;
    monaco.editor.setTheme(monacoTheme.value);
};

const applyLanguage = () => {
    if (!monaco || !model) return;
    const languageId = toMonacoLanguageId(props.language);
    monaco.editor.setModelLanguage(model, languageId);
};

const applyReadonly = () => {
    if (!editor) return;
    editor.updateOptions({ readOnly: props.readonly });
};

const layoutEditor = () => {
    if (!editor) return;
    editor.layout();
};

const getValue = (): string => {
    if (model) return model.getValue();
    return props.modelValue;
};

const setValue = (value: string): void => {
    if (!model) return;
    const next = value ?? "";
    if (model.getValue() === next) return;

    isApplyingExternalValue = true;
    model.setValue(next);
    isApplyingExternalValue = false;
};

const focus = (): void => {
    editor?.focus?.();
};

defineExpose({
    getValue,
    setValue,
    focus,
    layout: layoutEditor,
});

onMounted(async () => {
    if (!containerRef.value) return;

    monaco = await loadMonaco();

    model = monaco.editor.createModel(props.modelValue, toMonacoLanguageId(props.language));

    editor = monaco.editor.create(containerRef.value, {
        model,
        readOnly: props.readonly,
        automaticLayout: props.autoLayout,
        minimap: { enabled: false },
        scrollBeyondLastLine: false,
        fontSize: 14,
        tabSize: 4,
        insertSpaces: true,
        wordWrap: "on",
    });

    applyTheme();

    disposeContentListener = editor.onDidChangeModelContent(() => {
        if (isApplyingExternalValue) return;
        const value = model.getValue();
        emit("update:modelValue", value);
        emit("change", value);
    });

    // 某些布局场景（如切换 tab / 折叠容器）下，Monaco 的 automaticLayout 不一定能捕捉到尺寸变化。
    // 这里额外用 ResizeObserver 兜底。
    if (props.autoLayout && typeof ResizeObserver !== "undefined") {
        resizeObserver = new ResizeObserver(() => {
            layoutEditor();
        });
        resizeObserver.observe(containerRef.value);
    }

    emit("ready");
});

watch(
    () => props.modelValue,
    (newValue) => {
        if (!model) return;
        if (newValue === model.getValue()) return;
        setValue(newValue ?? "");
    }
);

watch(
    () => props.language,
    () => {
        applyLanguage();
    }
);

watch(
    () => props.readonly,
    () => {
        applyReadonly();
    }
);

watch(
    () => monacoTheme.value,
    () => {
        applyTheme();
    }
);

onBeforeUnmount(() => {
    disposeContentListener?.dispose?.();
    disposeContentListener = null;

    resizeObserver?.disconnect?.();
    resizeObserver = null;

    editor?.dispose?.();
    editor = null;

    model?.dispose?.();
    model = null;

    monaco = null;
});
</script>

<template>
    <div class="oj-code-editor" :style="{ height, width }">
        <div ref="containerRef" class="editor-container" />
    </div>
</template>

<style scoped>
.oj-code-editor {
    overflow: hidden;
    background: var(--oj-editor-bg);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
}

.editor-container {
    width: 100%;
    height: 100%;
}
</style>
