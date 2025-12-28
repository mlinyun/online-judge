<script setup lang="ts">
import { computed, onMounted, ref, watch } from "vue";
import { Document, Search, User } from "@element-plus/icons-vue";

import { selectStatusRecordList } from "@/api/status";
import { DateUtils } from "@/utils/date/date-utils";
import type { Api } from "@/types/api/api";
import type { TableInstance } from "element-plus";

import StatusRecordDetailDialog from "./components/status-record-detail-dialog.vue";

defineOptions({ name: "StatusRecord" });

type StatusRow = Api.Status.SelectStatusRecordListItem;
type StatusTagType = "success" | "warning" | "danger" | "info";

const loading = ref(false);
const loadError = ref("");

let debounceTimer: number | undefined;
const debounce = (fn: () => void, delay = 300) => {
    if (debounceTimer) window.clearTimeout(debounceTimer);
    debounceTimer = window.setTimeout(() => fn(), delay);
};

const page = ref(1);
const pageSize = ref(20);
const total = ref(0);

const records = ref<StatusRow[]>([]);
// 表格布局方式
const tableLayout = ref<TableInstance["tableLayout"]>("fixed");

const detailDialogOpen = ref(false);
const activeStatusRecordId = ref<Api.Status.StatusRecordId | undefined>(undefined);

// Toolbar: SearchInfo
const searchUserId = ref("");
const searchProblemId = ref("");
const searchProblemTitle = ref("");
const searchStatus = ref<"all" | number>("all");
const searchLanguage = ref<"all" | string>("all");

const hasSearch = computed(() => {
    return (
        !!searchUserId.value.trim() ||
        !!searchProblemId.value.trim() ||
        !!searchProblemTitle.value.trim() ||
        searchStatus.value !== "all" ||
        searchLanguage.value !== "all"
    );
});

const buildSearchInfo = (): Api.Status.SelectStatusRecordListParams["SearchInfo"] | undefined => {
    const UserId = searchUserId.value.trim();
    const ProblemId = searchProblemId.value.trim();
    const ProblemTitle = searchProblemTitle.value.trim();
    const Language = searchLanguage.value === "all" ? "" : String(searchLanguage.value).trim();
    const Status = searchStatus.value === "all" ? undefined : searchStatus.value;

    if (!UserId && !ProblemId && !ProblemTitle && !Language && Status === undefined) return undefined;

    return {
        ...(UserId ? { UserId } : {}),
        ...(ProblemId ? { ProblemId } : {}),
        ...(ProblemTitle ? { ProblemTitle } : {}),
        ...(Status !== undefined ? { Status } : {}),
        ...(Language ? { Language } : {}),
    };
};

const requestSeq = ref(0);

const fetchRecords = async () => {
    const seq = ++requestSeq.value;
    loading.value = true;
    loadError.value = "";

    try {
        const SearchInfo = buildSearchInfo();
        const response = await selectStatusRecordList({
            Page: page.value,
            PageSize: pageSize.value,
            ...(SearchInfo ? { SearchInfo } : {}),
        });

        if (seq !== requestSeq.value) return;

        if (response.data.code === 0 && response.data.data) {
            records.value = (response.data.data.List || []) as StatusRow[];
            total.value = response.data.data.Total || 0;
            return;
        }

        records.value = [];
        total.value = 0;
        loadError.value = response.data.message || "加载失败";
    } catch (err) {
        console.error("获取状态记录列表失败:", err);
        records.value = [];
        total.value = 0;
        loadError.value = "网络异常，请稍后重试";
    } finally {
        if (seq === requestSeq.value) loading.value = false;
    }
};

const handlePageChange = (newPage: number) => {
    page.value = newPage;
};

const handleSizeChange = (newSize: number) => {
    pageSize.value = newSize;
    page.value = 1;
};

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

const getStatusTagType = (status: number): StatusTagType => {
    if (status === 0) return "info";
    if (status === 2) return "success";
    if (status === 3) return "danger";
    return "warning";
};

watch([page, pageSize], () => {
    fetchRecords();
});

watch(
    [searchUserId, searchProblemId, searchProblemTitle, searchStatus, searchLanguage],
    () => {
        if (page.value !== 1) {
            page.value = 1;
            return;
        }
        debounce(() => {
            fetchRecords();
        });
    },
    { deep: false }
);

onMounted(() => {
    fetchRecords();
});

const handleRowClick = (row: StatusRow) => {
    activeStatusRecordId.value = row._id;
    detailDialogOpen.value = true;
};
</script>

<template>
    <section class="status-record-page" aria-label="测评记录">
        <!-- Toolbar -->
        <div class="toolbar oj-glass-panel">
            <div class="toolbar-left">
                <el-input v-model="searchUserId" class="toolbar-field" :prefix-icon="User" clearable
                    placeholder="用户ID (UserId)" />

                <el-input v-model="searchProblemId" class="toolbar-field" :prefix-icon="Document" clearable
                    placeholder="题目ID (ProblemId)" />

                <el-input v-model="searchProblemTitle" class="toolbar-field" :prefix-icon="Search" clearable
                    placeholder="题目标题 (ProblemTitle)" />

                <el-select v-model="searchStatus" class="toolbar-field toolbar-select" placeholder="判题状态 (Status)"
                    popper-class="oj-status-record-popper">
                    <el-option label="全部" value="all" />
                    <el-option label="Pending" :value="0" />
                    <el-option label="Compile Error" :value="1" />
                    <el-option label="Accepted" :value="2" />
                    <el-option label="Wrong Answer" :value="3" />
                    <el-option label="Runtime Error" :value="4" />
                    <el-option label="Time Limit Exceeded" :value="5" />
                    <el-option label="Memory Limit Exceeded" :value="6" />
                    <el-option label="System Error" :value="7" />
                </el-select>

                <el-select v-model="searchLanguage" class="toolbar-field toolbar-select" placeholder="语言 (Language)"
                    popper-class="oj-status-record-popper">
                    <el-option label="全部" value="all" />
                    <el-option label="C" value="C" />
                    <el-option label="C++" value="C++" />
                    <el-option label="Go" value="Go" />
                    <el-option label="Java" value="Java" />
                    <el-option label="Python2" value="Python2" />
                    <el-option label="Python3" value="Python3" />
                    <el-option label="JavaScript" value="JavaScript" />
                </el-select>
            </div>

            <div class="toolbar-right">
                <div class="meta-chip" :class="{ 'is-active': hasSearch }">
                    <span class="meta-k">Total</span>
                    <span class="meta-v">{{ total }}</span>
                </div>
            </div>
        </div>

        <!-- Table -->
        <div class="table-panel oj-glass-panel">
            <el-skeleton :loading="loading" animated>
                <template #template>
                    <div class="skeleton-block" />
                    <div class="skeleton-block" />
                    <div class="skeleton-block" />
                </template>

                <template #default>
                    <el-empty v-if="!records.length && !loadError" :description="hasSearch ? '暂无匹配的测评记录' : '暂无测评记录'" />

                    <el-empty v-else-if="!records.length && loadError" :description="loadError">
                        <el-button type="primary" plain @click="fetchRecords">重试</el-button>
                    </el-empty>

                    <div v-else>
                        <el-table :data="records" class="record-table" :table-layout="tableLayout"
                            @row-click="handleRowClick">
                            <el-table-column label="记录ID" min-width="180">
                                <template #default="scope">
                                    <span class="mono id-text" :title="String(scope.row._id)">{{ scope.row._id }}</span>
                                </template>
                            </el-table-column>

                            <el-table-column label="用户" min-width="180">
                                <template #default="scope">
                                    <div class="entity-cell">
                                        <div class="entity-main">{{ scope.row.UserNickName }}</div>
                                        <div class="entity-sub mono">ID: {{ scope.row.UserId }}</div>
                                    </div>
                                </template>
                            </el-table-column>

                            <el-table-column label="题目" min-width="220">
                                <template #default="scope">
                                    <div class="entity-cell">
                                        <div class="entity-main content-clamp" :title="scope.row.ProblemTitle">
                                            {{ scope.row.ProblemTitle }}
                                        </div>
                                        <div class="entity-sub mono">ID: {{ scope.row.ProblemId }}</div>
                                    </div>
                                </template>
                            </el-table-column>

                            <el-table-column label="状态" width="160" align="center">
                                <template #default="scope">
                                    <el-tag size="small" :type="getStatusTagType(scope.row.Status)"
                                        :title="`Status=${scope.row.Status}`">
                                        {{ getStatusTitle(scope.row.Status) }}
                                    </el-tag>
                                </template>
                            </el-table-column>

                            <el-table-column prop="Language" label="语言" align="center" />

                            <el-table-column prop="RunTime" label="运行时间" align="center">
                                <template #default="scope">
                                    <span class="metric">{{ scope.row.RunTime }}</span>
                                </template>
                            </el-table-column>

                            <el-table-column prop="RunMemory" label="运行内存" align="center">
                                <template #default="scope">
                                    <span class="metric">{{ scope.row.RunMemory }}</span>
                                </template>
                            </el-table-column>

                            <el-table-column prop="Length" label="代码长度" align="center">
                                <template #default="scope">
                                    <span class="metric">{{ scope.row.Length }}</span>
                                </template>
                            </el-table-column>

                            <el-table-column label="提交时间" min-width="150" align="center">
                                <template #default="scope">
                                    <span class="time-text">{{ DateUtils.formatDateTime(scope.row.SubmitTime) }}</span>
                                </template>
                            </el-table-column>
                        </el-table>

                        <div class="pagination-bar">
                            <el-pagination v-model:current-page="page" v-model:page-size="pageSize"
                                :page-sizes="[10, 20, 40, 60]" :total="total"
                                layout="total, sizes, prev, pager, next, jumper" @current-change="handlePageChange"
                                @size-change="handleSizeChange" />
                        </div>
                    </div>
                </template>
            </el-skeleton>
        </div>

        <StatusRecordDetailDialog v-model="detailDialogOpen" :status-record-id="activeStatusRecordId" />
    </section>
</template>

<style scoped>
.status-record-page {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-6);
}

/* Toolbar */
.toolbar {
    position: relative;
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-4);
    align-items: center;
    justify-content: space-between;
    padding: var(--oj-spacing-4);
    margin-top: var(--oj-spacing-8);
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
:global(.oj-status-record-popper) {
    --el-color-primary: rgb(var(--oj-color-primary-rgb));
}

:global(.oj-status-record-popper .el-select-dropdown__item.is-selected) {
    color: rgb(var(--oj-color-primary-rgb));
}

:global(.oj-status-record-popper .el-select-dropdown__item:hover) {
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

/* Table panel */
.table-panel {
    overflow: hidden;
    border-radius: var(--oj-radius-xl);
}

.skeleton-block {
    height: 44px;
    margin: var(--oj-spacing-4);
    border-radius: var(--oj-radius-lg);
}

.record-table :deep(.el-table__inner-wrapper::before) {
    background-color: transparent;
}

.record-table :deep(.el-table__header-wrapper th.el-table__cell) {
    color: var(--oj-text-color-muted);
    background: var(--oj-table-header-bg);
    border-bottom: 1px solid var(--oj-glass-border);
}

.record-table :deep(.el-table__body tr:hover > td.el-table__cell) {
    background: var(--oj-table-row-hover-bg);
}

.record-table :deep(.el-table__row) {
    cursor: pointer;
}

.record-table :deep(td.el-table__cell) {
    border-bottom: 1px solid var(--oj-glass-border);
}

.mono {
    font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace;
}

.id-text {
    display: inline-block;
    max-width: 100%;
    overflow: hidden;
    text-overflow: ellipsis;
    vertical-align: bottom;
    color: var(--oj-text-color-secondary);
    white-space: nowrap;
}

.entity-cell {
    display: flex;
    flex-direction: column;
    gap: 2px;
    min-width: 0;
}

.entity-main {
    min-width: 0;
    font-weight: var(--oj-font-weight-medium);
    color: var(--oj-text-color);
}

.entity-sub {
    overflow: hidden;
    text-overflow: ellipsis;
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-secondary);
    white-space: nowrap;
}

.content-clamp {
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
}

.time-text {
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-secondary);
}

.metric {
    font-variant-numeric: tabular-nums;
    color: var(--oj-text-color-secondary);
}

.pagination-bar {
    display: flex;
    justify-content: flex-end;
    padding: var(--oj-spacing-4);
    background: var(--oj-table-header-bg);
    border-top: 1px solid var(--oj-glass-border);
}

@media (width <=640px) {
    .pagination-bar {
        justify-content: center;
    }
}
</style>
