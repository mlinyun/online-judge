<script setup lang="ts">
import { computed, ref, watch } from "vue";
import { useRoute, useRouter } from "vue-router";

import { ArrowLeft, Link } from "@element-plus/icons-vue";

import { selectStatusRecord } from "@/api/status";
import OjCodeEditor from "@/components/common/code-editor/index.vue";
import type { Api } from "@/types/api/api";

defineOptions({ name: "StatusRecordDetail" });

type StatusRecordId = Api.Status.StatusRecordId;

type StatusTagType = "success" | "info" | "warning" | "danger";

type MetaInfo = {
    problemId?: string;
    problemTitle?: string;
    userId?: string;
    userNickName?: string;
    submitTime?: string;
    runTime?: string;
    runMemory?: string;
    length?: string;
};

const route = useRoute();
const router = useRouter();

const statusRecordId = computed<StatusRecordId | undefined>(() => {
    const id = String(route.params.id ?? "").trim();
    return id ? (id as StatusRecordId) : undefined;
});

const meta = computed<MetaInfo>(() => {
    const query = route.query;
    return {
        problemId: query.problemId ? String(query.problemId) : undefined,
        problemTitle: query.problemTitle ? String(query.problemTitle) : undefined,
        userId: query.userId ? String(query.userId) : undefined,
        userNickName: query.userNickName ? String(query.userNickName) : undefined,
        submitTime: query.submitTime ? String(query.submitTime) : undefined,
        runTime: query.runTime ? String(query.runTime) : undefined,
        runMemory: query.runMemory ? String(query.runMemory) : undefined,
        length: query.length ? String(query.length) : undefined,
    };
});

const detailMeta = computed<MetaInfo>(() => {
    return {
        problemId: detail.value?.ProblemId ? String(detail.value.ProblemId) : undefined,
        problemTitle: detail.value?.ProblemTitle ? String(detail.value.ProblemTitle) : undefined,
        userId: detail.value?.UserId ? String(detail.value.UserId) : undefined,
        userNickName: detail.value?.UserNickName ? String(detail.value.UserNickName) : undefined,
        submitTime: detail.value?.SubmitTime ? String(detail.value.SubmitTime) : undefined,
        runTime: detail.value?.RunTime ? String(detail.value.RunTime) : undefined,
        runMemory: detail.value?.RunMemory ? String(detail.value.RunMemory) : undefined,
        length: detail.value?.Length ? String(detail.value.Length) : undefined,
    };
});

const displayMeta = computed<MetaInfo>(() => ({
    problemId: detailMeta.value.problemId || meta.value.problemId,
    problemTitle: detailMeta.value.problemTitle || meta.value.problemTitle,
    userId: detailMeta.value.userId || meta.value.userId,
    userNickName: detailMeta.value.userNickName || meta.value.userNickName,
    submitTime: detailMeta.value.submitTime || meta.value.submitTime,
    runTime: detailMeta.value.runTime || meta.value.runTime,
    runMemory: detailMeta.value.runMemory || meta.value.runMemory,
    length: detailMeta.value.length || meta.value.length,
}));

const loading = ref(false);
const loadError = ref("");
const detail = ref<Api.Status.SelectStatusRecordResult | null>(null);

const compilerText = computed(() => String(detail.value?.CompilerInfo ?? "").trim());
const showCompiler = computed(() => !!compilerText.value);
const hasTests = computed(() => (detail.value?.TestInfo?.length ?? 0) > 0);

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

const statusTitle = computed(() => getStatusTitle(detail.value?.Status));
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

const handleBack = () => {
    if (window.history.length > 1) {
        router.back();
        return;
    }
    void router.push({ name: "status-record" });
};

const handleGoProblem = () => {
    if (!displayMeta.value.problemId) return;
    void router.push({ name: "problem-detail", params: { id: displayMeta.value.problemId } });
};

watch(
    () => statusRecordId.value,
    (id) => {
        if (!id) {
            loadError.value = "缺少状态记录 ID";
            detail.value = null;
            return;
        }
        void fetchDetail(id);
    },
    { immediate: true }
);
</script>

<template>
    <section class="status-detail-page" aria-label="提交详情">
        <header class="page-header oj-glass-panel">
            <div class="header-left">
                <el-button :icon="ArrowLeft" @click="handleBack">返回</el-button>
                <div class="title-group">
                    <div class="title">提交详情</div>
                    <div class="subtitle">StatusRecordId: {{ statusRecordId || "-" }}</div>
                </div>
            </div>

            <div class="header-right">
                <el-button v-if="displayMeta.problemId" :icon="Link" type="primary" plain @click="handleGoProblem">
                    查看题目
                </el-button>
            </div>
        </header>

        <el-skeleton :loading="loading" animated>
            <template #template>
                <div class="skeleton-block" />
                <div class="skeleton-block" />
                <div class="skeleton-block" />
            </template>

            <template #default>
                <el-empty v-if="loadError" :description="loadError">
                    <el-button
                        type="primary"
                        plain
                        :disabled="!statusRecordId"
                        @click="statusRecordId && fetchDetail(statusRecordId)"
                    >
                        重试加载
                    </el-button>
                </el-empty>

                <template v-else-if="detail">
                    <div class="detail-body">
                        <el-alert
                            class="status-alert"
                            :type="statusAlertType"
                            :title="statusTitle"
                            :closable="false"
                            show-icon
                        />

                        <div class="kv-grid">
                            <div class="kv">
                                <span class="k">记录ID</span>
                                <span class="v mono" :title="String(detail._id)">{{ detail._id }}</span>
                            </div>
                            <div class="kv">
                                <span class="k">语言</span>
                                <span class="v">{{ detail.Language || "-" }}</span>
                            </div>
                            <div class="kv">
                                <span class="k">提交时间</span>
                                <span class="v">{{ displayMeta.submitTime || "-" }}</span>
                            </div>
                            <div class="kv">
                                <span class="k">运行时间</span>
                                <span class="v mono">{{ displayMeta.runTime || "-" }}</span>
                            </div>
                            <div class="kv">
                                <span class="k">运行内存</span>
                                <span class="v mono">{{ displayMeta.runMemory || "-" }}</span>
                            </div>
                            <div class="kv">
                                <span class="k">代码长度</span>
                                <span class="v mono">{{ displayMeta.length || "-" }}</span>
                            </div>
                            <div class="kv">
                                <span class="k">用户</span>
                                <span class="v">{{ displayMeta.userNickName || displayMeta.userId || "-" }}</span>
                            </div>
                            <div class="kv">
                                <span class="k">题目</span>
                                <span class="v">{{ displayMeta.problemTitle || displayMeta.problemId || "-" }}</span>
                            </div>
                        </div>

                        <div class="code-panel oj-glass-panel">
                            <div class="panel-head">
                                <div class="panel-title">提交的代码</div>
                            </div>
                            <OjCodeEditor
                                :model-value="detail.Code"
                                readonly
                                :language="detail.Language"
                                height="520px"
                                class="code-editor"
                            />
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
                                <el-collapse-item v-for="(info, index) in detail.TestInfo" :key="index">
                                    <template #title>
                                        <div class="test-head">
                                            <div class="test-head-left">
                                                <span class="mono test-idx">#Test{{ index + 1 }}</span>
                                                <el-tag
                                                    class="test-status"
                                                    size="small"
                                                    effect="light"
                                                    :type="getStatusTagType(info.Status)"
                                                >
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
    </section>
</template>

<style scoped>
.status-detail-page {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-6);
    min-height: 0;
    padding: var(--oj-spacing-4);
}

.page-header {
    display: flex;
    align-items: center;
    justify-content: space-between;
    gap: var(--oj-spacing-4);
    padding: var(--oj-spacing-4) var(--oj-spacing-5);
    border-radius: var(--oj-radius-2xl);
}

.header-left {
    display: flex;
    align-items: center;
    gap: var(--oj-spacing-4);
    min-width: 0;
}

.title-group {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-1);
    min-width: 0;
}

.title {
    font-size: var(--oj-font-size-xl);
    font-weight: var(--oj-font-weight-bold);
    color: var(--oj-text-color);
}

.subtitle {
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-muted);
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

.kv-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
    gap: var(--oj-spacing-4);
}

.kv {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-2);
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
    max-height: calc(1.6em * 10 + var(--oj-spacing-4) * 2);
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
    gap: var(--oj-spacing-3);
    align-items: center;
}

.mini-metric {
    display: flex;
    flex-direction: column;
    gap: 2px;
    text-align: right;
}

.mk {
    font-size: var(--oj-font-size-2xs);
    color: var(--oj-text-color-muted);
}

.mv {
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color);
}

.test-body {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-3);
}

.test-io-row {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
    gap: var(--oj-spacing-3);
}

.test-block {
    padding: var(--oj-spacing-3);
    background: var(--oj-surface-hover);
    border-radius: var(--oj-radius-lg);
}

.test-k {
    margin-bottom: var(--oj-spacing-2);
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-muted);
}

.test-pre {
    margin: 0;
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color);
    white-space: pre-wrap;
    word-break: break-word;
}

@media (max-width: 720px) {
    .page-header {
        flex-direction: column;
        align-items: flex-start;
    }

    .header-left {
        width: 100%;
    }

    .header-right {
        width: 100%;
        display: flex;
        justify-content: flex-end;
    }
}
</style>
