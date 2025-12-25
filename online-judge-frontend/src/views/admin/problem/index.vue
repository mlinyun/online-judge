<script setup lang="ts">
/**
 * 管理后台 - 题目管理页面
 */
import { computed, onMounted, ref, watch } from "vue";
import { useRouter } from "vue-router";
import { Collection, Delete, Document, Edit, More, Plus, Search, User, View } from "@element-plus/icons-vue";
import { deleteProblem, selectProblemListByAdmin } from "@/api/problem";
import type { Api } from "@/types/api/api";

defineOptions({ name: "AdminProblem" });

const router = useRouter();

type ProblemRow = Api.Problem.SelectProblemListItem;

const loading = ref(false);
const loadError = ref("");

const page = ref(1);
const pageSize = ref(20);
const total = ref(0);

const problems = ref<ProblemRow[]>([]);

// Toolbar (SearchInfo)
const searchId = ref("");
const searchTitle = ref("");
const searchTags = ref("");
const searchUserNickName = ref("");

const hasSearch = computed(() => {
    return (
        !!searchId.value.trim() ||
        !!searchTitle.value.trim() ||
        !!searchTags.value.trim() ||
        !!searchUserNickName.value.trim()
    );
});

let debounceTimer: number | undefined;
const debounce = (fn: () => void, delay = 300) => {
    if (debounceTimer) window.clearTimeout(debounceTimer);
    debounceTimer = window.setTimeout(() => fn(), delay);
};

const parseTags = (raw: string): Api.Tag.Tags | undefined => {
    const items = raw
        .split(/[，,\s]+/)
        .map((x) => x.trim())
        .filter(Boolean);
    return items.length ? (items as Api.Tag.Tags) : undefined;
};

const buildSearchInfo = (): Api.Problem.SelectProblemListByAdminParams["SearchInfo"] | undefined => {
    const Id = searchId.value.trim();
    const Title = searchTitle.value.trim();
    const Tags = parseTags(searchTags.value);
    const UserNickName = searchUserNickName.value.trim();

    if (!Id && !Title && !Tags?.length && !UserNickName) return undefined;

    return {
        ...(Id ? { Id: Id as Api.Problem.ProblemId } : {}),
        ...(Title ? { Title } : {}),
        ...(Tags?.length ? { Tags } : {}),
        ...(UserNickName ? { UserNickName } : {}),
    };
};

const getPassRateText = (row: ProblemRow) => {
    const submit = Number(row.SubmitNum || 0);
    const ac = Number(row.ACNum || 0);
    if (!submit) return "-";
    const pct = (ac / submit) * 100;
    return `${pct.toFixed(1)}%`;
};

const requestSeq = ref(0);

const fetchProblems = async () => {
    const seq = ++requestSeq.value;
    loading.value = true;
    loadError.value = "";

    try {
        const SearchInfo = buildSearchInfo();
        const response = await selectProblemListByAdmin({
            Page: page.value,
            PageSize: pageSize.value,
            ...(SearchInfo ? { SearchInfo } : {}),
        });

        if (seq !== requestSeq.value) return;

        if (response.data.code === 0 && response.data.data) {
            problems.value = response.data.data.List || [];
            total.value = response.data.data.Total || 0;
        } else {
            problems.value = [];
            total.value = 0;
            loadError.value = response.data.message || "加载失败";
        }
    } catch (err) {
        console.error("获取题目列表失败:", err);
        problems.value = [];
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

const handleCreate = () => {
    // 新前端已存在 ProblemEditor 视图，但路由暂未接入；先给出一致 UI + 提示
    ElMessage({
        type: "info",
        message: "题目新增/编辑页暂未接入路由",
        showClose: true,
    });
};

const handleView = (row: ProblemRow) => {
    router.push({ name: "problem-detail", params: { id: row._id } });
};

const handleEdit = (row: ProblemRow) => {
    ElMessage({
        type: "info",
        message: `题目编辑页暂未接入路由（ID: ${row._id}）`,
        showClose: true,
    });
};

const handleDelete = async (row: ProblemRow) => {
    try {
        await ElMessageBox.confirm(`确认删除题目「${row.Title}」？此操作不可恢复。`, "删除确认", {
            type: "warning",
            confirmButtonText: "删除",
            cancelButtonText: "取消",
            confirmButtonClass: "el-button--danger",
        });
    } catch {
        return;
    }

    loading.value = true;
    try {
        const response = await deleteProblem(row._id);
        if (response.data.code === 0) {
            const isLastItemOnPage = problems.value.length <= 1 && page.value > 1;
            if (isLastItemOnPage) page.value -= 1;
            await fetchProblems();
        }
    } catch (err) {
        console.error("删除题目失败:", err);
    } finally {
        loading.value = false;
    }
};

const handleRowCommand = (cmd: string, row: ProblemRow) => {
    if (cmd === "view") {
        handleView(row);
        return;
    }
    if (cmd === "edit") {
        handleEdit(row);
        return;
    }
    if (cmd === "delete") {
        void handleDelete(row);
    }
};

watch([page, pageSize], () => {
    fetchProblems();
});

watch(
    [searchId, searchTitle, searchTags, searchUserNickName],
    () => {
        if (page.value !== 1) {
            page.value = 1;
            return;
        }
        debounce(() => {
            fetchProblems();
        });
    },
    { deep: false }
);

onMounted(() => {
    fetchProblems();
});
</script>

<template>
    <section class="admin-problem" aria-label="题目管理">
        <!-- Toolbar -->
        <div class="toolbar oj-glass-panel">
            <div class="toolbar-left">
                <el-input
                    v-model="searchId"
                    class="toolbar-field"
                    :prefix-icon="Search"
                    clearable
                    placeholder="题目ID (Id)"
                />

                <el-input
                    v-model="searchTitle"
                    class="toolbar-field"
                    :prefix-icon="Document"
                    clearable
                    placeholder="题目标题 (Title)"
                />

                <el-input
                    v-model="searchTags"
                    class="toolbar-field"
                    :prefix-icon="Collection"
                    clearable
                    placeholder="标签 (Tags，用空格/逗号分隔)"
                />

                <el-input
                    v-model="searchUserNickName"
                    class="toolbar-field"
                    :prefix-icon="User"
                    clearable
                    placeholder="创建者昵称 (UserNickName)"
                />
            </div>

            <div class="toolbar-right">
                <el-button class="create-btn" type="primary" :icon="Plus" @click="handleCreate">新增题目</el-button>
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
                    <el-empty
                        v-if="!problems.length && !loadError"
                        :description="hasSearch ? '暂无匹配的题目' : '暂无题目数据'"
                    />

                    <el-empty v-else-if="!problems.length && loadError" :description="loadError">
                        <el-button type="primary" plain @click="fetchProblems">重试加载</el-button>
                    </el-empty>

                    <div v-else>
                        <el-table :data="problems" class="problem-table" table-layout="fixed">
                            <el-table-column label="ID" width="150">
                                <template #default="scope">
                                    <span class="id-mono" :title="String(scope.row._id)">
                                        {{ String(scope.row._id) }}
                                    </span>
                                </template>
                            </el-table-column>

                            <el-table-column label="标题" min-width="260">
                                <template #default="scope">
                                    <span class="title-ellipsis" :title="scope.row.Title">{{ scope.row.Title }}</span>
                                </template>
                            </el-table-column>

                            <el-table-column label="标签" min-width="220">
                                <template #default="scope">
                                    <div class="tags-wrap">
                                        <el-tag
                                            v-for="tag in scope.row.Tags"
                                            :key="tag"
                                            size="small"
                                            type="info"
                                            class="tag-item"
                                        >
                                            {{ tag }}
                                        </el-tag>
                                        <span v-if="!scope.row.Tags?.length" class="muted-text">-</span>
                                    </div>
                                </template>
                            </el-table-column>

                            <el-table-column label="通过率" width="110" align="center">
                                <template #default="scope">
                                    <span class="muted-text">{{ getPassRateText(scope.row) }}</span>
                                </template>
                            </el-table-column>

                            <el-table-column prop="SubmitNum" label="提交" width="96" align="right" />
                            <el-table-column prop="ACNum" label="通过" width="96" align="right" />

                            <el-table-column label="操作" width="120" fixed="right" align="center">
                                <template #default="scope">
                                    <el-dropdown
                                        trigger="click"
                                        @command="(cmd) => handleRowCommand(String(cmd), scope.row)"
                                    >
                                        <el-button class="opt-btn" link :icon="More">操作</el-button>
                                        <template #dropdown>
                                            <el-dropdown-menu>
                                                <el-dropdown-item command="view" :icon="View">查看</el-dropdown-item>
                                                <el-dropdown-item command="edit" :icon="Edit">编辑</el-dropdown-item>
                                                <el-dropdown-item command="delete" :icon="Delete" divided>
                                                    删除
                                                </el-dropdown-item>
                                            </el-dropdown-menu>
                                        </template>
                                    </el-dropdown>
                                </template>
                            </el-table-column>
                        </el-table>

                        <!-- Pagination -->
                        <div class="pagination-bar">
                            <el-pagination
                                v-model:current-page="page"
                                v-model:page-size="pageSize"
                                :page-sizes="[10, 20, 40, 60]"
                                :total="total"
                                layout="total, sizes, prev, pager, next, jumper"
                                @current-change="handlePageChange"
                                @size-change="handleSizeChange"
                            />
                        </div>
                    </div>
                </template>
            </el-skeleton>
        </div>
    </section>
</template>

<style scoped>
.admin-problem {
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
    align-items: flex-start;
    justify-content: space-between;
    padding: var(--oj-spacing-4);
    overflow: hidden;
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
    transition:
        border-color 0.2s ease,
        transform 0.2s ease;

    --el-color-primary: rgb(var(--oj-color-primary-rgb));
    --el-select-input-focus-border-color: rgb(var(--oj-color-primary-rgb));
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
    grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
    gap: var(--oj-spacing-3);
    align-items: center;
    min-width: 0;
}

.toolbar-right {
    display: flex;
    flex: 0 0 auto;
    gap: var(--oj-spacing-3);
    align-items: center;
}

.toolbar-field {
    width: 100%;
    min-width: 0;
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
    background-color: var(--oj-input-bg);
    box-shadow:
        0 0 0 1px var(--oj-input-focus-border) inset,
        0 0 0 3px var(--oj-color-primary-soft) !important;
}

.toolbar :deep(.el-select .el-input__wrapper.is-focus),
.toolbar :deep(.el-select__wrapper.is-focused) {
    box-shadow:
        0 0 0 1px var(--oj-input-focus-border) inset,
        0 0 0 3px var(--oj-color-primary-soft) !important;
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

.create-btn {
    --el-color-primary: var(--oj-button-primary-bg);
    --el-button-bg-color: var(--oj-button-primary-bg);
    --el-button-border-color: var(--oj-button-primary-bg);
    --el-button-hover-bg-color: var(--oj-button-primary-hover-bg);
    --el-button-hover-border-color: var(--oj-button-primary-hover-bg);
    --el-button-active-bg-color: var(--oj-button-primary-hover-bg);
    --el-button-active-border-color: var(--oj-button-primary-hover-bg);
}

.skeleton-block {
    height: 44px;
    margin: var(--oj-spacing-4);
    border-radius: var(--oj-radius-lg);
}

/* Table panel */
.table-panel {
    overflow: hidden;
    border-radius: var(--oj-radius-xl);
}

/* Table fine tuning */
.problem-table :deep(.el-table__inner-wrapper::before) {
    background-color: transparent;
}

.problem-table :deep(.el-table__header-wrapper th.el-table__cell) {
    color: var(--oj-text-color-muted);
    background: var(--oj-table-header-bg);
    border-bottom: 1px solid var(--oj-glass-border);
}

.problem-table :deep(.el-table__body tr:hover > td.el-table__cell) {
    background: var(--oj-table-row-hover-bg);
}

.problem-table :deep(.el-table__body td.el-table__cell) {
    border-bottom: 1px solid var(--oj-table-border);
}

.id-mono {
    font-family: var(--oj-font-family-mono), monospace;
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-secondary);
}

.title-ellipsis {
    overflow: hidden;
    text-overflow: ellipsis;
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
    white-space: nowrap;
}

.muted-text {
    color: var(--oj-text-color-muted);
}

:deep(.el-dropdown-menu__item:not(.is-active):focus, .el-dropdown-menu__item:not(.is-active):hover) {
    color: var(--oj-color-primary);
    background-color: rgb(var(--oj-color-primary-rgb) / 10%);
}

.opt-btn {
    color: var(--oj-link-hover-color);
}

.tags-wrap {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-2);
}

.tag-item {
    flex: 0 0 auto;
}

.pagination-bar {
    display: flex;
    justify-content: flex-end;
    padding: var(--oj-spacing-4);
    background: var(--oj-table-header-bg);
    border-top: 1px solid var(--oj-glass-border);
}

@media (width <=640px) {
    .toolbar-field {
        width: 100%;
    }

    .toolbar-select {
        width: 100%;
    }

    .pagination-bar {
        justify-content: center;
    }
}
</style>
