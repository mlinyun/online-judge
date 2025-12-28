<script setup lang="ts">
import { computed, ref, watch } from "vue";

import { selectStatusRecord } from "@/api/status";
import OjCodeEditor from "@/components/common/code-editor/index.vue";
import type { Api } from "@/types/api/api";

defineOptions({ name: "StatusRecordDetailDialog" });

type StatusRecordId = Api.Status.StatusRecordId;

type Props = {
    modelValue: boolean;
    statusRecordId?: StatusRecordId;
};

const props = defineProps<Props>();
const emit = defineEmits<{
    (e: "update:modelValue", v: boolean): void;
}>();

const visible = computed({
    get: () => props.modelValue,
    set: (v: boolean) => emit("update:modelValue", v),
});

const isMobile = ref(false);
const updateIsMobile = () => {
    isMobile.value = window.matchMedia("(width <= 640px)").matches;
};

const loading = ref(false);
const loadError = ref("");
const detail = ref<Api.Status.SelectStatusRecordResult | null>(null);

const compilerText = computed(() => String(detail.value?.CompilerInfo ?? "").trim());
const showCompiler = computed(() => !!compilerText.value);
const visibleTests = computed(() => (detail.value?.TestInfo ?? []).slice(0, 4));
const hasTests = computed(() => (detail.value?.TestInfo?.length ?? 0) > 0);

type StatusTagType = "success" | "info" | "warning" | "danger";

const getStatusTitle = (status: unknown): string => {
    const s = Number(status);
    if (!Number.isFinite(s)) return String(status ?? "");
    if (s === 0) return "Pending";
    if (s === 1) return "Compile Error";
    if (s === 2) return "Accepted";
    if (s === 3) return "Wrong Answer";
    if (s === 4) return "Runtime Error";
    if (s === 5) return "Time Limit Exceeded";
    if (s === 6) return "Memory Limit Exceeded";
    if (s === 7) return "System Error";
    return `Unknown (${s})`;
};

const getStatusTagType = (status: unknown): StatusTagType => {
    const s = Number(status);
    if (!Number.isFinite(s)) return "info";
    if (s === 0) return "info";
    if (s === 2) return "success";
    if (s === 3) return "danger";
    return "warning";
};

const statusTitle = computed(() => {
    const s = Number(detail.value?.Status ?? NaN);
    if (s === 0) return "Pending";
    if (s === 1) return "Compile Error";
    if (s === 2) return "Accepted";
    if (s === 3) return "Wrong Answer";
    if (s === 4) return "Runtime Error";
    if (s === 5) return "Time Limit Exceeded";
    if (s === 6) return "Memory Limit Exceeded";
    if (s === 7) return "System Error";
    return detail.value?.Status ? `Unknown (${detail.value.Status})` : "";
});

const statusAlertType = computed<"success" | "warning" | "error" | "info">(() => {
    const s = Number(detail.value?.Status ?? NaN);
    if (s === 0) return "info";
    if (s === 2) return "success";
    if (s === 3) return "error";
    if (Number.isNaN(s)) return "info";
    return "warning";
});

const fetchDetail = async (id: StatusRecordId) => {
    loading.value = true;
    loadError.value = "";
    detail.value = null;

    try {
        const res = await selectStatusRecord(id);
        if (res.data.code === 0 && res.data.data) {
            detail.value = res.data.data;
            return;
        }
        loadError.value = res.data.message || "加载失败";
    } catch (e) {
        console.error("获取状态记录详情失败:", e);
        loadError.value = "网络异常，请稍后重试";
    } finally {
        loading.value = false;
    }
};

const handleClose = () => {
    visible.value = false;
};

watch(
    () => visible.value,
    (v) => {
        if (!v) {
            loading.value = false;
            loadError.value = "";
            detail.value = null;
            return;
        }

        updateIsMobile();
        if (props.statusRecordId) void fetchDetail(props.statusRecordId);
    }
);

watch(
    () => props.statusRecordId,
    (id) => {
        if (!visible.value) return;
        if (!id) return;
        void fetchDetail(id);
    }
);

const editorHeight = computed(() => (isMobile.value ? "320px" : "440px"));
</script>

<template>
    <el-dialog v-model="visible" class="detail-dialog" title="测评详情" :fullscreen="isMobile"
        :width="isMobile ? '100%' : '860px'" :append-to-body="true" :lock-scroll="true"
        modal-class="detail-dialog-modal" destroy-on-close @close="handleClose">
        <el-skeleton :loading="loading" animated>
            <template #template>
                <div class="skeleton-block" />
                <div class="skeleton-block" />
                <div class="skeleton-block" />
            </template>

            <template #default>
                <el-empty v-if="loadError" :description="loadError">
                    <el-button type="primary" plain :disabled="!statusRecordId"
                        @click="statusRecordId && fetchDetail(statusRecordId)">
                        重试加载
                    </el-button>
                </el-empty>

                <template v-else-if="detail">
                    <div class="detail-body">
                        <el-alert class="status-alert" :type="statusAlertType" :title="statusTitle" :closable="false"
                            show-icon />

                        <div class="kv-row">
                            <div class="kv">
                                <span class="k">记录ID</span>
                                <span class="v mono" :title="String(detail._id)">{{ detail._id }}</span>
                            </div>
                            <div class="kv">
                                <span class="k">语言</span>
                                <span class="v">{{ detail.Language }}</span>
                            </div>
                        </div>

                        <div class="code-panel oj-glass-panel">
                            <div class="panel-head">
                                <div class="panel-title">提交的代码</div>
                            </div>
                            <OjCodeEditor :model-value="detail.Code" readonly :language="detail.Language"
                                :height="editorHeight" class="code-editor" />
                        </div>

                        <div v-if="showCompiler" class="compiler-panel oj-glass-panel">
                            <div class="panel-head">
                                <div class="panel-title">编译信息</div>
                            </div>
                            <pre class="compiler-pre">{{ compilerText }}</pre>
                        </div>

                        <div v-if="hasTests" class="tests-panel oj-glass-panel">
                            <div class="panel-head">
                                <div class="panel-title">测试点详情</div>
                            </div>
                            <el-collapse>
                                <el-collapse-item v-for="(info, index) in visibleTests" :key="index">
                                    <template #title>
                                        <div class="test-head">
                                            <div class="test-head-left">
                                                <span class="mono test-idx">#Test{{ index + 1 }}</span>
                                                <el-tag class="test-status" size="small" effect="light"
                                                    :type="getStatusTagType(info.Status)">
                                                    {{ getStatusTitle(info.Status) }}
                                                </el-tag>
                                            </div>

                                            <div class="test-head-right">
                                                <div class="mini-metric">
                                                    <div class="mk">运行时间</div>
                                                    <div class="mv mono">{{ info.RunTime || "-" }}</div>
                                                </div>
                                                <div class="mini-metric">
                                                    <div class="mk">内存使用</div>
                                                    <div class="mv mono">{{ info.RunMemory || "-" }}</div>
                                                </div>
                                            </div>
                                        </div>
                                    </template>

                                    <div class="test-body">
                                        <div class="test-block">
                                            <div class="test-k">标准输入</div>
                                            <pre class="test-pre">{{ info.StandardInput || "-" }}</pre>
                                        </div>

                                        <div class="test-io-row">
                                            <div class="test-block">
                                                <div class="test-k">期望输出</div>
                                                <pre class="test-pre">{{ info.StandardOutput || "-" }}</pre>
                                            </div>
                                            <div class="test-block">
                                                <div class="test-k">实际输出</div>
                                                <pre class="test-pre">{{ info.PersonalOutput || "-" }}</pre>
                                            </div>
                                        </div>
                                    </div>
                                </el-collapse-item>
                            </el-collapse>
                        </div>
                    </div>
                </template>

                <el-empty v-else description="暂无数据" />
            </template>
        </el-skeleton>
    </el-dialog>
</template>

<style scoped>
:global(.el-dialog.detail-dialog) {
    display: flex;
    flex-direction: column;
    height: min(84vh, 900px);
    max-height: min(84vh, 900px);
    overflow: hidden;
    border-radius: var(--oj-radius-2xl);
}

:global(.el-dialog.detail-dialog.is-fullscreen) {
    height: 100vh;
    max-height: 100vh;
    border-radius: 0;
}

:global(.el-dialog.detail-dialog .el-dialog__body) {
    flex: 1;
    min-height: 0;
    padding: var(--oj-spacing-5);
    overflow: auto;
}

.detail-body {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-5);
}

.skeleton-block {
    height: 44px;
    margin: var(--oj-spacing-2) 0;
    background: var(--oj-surface-hover);
    border-radius: var(--oj-radius-lg);
}

.status-alert {
    border-radius: var(--oj-radius-xl);
}

.kv-row {
    display: grid;
    grid-template-columns: repeat(2, minmax(0, 1fr));
    gap: var(--oj-spacing-4);
}

.kv {
    display: flex;
    gap: var(--oj-spacing-3);
    align-items: baseline;
    padding: var(--oj-spacing-4);
    background: var(--oj-glass-bg-light);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
}

.k {
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-muted);
}

.v {
    min-width: 0;
    overflow: hidden;
    text-overflow: ellipsis;
    color: var(--oj-text-color);
    white-space: nowrap;
}

.mono {
    font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace;
}

.panel-head {
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding-bottom: var(--oj-spacing-3);
    margin-bottom: var(--oj-spacing-3);
    border-bottom: 1px solid var(--oj-glass-border);
}

.panel-title {
    font-size: var(--oj-font-size-sm);
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
}

.code-panel,
.compiler-panel,
.tests-panel {
    padding: var(--oj-spacing-5);
    border-radius: var(--oj-radius-2xl);
}

.code-editor {
    min-height: 320px;
}

.compiler-pre {
    max-height: calc(1.6em * 8 + var(--oj-spacing-4) * 2);
    padding: var(--oj-spacing-4);
    margin: 0;
    overflow: auto;
    font-size: var(--oj-font-size-xs);
    line-height: 1.6;
    color: var(--oj-text-color-secondary);
    word-break: normal;
    overflow-wrap: anywhere;
    white-space: pre-wrap;
    background: var(--oj-glass-bg-light);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
}

.tests-panel :deep(.el-collapse) {
    border-top: 1px solid var(--oj-glass-border);
}

.tests-panel :deep(.el-collapse-item__content) {
    padding-bottom: var(--oj-spacing-4);
}

.test-head {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-3);
    align-items: center;
    justify-content: space-between;
    width: 100%;
    min-width: 0;
    padding-right: var(--oj-spacing-8);
}

.test-head-left {
    display: inline-flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    min-width: 0;
}

.test-idx {
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-color);
}

.test-status {
    white-space: nowrap;
}

.test-head-right {
    display: grid;
    grid-template-columns: repeat(2, max-content);
    grid-auto-flow: column;
    gap: var(--oj-spacing-5);
    align-items: center;
    justify-content: end;
}

.mini-metric {
    display: flex;
    flex-direction: column;
    gap: 2px;
    align-items: flex-start;
    justify-content: center;
}

.mk {
    font-size: var(--oj-font-size-xs);
    line-height: var(--oj-line-height-normal);
    color: var(--oj-text-color-muted);
}

.mv {
    font-size: var(--oj-font-size-sm);
    font-weight: normal;
    font-variant-numeric: tabular-nums;
    line-height: 1.2;
    color: var(--oj-text-color);
}

.test-body {
    display: grid;
    gap: var(--oj-spacing-4);
}

.test-io-row {
    display: grid;
    grid-template-columns: repeat(2, minmax(0, 1fr));
    gap: var(--oj-spacing-4);
}

.test-block {
    padding: var(--oj-spacing-4);
    background: var(--oj-glass-bg-light);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
}

.test-k {
    margin-bottom: var(--oj-spacing-2);
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-muted);
}

.test-pre {
    margin: 0;
    overflow: auto;
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-secondary);
    word-break: normal;
    overflow-wrap: anywhere;
    white-space: pre-wrap;
}

@media (width <=640px) {
    .test-head-right {
        gap: var(--oj-spacing-4);
    }

    .mv {
        font-size: var(--oj-font-size-sm);
    }

    .test-head {
        padding-right: var(--oj-spacing-7);
    }
}

@media (width <=640px) {
    :global(.el-dialog.detail-dialog .el-dialog__body) {
        max-height: none;
        padding: var(--oj-spacing-4);
    }

    .kv-row {
        grid-template-columns: 1fr;
    }

    .code-panel,
    .compiler-panel,
    .tests-panel {
        padding: var(--oj-spacing-4);
    }

    .code-editor {
        min-height: 260px;
    }
}

/* Prevent overlay container from scrolling the whole screen */
:global(.detail-dialog-modal) {
    overflow: hidden;
}

:global(.detail-dialog-modal.el-overlay-dialog) {
    overflow: hidden;
}

:global(.detail-dialog-modal .el-overlay-dialog) {
    overflow: hidden;
}
</style>
