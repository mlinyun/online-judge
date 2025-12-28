<script setup lang="ts">
import { computed, ref } from "vue";
import { Check } from "@element-plus/icons-vue";
import type { Api } from "@/types/api/api";
import type { TableInstance } from "element-plus";

defineOptions({ name: "ProblemTable" });

type ProblemRow = Api.Problem.SelectProblemListItem;

interface Props {
    rows: ProblemRow[];
    loading: boolean;
    loadError: string;
    hasSearch: boolean;
    solvedIds: Set<string>;
}

const props = defineProps<Props>();

const emit = defineEmits<{
    rowClick: [row: ProblemRow];
    rowHover: [row: ProblemRow];
    retry: [];
}>();

// 表格布局方式
const tableLayout = ref<TableInstance["tableLayout"]>("fixed");

const emptyText = computed(() => {
    if (props.loadError) return props.loadError;
    return props.hasSearch ? "暂无匹配的题目" : "暂无题目数据";
});

const getPassRateText = (row: ProblemRow) => {
    const submit = Number(row.SubmitNum || 0);
    const ac = Number(row.ACNum || 0);
    if (!submit) return "-";
    const pct = (ac / submit) * 100;
    return `${pct.toFixed(1)}%`;
};
</script>

<template>
    <div class="table-panel oj-glass-panel" aria-label="题目列表">
        <el-skeleton :loading="loading" animated>
            <template #template>
                <div class="skeleton-block" />
                <div class="skeleton-block" />
                <div class="skeleton-block" />
            </template>

            <template #default>
                <el-empty v-if="!rows.length" :description="emptyText">
                    <template #default>
                        <div v-if="loadError" class="empty-extra">
                            <el-button type="primary" plain @click="emit('retry')">重试加载</el-button>
                        </div>
                    </template>
                </el-empty>

                <div v-else class="table-wrap">
                    <el-table :data="rows" class="problem-table" stripe :table-layout="tableLayout"
                        @row-click="(row: ProblemRow) => emit('rowClick', row)"
                        @row-mouseenter="(row: ProblemRow) => emit('rowHover', row)">
                        <el-table-column label="状态" width="80" align="center">
                            <template #default="scope">
                                <el-icon v-if="solvedIds.has(String(scope.row._id))" class="solved-ic">
                                    <Check />
                                </el-icon>
                            </template>
                        </el-table-column>

                        <el-table-column label="ID" min-width="80">
                            <template #default="scope">
                                <span class="mono id-text" :title="String(scope.row._id)">{{ scope.row._id }}</span>
                            </template>
                        </el-table-column>

                        <el-table-column label="题目" min-width="200">
                            <template #default="scope">
                                <div class="entity-cell">
                                    <div class="entity-main content-clamp" :title="String(scope.row.Title)">
                                        {{ scope.row.Title }}
                                    </div>
                                </div>
                            </template>
                        </el-table-column>

                        <el-table-column prop="Tags" label="标签" min-width="200">
                            <template #default="scope">
                                <div class="tag-list">
                                    <el-tag v-for="t in scope.row.Tags" :key="t" size="small" disable-transitions
                                        class="tag">
                                        {{ t }}
                                    </el-tag>
                                </div>
                            </template>
                        </el-table-column>

                        <el-table-column label="通过" width="80" align="center">
                            <template #default="scope">
                                <span class="metric">{{ scope.row.ACNum }}</span>
                            </template>
                        </el-table-column>
                        <el-table-column label="提交" width="80" align="center">
                            <template #default="scope">
                                <span class="metric">{{ scope.row.SubmitNum }}</span>
                            </template>
                        </el-table-column>

                        <el-table-column label="通过率" width="100" align="center">
                            <template #default="scope">{{ getPassRateText(scope.row) }}</template>
                        </el-table-column>
                    </el-table>
                </div>
            </template>
        </el-skeleton>
    </div>
</template>

<style scoped>
.table-panel {
    overflow: hidden;
    border-radius: var(--oj-radius-2xl);
}

.table-wrap {
    overflow-x: auto;
    -webkit-overflow-scrolling: touch;
}

.skeleton-block {
    height: 88px;
    margin: var(--oj-spacing-4);
    background: var(--oj-glass-bg-light);
    border-radius: var(--oj-radius-xl);
}

.empty-extra {
    margin-top: var(--oj-spacing-3);
}

.solved-ic {
    color: var(--oj-color-success);
}

.tag-list {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-2);
}

.tag {
    border-color: var(--oj-glass-border);
}

.problem-table :deep(.el-table) {
    background: transparent;
}

.problem-table {
    width: 100%;
    min-width: 760px;
}

.problem-table :deep(.el-table__inner-wrapper::before) {
    background-color: transparent;
}

.problem-table :deep(.el-table__header-wrapper th.el-table__cell) {
    color: var(--oj-text-color-muted);
    background: var(--oj-table-header-bg);
    border-bottom: 1px solid var(--oj-glass-border);
}

.problem-table :deep(td.el-table__cell) {
    border-bottom: 1px solid var(--oj-glass-border);
}

.problem-table :deep(.el-table__header-wrapper) {
    background: var(--oj-table-header-bg);
}

.problem-table :deep(.el-table__row) {
    cursor: pointer;
}

.problem-table :deep(.el-table__row:hover > td.el-table__cell) {
    background: var(--oj-table-row-hover-bg);
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

.content-clamp {
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
}

.metric {
    font-variant-numeric: tabular-nums;
    color: var(--oj-text-color-secondary);
}

@media (width <=640px) {
    /* 手机端：表格可左右滑，避免列被压扁到不可读 */
    .problem-table {
        min-width: 680px;
    }

    /* 手机端：标签不换行，防止行高爆炸；只展示前 3 个标签 */
    .tag-list {
        flex-wrap: nowrap;
        overflow: hidden;
    }

    .tag-list :deep(.el-tag:nth-child(n + 4)) {
        display: none;
    }
}
</style>
