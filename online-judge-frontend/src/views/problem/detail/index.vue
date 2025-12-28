<script setup lang="ts">
import { computed, onMounted, ref } from "vue";
import { useRoute, useRouter } from "vue-router";
import { ArrowLeft, ChatRound, Document, PriceTag, Timer, Upload, View } from "@element-plus/icons-vue";
import { ElMessage } from "element-plus";

import { selectProblemInfo } from "@/api/problem";
import { submitJudgeCode } from "@/api/judge";
import { OjCodeEditor, OjMarkdownPreview } from "@/components/common";
import type { Api } from "@/types/api/api";

import ProblemRelatedDrawer from "./components/problem-related-drawer.vue";
import ProblemStatusRecordDrawer from "./components/problem-status-record-drawer.vue";

defineOptions({ name: "ProblemDetail" });

type ProblemDetailInfo = Api.Problem.SelectProblemResult;
type SubmitResult = Api.Judge.SubmitJudgeCodeResult;

const route = useRoute();
const router = useRouter();

const problemId = computed(() => String(route.params.id || ""));

const loading = ref(false);
const loadError = ref("");
const detail = ref<ProblemDetailInfo | null>(null);

const code = ref<string>(
    "#include <bits/stdc++.h>\nusing namespace std;\n\nint main() {\n    ios::sync_with_stdio(false);\n    cin.tie(nullptr);\n\n    // TODO: write your solution\n\n    return 0;\n}\n"
);

const language = ref<string>("C++");
const languageOptions = [
    { label: "C", value: "C" },
    { label: "C++", value: "C++" },
    { label: "Go", value: "Go" },
    { label: "Java", value: "Java" },
    { label: "Python2", value: "Python2" },
    { label: "Python3", value: "Python3" },
    { label: "JavaScript", value: "JavaScript" },
    { label: "TypeScript", value: "TypeScript" },
];

const editorLanguage = computed(() => {
    // OjCodeEditor 内部会做 toMonacoLanguageId 映射，这里保持和后端 Language 字符串一致即可
    return language.value;
});

const lastSubmit = ref<SubmitResult | null>(null);
const submitting = ref(false);

const requestSeq = ref(0);

const relatedDrawerOpen = ref(false);
const relatedKind = ref<"solution" | "discussion">("solution");
const statusDrawerOpen = ref(false);

const getStatusTitle = (status: number): string => {
    if (status === 0) return "Pending";
    if (status === 1) return "Compile Error";
    if (status === 2) return "Accepted";
    if (status === 3) return "Wrong Answer";
    if (status === 4) return "Runtime Error";
    if (status === 5) return "Time Limit Exceeded";
    if (status === 6) return "Memory Limit Exceeded";
    if (status === 7) return "System Error";
    return `Unknown (${status})`;
};

const fetchDetail = async () => {
    const id = problemId.value.trim();
    if (!id) {
        loadError.value = "缺少题目 ID";
        detail.value = null;
        return;
    }

    const seq = ++requestSeq.value;
    loading.value = true;
    loadError.value = "";

    try {
        const res = await selectProblemInfo(id as Api.Problem.ProblemId);
        if (seq !== requestSeq.value) return;

        if (res.data.code === 0 && res.data.data) {
            detail.value = res.data.data;
            return;
        }

        detail.value = null;
        loadError.value = res.data.message || "加载失败";
    } catch (err) {
        console.error("获取题目详情失败:", err);
        if (seq !== requestSeq.value) return;
        detail.value = null;
        loadError.value = "网络异常，请稍后重试";
    } finally {
        if (seq === requestSeq.value) loading.value = false;
    }
};

const handleBack = async () => {
    // 优先回到题库列表（用户习惯）
    await router.push({ name: "problem-list" });
};

const handleSubmit = async () => {
    const id = problemId.value.trim();
    if (!id) {
        ElMessage.error("缺少题目 ID");
        return;
    }

    const source = String(code.value || "").trim();
    if (!source) {
        ElMessage.warning("请先输入代码");
        return;
    }

    submitting.value = true;
    try {
        const res = await submitJudgeCode({
            ProblemId: id as Api.Problem.ProblemId,
            Language: language.value,
            Code: source,
        });

        if (res.data.code === 0 && res.data.data) {
            lastSubmit.value = res.data.data;
            const title = getStatusTitle(res.data.data.Status);
            if (res.data.data.Status === 2) ElMessage.success(`提交成功：${title}`);
            else ElMessage.warning(`提交成功：${title}`);
            return;
        }

        ElMessage.error(res.data.message || "提交失败");
    } catch (err) {
        console.error("提交判题失败:", err);
        ElMessage.error("网络异常，请稍后重试");
    } finally {
        submitting.value = false;
    }
};

onMounted(() => {
    void fetchDetail();
});

const handleOpenSolutions = () => {
    relatedKind.value = "solution";
    relatedDrawerOpen.value = true;
};

const handleOpenDiscussions = () => {
    relatedKind.value = "discussion";
    relatedDrawerOpen.value = true;
};

const handleOpenStatusRecords = () => {
    statusDrawerOpen.value = true;
};
</script>

<template>
    <section class="problem-detail-page" aria-label="题目详情">
        <!-- Compact Navigation (in-page, JudgeLayout is minimal) -->
        <header class="topbar oj-glass-panel">
            <button class="back" type="button" @click="handleBack">
                <el-icon class="icon">
                    <ArrowLeft />
                </el-icon>
                <span class="brand">LYOJ</span>
            </button>

            <div class="crumb">
                <span class="pid mono">{{ detail?._id || problemId }}</span>
                <span class="dot" />
                <span class="ptitle" :title="detail?.Title || ''">{{ detail?.Title || "题目详情" }}</span>
            </div>
        </header>

        <!-- Split Workspace -->
        <div class="workspace">
            <!-- Left: Problem Description -->
            <div class="pane left oj-glass-panel">
                <div class="tabs">
                    <div class="tab is-active">
                        <el-icon class="tab-icon">
                            <Document />
                        </el-icon>
                        题目描述
                    </div>

                    <div class="tab-actions">
                        <el-button size="small" :icon="Document" @click="handleOpenSolutions">题解</el-button>
                        <el-button size="small" :icon="ChatRound" @click="handleOpenDiscussions">讨论</el-button>
                        <el-button size="small" :icon="View" @click="handleOpenStatusRecords">提交记录</el-button>
                    </div>
                </div>

                <div class="content-scroll">
                    <el-skeleton :loading="loading" animated>
                        <template #template>
                            <div class="skeleton-title" />
                            <div class="skeleton-meta" />
                            <div class="skeleton-block" />
                            <div class="skeleton-block" />
                        </template>

                        <template #default>
                            <div v-if="loadError" class="state">
                                <el-result icon="error" title="加载失败" :sub-title="loadError">
                                    <template #extra>
                                        <el-button type="primary" @click="fetchDetail">重试</el-button>
                                    </template>
                                </el-result>
                            </div>

                            <div v-else-if="detail" class="problem">
                                <div class="problem-header">
                                    <h1 class="title">{{ detail.Title }}</h1>

                                    <div class="meta-row">
                                        <div class="meta-item">
                                            <el-icon class="meta-icon">
                                                <Timer />
                                            </el-icon>
                                            <span class="meta-label">Time</span>
                                            <span class="meta-value">{{ detail.TimeLimit }} ms</span>
                                        </div>
                                        <div class="meta-item">
                                            <span class="meta-label">Memory</span>
                                            <span class="meta-value">{{ detail.MemoryLimit }} MB</span>
                                        </div>
                                        <div class="meta-item">
                                            <span class="meta-label">Tests</span>
                                            <span class="meta-value">{{ detail.JudgeNum }}</span>
                                        </div>
                                        <div class="meta-item">
                                            <span class="meta-label">AC</span>
                                            <span class="meta-value">{{ detail.ACNum }}</span>
                                        </div>
                                        <div class="meta-item">
                                            <span class="meta-label">Submit</span>
                                            <span class="meta-value">{{ detail.SubmitNum }}</span>
                                        </div>
                                    </div>

                                    <div v-if="detail.Tags?.length" class="tag-row">
                                        <el-icon class="tag-icon">
                                            <PriceTag />
                                        </el-icon>
                                        <el-tag v-for="t in detail.Tags" :key="t" size="small" effect="light" round>
                                            {{ t }}
                                        </el-tag>
                                    </div>
                                </div>

                                <OjMarkdownPreview class="md" :content="detail.Description" />
                            </div>

                            <div v-else class="state">
                                <el-empty description="暂无数据" />
                            </div>
                        </template>
                    </el-skeleton>
                </div>
            </div>

            <!-- Right: Code Editor (no console) -->
            <div class="pane right">
                <div class="editor-shell oj-glass-panel">
                    <div class="editor-toolbar">
                        <div class="toolbar-left">
                            <el-select v-model="language" class="lang" placeholder="语言"
                                popper-class="oj-problem-detail-popper">
                                <el-option v-for="opt in languageOptions" :key="opt.value" :label="opt.label"
                                    :value="opt.value" />
                            </el-select>
                        </div>

                        <div class="toolbar-right">
                            <el-button type="primary" :icon="Upload" :loading="submitting" @click="handleSubmit">
                                提交
                            </el-button>
                        </div>
                    </div>

                    <div class="editor-area">
                        <OjCodeEditor v-model="code" :language="editorLanguage" height="100%" />
                    </div>

                    <div v-if="lastSubmit" class="result-bar">
                        <div class="result-left">
                            <span class="result-label">结果</span>
                            <span class="result-value">{{ getStatusTitle(lastSubmit.Status) }}</span>
                            <span class="result-id mono">#{{ lastSubmit.StatusRecordId }}</span>
                        </div>
                        <div class="result-right">
                            <el-tag size="small" effect="dark" :type="lastSubmit.IsFirstAC ? 'success' : 'info'">
                                {{ lastSubmit.IsFirstAC ? "First AC" : "Submitted" }}
                            </el-tag>
                        </div>
                    </div>
                </div>

                <div v-if="lastSubmit?.CompilerInfo" class="compiler oj-glass-panel">
                    <div class="compiler-title">Compiler Info</div>
                    <pre class="compiler-pre">{{ lastSubmit.CompilerInfo }}</pre>
                </div>
            </div>
        </div>

        <ProblemRelatedDrawer v-model="relatedDrawerOpen" :kind="relatedKind"
            :problem-id="(problemId.trim() as Api.Problem.ProblemId)" />

        <ProblemStatusRecordDrawer v-model="statusDrawerOpen"
            :problem-id="(problemId.trim() as Api.Problem.ProblemId)" />
    </section>
</template>

<style scoped>
.problem-detail-page {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-4);
    height: 100vh;
    padding: var(--oj-spacing-4);
}

.topbar {
    display: flex;
    align-items: center;
    justify-content: flex-start;
    height: 56px;
    padding: 0 var(--oj-spacing-4);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
}

.back {
    display: inline-flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    color: var(--oj-text-color-secondary);
    cursor: pointer;
    background: transparent;
    border: none;
    transition: color 0.15s ease;
}

.back:hover {
    color: var(--oj-text-color);
}

.brand {
    font-weight: var(--oj-font-weight-bold);
}

.crumb {
    display: inline-flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    min-width: 0;
    margin-left: var(--oj-spacing-8);
}

.mono {
    font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace;
}

.pid {
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-color-primary);
}

.dot {
    width: 6px;
    height: 6px;
    background: rgb(var(--oj-color-primary-rgb) / 45%);
    border-radius: 999px;
}

.ptitle {
    overflow: hidden;
    text-overflow: ellipsis;
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
    white-space: nowrap;
}

.workspace {
    display: flex;
    flex: 1;
    gap: var(--oj-spacing-4);
    min-height: 0;
}

.pane {
    min-width: 0;
    min-height: 0;
}

.left {
    display: flex;
    flex: 1;
    flex-direction: column;
    overflow: hidden;
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
}

.tabs {
    display: flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    justify-content: space-between;
    padding: var(--oj-spacing-3) var(--oj-spacing-4);
    background: rgb(0 0 0 / 6%);
    border-bottom: 1px solid var(--oj-glass-border);
}

.tab-actions {
    display: flex;
    flex: 0 0 auto;
    gap: var(--oj-spacing-2);
    align-items: center;
}

.tab {
    display: inline-flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    padding: var(--oj-spacing-2) var(--oj-spacing-3);
    font-size: var(--oj-font-size-sm);
    font-weight: var(--oj-font-weight-medium);
    color: var(--oj-text-color-secondary);
    cursor: default;
    background: transparent;
    border: 1px solid transparent;
    border-radius: var(--oj-radius-lg);
}

.tab.is-active {
    color: var(--oj-color-primary);
    background: rgb(var(--oj-color-primary-rgb) / 10%);
    border-color: rgb(var(--oj-color-primary-rgb) / 18%);
}

.content-scroll {
    flex: 1;
    min-height: 0;
    padding: var(--oj-spacing-5);
    overflow: auto;
}

.problem-header {
    margin-bottom: var(--oj-spacing-5);
}

.problem-header .title {
    margin: 0;
    font-size: var(--oj-font-size-2xl);
    font-weight: var(--oj-font-weight-bold);
    color: var(--oj-text-color);
}

.meta-row {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-3);
    margin-top: var(--oj-spacing-3);
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-secondary);
}

.meta-item {
    display: inline-flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    padding: var(--oj-spacing-2) var(--oj-spacing-3);
    background: rgb(var(--oj-color-primary-rgb) / 6%);
    border: 1px solid rgb(var(--oj-color-primary-rgb) / 12%);
    border-radius: var(--oj-radius-lg);
}

.meta-label {
    text-transform: uppercase;
    letter-spacing: var(--oj-letter-spacing-wide);
    opacity: 0.9;
}

.meta-value {
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
}

.tag-row {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-2);
    align-items: center;
    margin-top: var(--oj-spacing-3);
}

.tag-icon {
    color: var(--oj-text-color-secondary);
}

.md {
    color: var(--oj-text-color);
}

.right {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-4);
    width: min(720px, 48%);
}

.editor-shell {
    display: flex;
    flex: 1;
    flex-direction: column;
    min-height: 0;
    overflow: hidden;
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
}

.editor-toolbar {
    display: flex;
    gap: var(--oj-spacing-3);
    align-items: center;
    justify-content: space-between;
    padding: var(--oj-spacing-3) var(--oj-spacing-4);
    background: rgb(0 0 0 / 8%);
    border-bottom: 1px solid var(--oj-glass-border);
}

.lang {
    width: 220px;
    max-width: 100%;
}

.editor-area {
    flex: 1;
    min-height: 0;
}

.result-bar {
    display: flex;
    gap: var(--oj-spacing-3);
    align-items: center;
    justify-content: space-between;
    padding: var(--oj-spacing-3) var(--oj-spacing-4);
    background: rgb(0 0 0 / 6%);
    border-top: 1px solid var(--oj-glass-border);
}

.result-left {
    display: inline-flex;
    gap: var(--oj-spacing-3);
    align-items: center;
    min-width: 0;
}

.result-label {
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-secondary);
}

.result-value {
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
}

.result-id {
    overflow: hidden;
    text-overflow: ellipsis;
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-secondary);
    white-space: nowrap;
}

.compiler {
    padding: var(--oj-spacing-4);
    overflow: hidden;
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
}

.compiler-title {
    margin-bottom: var(--oj-spacing-3);
    font-size: var(--oj-font-size-sm);
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
}

.compiler-pre {
    max-height: 220px;
    padding: var(--oj-spacing-3);
    margin: 0;
    overflow: auto;
    font-size: var(--oj-font-size-xs);
    line-height: var(--oj-line-height-relaxed);
    color: var(--oj-text-color);
    background: var(--oj-editor-bg);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-lg);
}

.state {
    padding: var(--oj-spacing-6) 0;
}

.skeleton-title {
    height: 28px;
    margin-bottom: var(--oj-spacing-4);
    background: var(--oj-surface-overlay);
    border-radius: var(--oj-radius-lg);
}

.skeleton-meta {
    height: 20px;
    margin-bottom: var(--oj-spacing-4);
    background: var(--oj-surface-overlay);
    border-radius: var(--oj-radius-lg);
}

.skeleton-block {
    height: 120px;
    margin-bottom: var(--oj-spacing-4);
    background: var(--oj-surface-overlay);
    border-radius: var(--oj-radius-lg);
}

@media (width <=1024px) {
    .problem-detail-page {
        height: auto;
        min-height: 100vh;
    }

    .workspace {
        flex-direction: column;
    }

    .right {
        width: 100%;
    }
}

/* Select dropdown (popper teleports to body) */
:global(.oj-problem-detail-popper) {
    --el-color-primary: rgb(var(--oj-color-primary-rgb));
}

:global(.oj-problem-detail-popper .el-select-dropdown__item.is-selected) {
    color: rgb(var(--oj-color-primary-rgb));
}

:global(.oj-problem-detail-popper .el-select-dropdown__item:hover) {
    color: rgb(var(--oj-color-primary-rgb));
}
</style>
