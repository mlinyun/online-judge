<script setup lang="ts">
/**
 * 管理后台 - 讨论区管理页面
 */
import { computed, onMounted, ref, watch } from "vue";
import { useRouter } from "vue-router";
import { Collection, Delete, Edit, More, Search, User, View } from "@element-plus/icons-vue";
import { deleteDiscuss, selectDiscussListByAdmin } from "@/api/discussion";
import { DateUtils } from "@/utils/date/date-utils";
import type { Api } from "@/types/api/api";

defineOptions({ name: "AdminDiscussion" });

const router = useRouter();

type DiscussionRow = Api.Discuss.SelectDiscussListItem;

const loading = ref(false);
const loadError = ref("");

const page = ref(1);
const pageSize = ref(20);
const total = ref(0);

const discussions = ref<DiscussionRow[]>([]);

// Toolbar
const searchTitle = ref("");
const searchUserId = ref("");
const searchParentId = ref("");
const sourceFilter = ref<"all" | "forum" | "problem">("all");

const hasSearch = computed(() => {
    return (
        !!searchTitle.value.trim() ||
        !!searchUserId.value.trim() ||
        !!searchParentId.value.trim() ||
        sourceFilter.value !== "all"
    );
});

let debounceTimer: number | undefined;
const debounce = (fn: () => void, delay = 300) => {
    if (debounceTimer) window.clearTimeout(debounceTimer);
    debounceTimer = window.setTimeout(() => fn(), delay);
};

const normalizeUser = (row: DiscussionRow): Api.User.SimpleUserInfo | undefined => {
    const user = row.User as Api.User.SimpleUserInfo | Api.User.SimpleUserInfo[];
    if (Array.isArray(user)) return user[0];
    return user;
};

const normalizeRows = computed(() => {
    return discussions.value.filter((item) => {
        const pid = String(item.ParentId);
        // 只保留讨论区(0)或题目关联讨论(非0)，过滤掉未知形式
        return pid === "0" || pid.length > 0;
    });
});

const filteredDiscussions = computed(() => {
    const kw = searchTitle.value.trim();
    const src = sourceFilter.value;

    let list = normalizeRows.value;

    if (kw) {
        list = list.filter((d) => (d.Title || "").includes(kw));
    }

    if (src === "forum") {
        list = list.filter((d) => String(d.ParentId) === "0");
    } else if (src === "problem") {
        list = list.filter((d) => String(d.ParentId) !== "0");
    }

    return list;
});

const requestSeq = ref(0);

const buildSearchInfo = (): Api.Discuss.SelectDiscussListByAdminParams["SearchInfo"] | undefined => {
    const Title = searchTitle.value.trim();
    const UserId = searchUserId.value.trim();
    const ParentId = searchParentId.value.trim() || (sourceFilter.value === "forum" ? "0" : undefined);

    if (!Title && !UserId && !ParentId) return undefined;

    return {
        ...(Title ? { Title } : {}),
        ...(UserId ? { UserId: UserId as Api.User.UserId } : {}),
        ...(ParentId ? { ParentId: ParentId as Api.Problem.ProblemId | "0" } : {}),
    };
};

const fetchDiscussions = async () => {
    const seq = ++requestSeq.value;
    loading.value = true;
    loadError.value = "";

    try {
        const SearchInfo = buildSearchInfo();
        const response = await selectDiscussListByAdmin({
            Page: page.value,
            PageSize: pageSize.value,
            ...(SearchInfo ? { SearchInfo } : {}),
        });

        if (seq !== requestSeq.value) return;

        if (response.data.code === 0 && response.data.data) {
            discussions.value = (response.data.data.List || []) as DiscussionRow[];
            total.value = response.data.data.Total || 0;
        } else {
            discussions.value = [];
            total.value = 0;
            loadError.value = response.data.message || "加载失败";
        }
    } catch (err) {
        console.error("获取讨论列表失败:", err);
        discussions.value = [];
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

const getAuthorName = (row: DiscussionRow) => {
    const user = normalizeUser(row);
    return user?.NickName || "-";
};

const getAuthorAvatar = (row: DiscussionRow) => {
    const user = normalizeUser(row);
    return user?.Avatar || "";
};

const getSourceLabel = (row: DiscussionRow) => {
    const pid = String(row.ParentId);
    return pid === "0" ? "讨论区" : "题目讨论";
};

const handleView = (row: DiscussionRow) => {
    router.push({ name: "discussion-detail", params: { id: row._id } });
};

const handleEdit = (row: DiscussionRow) => {
    // 编辑入口暂未接入，先给出提示，保持与用户管理一致的交互
    ElMessage({
        type: "info",
        message: `讨论编辑暂未接入路由（ID: ${row._id}）`,
        showClose: true,
    });
};

const handleDelete = async (row: DiscussionRow) => {
    try {
        await ElMessageBox.confirm(`确认删除讨论「${row.Title}」？此操作不可恢复。`, "删除确认", {
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
        const response = await deleteDiscuss(row._id);
        if (response.data.code === 0) {
            const isLastItemOnPage = discussions.value.length <= 1 && page.value > 1;
            if (isLastItemOnPage) page.value -= 1;
            await fetchDiscussions();
        }
    } catch (err) {
        console.error("删除讨论失败:", err);
    } finally {
        loading.value = false;
    }
};

const handleRowCommand = (command: "view" | "edit" | "delete", row: DiscussionRow) => {
    if (command === "view") return handleView(row);
    if (command === "edit") return handleEdit(row);
    if (command === "delete") return handleDelete(row);
};

watch([page, pageSize], () => {
    fetchDiscussions();
});

watch(
    [searchTitle, searchUserId, searchParentId, sourceFilter],
    () => {
        if (page.value !== 1) {
            page.value = 1;
            return;
        }
        debounce(() => {
            fetchDiscussions();
        });
    },
    { deep: false }
);

onMounted(() => {
    fetchDiscussions();
});
</script>

<template>
    <section class="admin-discussion" aria-label="讨论管理">
        <!-- Toolbar -->
        <div class="toolbar oj-glass-panel">
            <div class="toolbar-left">
                <el-input
                    v-model="searchTitle"
                    class="toolbar-field"
                    :prefix-icon="Search"
                    clearable
                    placeholder="搜索讨论标题..."
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
                    :prefix-icon="Collection"
                    clearable
                    placeholder="父ID (0或题目ID)"
                />

                <el-select
                    v-model="sourceFilter"
                    class="toolbar-field toolbar-select"
                    placeholder="讨论来源"
                    popper-class="oj-discussion-source-popper"
                >
                    <el-option label="全部" value="all" />
                    <el-option label="讨论区" value="forum" />
                    <el-option label="题目讨论" value="problem" />
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
                        v-if="!filteredDiscussions.length && !loadError"
                        :description="hasSearch ? '暂无匹配的讨论' : '暂无讨论数据'"
                    />

                    <el-empty v-else-if="!filteredDiscussions.length && loadError" :description="loadError">
                        <el-button type="primary" plain @click="fetchDiscussions">重试加载</el-button>
                    </el-empty>

                    <div v-else>
                        <el-table :data="filteredDiscussions" class="discussion-table" table-layout="fixed">
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

                            <el-table-column label="类型" width="120">
                                <template #default="scope">
                                    <el-tag size="small" type="info">{{ getSourceLabel(scope.row) }}</el-tag>
                                </template>
                            </el-table-column>

                            <el-table-column label="浏览" width="90" align="center">
                                <template #default="scope">
                                    <span class="muted-text">{{ scope.row.Views }}</span>
                                </template>
                            </el-table-column>

                            <el-table-column label="评论" width="90" align="center">
                                <template #default="scope">
                                    <span class="muted-text">{{ scope.row.Comments }}</span>
                                </template>
                            </el-table-column>

                            <el-table-column label="创建时间" width="170">
                                <template #default="scope">
                                    <span class="time-text">{{ DateUtils.formatDateTime(scope.row.CreateTime) }}</span>
                                </template>
                            </el-table-column>

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
.admin-discussion {
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

    --el-color-primary: rgb(var(--oj-color-primary-rgb));
    --el-select-input-focus-border-color: rgb(var(--oj-color-primary-rgb));

    overflow: hidden;
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
    transition:
        border-color 0.2s ease,
        transform 0.2s ease;
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
.discussion-table :deep(.el-table__inner-wrapper::before) {
    background-color: transparent;
}

.discussion-table :deep(.el-table__header-wrapper th.el-table__cell) {
    color: var(--oj-text-color-muted);
    background: var(--oj-table-header-bg);
    border-bottom: 1px solid var(--oj-glass-border);
}

.discussion-table :deep(.el-table__body tr:hover > td.el-table__cell) {
    background: var(--oj-table-row-hover-bg);
}

.discussion-table :deep(.el-table__body td.el-table__cell) {
    border-bottom: 1px solid var(--oj-table-border);
}

.title-cell {
    display: flex;
    gap: var(--oj-spacing-2);
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

.title-cell:hover .title-text {
    color: var(--oj-color-primary);
}

.user-cell {
    display: flex;
    gap: var(--oj-spacing-3);
    align-items: center;
    min-width: 0;
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
