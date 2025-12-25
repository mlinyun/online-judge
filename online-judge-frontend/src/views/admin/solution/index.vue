<script setup lang="ts">
/**
 * 管理后台 - 题解管理页面
 */
import { computed, onMounted, ref, watch } from "vue";
import { useRouter } from "vue-router";
import { Delete, Document, Edit, More, Search, User, View } from "@element-plus/icons-vue";
import { deleteSolution, selectSolutionListByAdmin } from "@/api/solution";
import { DateUtils } from "@/utils/date/date-utils";
import type { Api } from "@/types/api/api";

defineOptions({ name: "AdminSolution" });

const router = useRouter();

type SolutionRow = Api.Solution.SelectSolutionListByAdminItem;

const loading = ref(false);
const loadError = ref("");

const page = ref(1);
const pageSize = ref(20);
const total = ref(0);

const solutions = ref<SolutionRow[]>([]);

// Toolbar
const searchTitle = ref("");
const searchUserId = ref("");
const searchParentId = ref("");
const searchPublic = ref<"all" | "public" | "private">("all");

const hasSearch = computed(() => {
    return (
        !!searchTitle.value.trim() ||
        !!searchUserId.value.trim() ||
        !!searchParentId.value.trim() ||
        searchPublic.value !== "all"
    );
});

let debounceTimer: number | undefined;
const debounce = (fn: () => void, delay = 300) => {
    if (debounceTimer) window.clearTimeout(debounceTimer);
    debounceTimer = window.setTimeout(() => fn(), delay);
};

const normalizeUser = (row: SolutionRow): Api.User.SimpleUserInfo | undefined => {
    const user = row.User as Api.User.SimpleUserInfo | Api.User.SimpleUserInfo[];
    if (Array.isArray(user)) return user[0];
    return user;
};

const getAuthorName = (row: SolutionRow) => {
    const user = normalizeUser(row);
    return user?.NickName || "-";
};

const getAuthorAvatar = (row: SolutionRow) => {
    const user = normalizeUser(row);
    return user?.Avatar || "";
};

const getPublicLabel = (row: SolutionRow) => {
    return row.Public ? "公开" : "未公开";
};

const getPublicTagType = (row: SolutionRow) => {
    return row.Public ? "success" : "info";
};

const requestSeq = ref(0);

const buildSearchInfo = (): Api.Solution.SelectSolutionListByAdminParams["SearchInfo"] | undefined => {
    const Title = searchTitle.value.trim();
    const UserId = searchUserId.value.trim();
    const ParentId = searchParentId.value.trim();

    const Public = searchPublic.value === "all" ? undefined : searchPublic.value === "public";

    if (!Title && !UserId && !ParentId && typeof Public !== "boolean") return undefined;

    return {
        ...(Title ? { Title } : {}),
        ...(UserId ? { UserId: UserId as Api.User.UserId } : {}),
        ...(ParentId ? { ParentId: ParentId as Api.Problem.ProblemId } : {}),
        ...(typeof Public === "boolean" ? { Public } : {}),
    };
};

const fetchSolutions = async () => {
    const seq = ++requestSeq.value;
    loading.value = true;
    loadError.value = "";

    try {
        const SearchInfo = buildSearchInfo();
        const response = await selectSolutionListByAdmin({
            Page: page.value,
            PageSize: pageSize.value,
            ...(SearchInfo ? { SearchInfo } : {}),
        });

        if (seq !== requestSeq.value) return;

        if (response.data.code === 0 && response.data.data) {
            solutions.value = (response.data.data.List || []) as SolutionRow[];
            total.value = response.data.data.Total || 0;
        } else {
            solutions.value = [];
            total.value = 0;
            loadError.value = response.data.message || "加载失败";
        }
    } catch (err) {
        console.error("获取题解列表失败:", err);
        solutions.value = [];
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

const handleView = (row: SolutionRow) => {
    router.push({ name: "solution-detail", params: { id: row._id } });
};

const handleEdit = (row: SolutionRow) => {
    // 新前端暂未接入题解编辑路由（仅存在视图占位），先给出一致的 UI 与提示
    ElMessage({
        type: "info",
        message: `题解编辑暂未接入路由（ID: ${row._id}）`,
        showClose: true,
    });
};

const handleDelete = async (row: SolutionRow) => {
    try {
        await ElMessageBox.confirm(`确认删除题解「${row.Title}」？此操作不可恢复。`, "删除确认", {
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
        const response = await deleteSolution(row._id);
        if (response.data.code === 0) {
            const isLastItemOnPage = solutions.value.length <= 1 && page.value > 1;
            if (isLastItemOnPage) page.value -= 1;
            await fetchSolutions();
        }
    } catch (err) {
        console.error("删除题解失败:", err);
    } finally {
        loading.value = false;
    }
};

const handleRowCommand = (command: "view" | "edit" | "delete", row: SolutionRow) => {
    if (command === "view") return handleView(row);
    if (command === "edit") return handleEdit(row);
    if (command === "delete") return handleDelete(row);
};

watch([page, pageSize], () => {
    fetchSolutions();
});

watch(
    [searchTitle, searchUserId, searchParentId, searchPublic],
    () => {
        if (page.value !== 1) {
            page.value = 1;
            return;
        }
        debounce(() => {
            fetchSolutions();
        });
    },
    { deep: false }
);

onMounted(() => {
    fetchSolutions();
});
</script>

<template>
    <section class="admin-solution" aria-label="题解管理">
        <!-- Toolbar -->
        <div class="toolbar oj-glass-panel">
            <div class="toolbar-left">
                <el-input
                    v-model="searchTitle"
                    class="toolbar-field"
                    :prefix-icon="Search"
                    clearable
                    placeholder="题解标题"
                />

                <el-input
                    v-model="searchUserId"
                    class="toolbar-field"
                    :prefix-icon="User"
                    clearable
                    placeholder="用户ID"
                />

                <el-input
                    v-model="searchParentId"
                    class="toolbar-field"
                    :prefix-icon="Document"
                    clearable
                    placeholder="题目ID(ParentId)"
                />

                <el-select
                    v-model="searchPublic"
                    class="toolbar-field toolbar-select"
                    placeholder="公开状态"
                    popper-class="oj-discussion-source-popper"
                >
                    <el-option label="全部" value="all" />
                    <el-option label="公开" value="public" />
                    <el-option label="未公开" value="private" />
                </el-select>
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
                        v-if="!solutions.length && !loadError"
                        :description="hasSearch ? '暂无匹配的题解' : '暂无题解数据'"
                    />

                    <el-empty v-else-if="!solutions.length && loadError" :description="loadError">
                        <el-button type="primary" plain @click="fetchSolutions">重试加载</el-button>
                    </el-empty>

                    <div v-else>
                        <el-table :data="solutions" class="solution-table" table-layout="fixed">
                            <el-table-column label="标题" min-width="260">
                                <template #default="scope">
                                    <div class="title-cell" @click="handleView(scope.row)">
                                        <span class="title-text" :title="scope.row.Title">{{ scope.row.Title }}</span>
                                    </div>
                                </template>
                            </el-table-column>

                            <el-table-column label="用户" min-width="200">
                                <template #default="scope">
                                    <div class="user-cell">
                                        <el-avatar class="user-avatar" :size="32" :src="getAuthorAvatar(scope.row)">
                                            {{ getAuthorName(scope.row).slice(0, 1) }}
                                        </el-avatar>
                                        <div class="user-meta">
                                            <div class="user-name">{{ getAuthorName(scope.row) }}</div>
                                            <div class="user-sub">ID: {{ scope.row._id }}</div>
                                        </div>
                                    </div>
                                </template>
                            </el-table-column>

                            <el-table-column label="公开" width="120" align="center">
                                <template #default="scope">
                                    <el-tag :type="getPublicTagType(scope.row)">{{ getPublicLabel(scope.row) }}</el-tag>
                                </template>
                            </el-table-column>

                            <el-table-column label="发布时间" width="170">
                                <template #default="scope">
                                    <span class="time-text">{{ DateUtils.formatDateTime(scope.row.CreateTime) }}</span>
                                </template>
                            </el-table-column>

                            <el-table-column prop="Views" label="浏览" width="96" align="right" />
                            <el-table-column prop="Comments" label="评论" width="96" align="right" />

                            <el-table-column label="操作" width="120" fixed="right" align="right">
                                <template #default="scope">
                                    <el-dropdown
                                        trigger="click"
                                        @command="(cmd) => handleRowCommand(cmd as any, scope.row)"
                                    >
                                        <el-button class="opt-btn" link :icon="More">操作</el-button>
                                        <template #dropdown>
                                            <el-dropdown-menu>
                                                <el-dropdown-item command="view" :icon="View">查看</el-dropdown-item>
                                                <el-dropdown-item command="edit" :icon="Edit">编辑</el-dropdown-item>
                                                <el-dropdown-item command="delete" :icon="Delete" divided
                                                    >删除</el-dropdown-item
                                                >
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
.admin-solution {
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
    grid-template-columns: repeat(auto-fit, minmax(240px, 1fr));
    gap: var(--oj-spacing-3);
    align-items: center;
    min-width: 0;
}

.toolbar-field {
    width: 100%;
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

/* Select dropdown (popper is teleported to body) */
:global(.oj-discussion-source-popper) {
    --el-color-primary: rgb(var(--oj-color-primary-rgb));
}

:global(.oj-discussion-source-popper .el-select-dropdown__item.is-selected) {
    color: rgb(var(--oj-color-primary-rgb));
}

:global(.oj-discussion-source-popper .el-select-dropdown__item:hover) {
    color: rgb(var(--oj-color-primary-rgb));
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
.solution-table :deep(.el-table__inner-wrapper::before) {
    background-color: transparent;
}

.solution-table :deep(.el-table__header-wrapper th.el-table__cell) {
    color: var(--oj-text-color-muted);
    background: var(--oj-table-header-bg);
    border-bottom: 1px solid var(--oj-glass-border);
}

.solution-table :deep(.el-table__body tr:hover > td.el-table__cell) {
    background: var(--oj-table-row-hover-bg);
}

.solution-table :deep(.el-table__body td.el-table__cell) {
    border-bottom: 1px solid var(--oj-table-border);
}

.title-cell {
    display: flex;
    align-items: center;
    min-width: 0;
    cursor: pointer;
}

.title-text {
    overflow: hidden;
    text-overflow: ellipsis;
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
    white-space: nowrap;
}

.user-cell {
    display: flex;
    gap: var(--oj-spacing-3);
    align-items: center;
    min-width: 0;
}

.title-cell:hover .title-text {
    color: var(--oj-color-primary);
}

.user-avatar {
    border: 1px solid var(--oj-color-primary);
}

.user-meta {
    display: flex;
    flex-direction: column;
    min-width: 0;
}

.user-name {
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
}

.user-sub {
    overflow: hidden;
    text-overflow: ellipsis;
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-muted);
    white-space: nowrap;
}

.time-text {
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-secondary);
}

:deep(.el-dropdown-menu__item:not(.is-active):focus, .el-dropdown-menu__item:not(.is-active):hover) {
    color: var(--oj-color-primary);
    background-color: rgb(var(--oj-color-primary-rgb) / 10%);
}

.muted-text {
    color: var(--oj-text-color-muted);
}

.opt-btn {
    color: var(--oj-link-hover-color);
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
