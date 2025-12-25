<script setup lang="ts">
/**
 * 管理后台 - 用户管理页面
 */
import { computed, onMounted, ref, watch } from "vue";
import { Collection, Delete, Edit, More, School, Search, User, View } from "@element-plus/icons-vue";
import { deleteUser, selectUserList } from "@/api/user";
import { DateUtils } from "@/utils/date/date-utils.ts";
import { UserDetailDrawer, UserEditDialog } from "@/components/admin";
import type { Api } from "@/types/api/api";

defineOptions({ name: "AdminUsers" });

const loading = ref(false);
const loadError = ref("");

let debounceTimer: number | undefined;
// 防抖函数
const debounce = (fn: () => void, delay = 300) => {
    if (debounceTimer) window.clearTimeout(debounceTimer);
    debounceTimer = window.setTimeout(() => fn(), delay);
};

const page = ref(1);
const pageSize = ref(20);
const total = ref(0);

const users = ref<Api.User.UserManageListItem[]>([]);

const detailDrawerOpen = ref(false);
const activeUserId = ref<Api.User.UserId | undefined>(undefined);

const editDialogOpen = ref(false);
const editUserId = ref<Api.User.UserId | undefined>(undefined);

// 工具栏：搜索信息 SearchInfo 支持的字段
const searchNickName = ref("");
const searchSchool = ref("");
const searchMajor = ref("");
const searchAccount = ref("");

const hasSearch = computed(() => {
    return (
        !!searchNickName.value.trim() ||
        !!searchSchool.value.trim() ||
        !!searchMajor.value.trim() ||
        !!searchAccount.value.trim()
    );
});

/**
 * 构建搜索信息对象
 */
const buildSearchInfo = (): Api.User.UserManageListParams["SearchInfo"] | undefined => {
    const NickName = searchNickName.value.trim();
    const School = searchSchool.value.trim();
    const Major = searchMajor.value.trim();
    const Account = searchAccount.value.trim();

    if (!NickName && !School && !Major && !Account) return undefined;

    return {
        ...(NickName ? { NickName } : {}),
        ...(School ? { School } : {}),
        ...(Major ? { Major } : {}),
        ...(Account ? { Account } : {}),
    };
};

const requestSeq = ref(0);

/**
 * 获取用户列表
 */
const fetchUsers = async () => {
    const seq = ++requestSeq.value;
    loading.value = true;
    loadError.value = "";

    try {
        // 构建搜索参数
        const searchInfo = buildSearchInfo();
        const response = await selectUserList({
            Page: page.value,
            PageSize: pageSize.value,
            SearchInfo: searchInfo,
        });

        if (seq !== requestSeq.value) return;

        if (response.data.code === 0 && response.data.data) {
            users.value = response.data.data.List || [];
            total.value = response.data.data.Total || 0;
        } else {
            users.value = [];
            total.value = 0;
            loadError.value = response.data.message || "加载失败";
        }
    } catch (err) {
        console.error("获取用户列表失败:", err);
        users.value = [];
        total.value = 0;
        loadError.value = "网络异常，请稍后重试";
    } finally {
        if (seq === requestSeq.value) loading.value = false;
    }
};

/**
 * 处理查看用户详情
 * @param row 行数据
 */
const handleView = (row: Api.User.UserManageListItem) => {
    activeUserId.value = row._id;
    detailDrawerOpen.value = true;
};

/**
 * 处理编辑用户
 * @param row 行数据
 */
const handleEdit = (row: Api.User.UserManageListItem) => {
    editUserId.value = row._id;
    editDialogOpen.value = true;
};

/**
 * 处理删除用户
 * @param row 行数据
 */
const handleDelete = async (row: Api.User.UserManageListItem) => {
    try {
        await ElMessageBox.confirm(`确认删除用户「${row.NickName}」？此操作不可恢复。`, "删除确认", {
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
        const response = await deleteUser(row._id);
        if (response.data.code === 0) {
            const isLastItemOnPage = users.value.length <= 1 && page.value > 1;
            if (isLastItemOnPage) page.value -= 1;
            await fetchUsers();
        }
    } catch (err) {
        console.error("删除用户失败:", err);
    } finally {
        loading.value = false;
    }
};

/**
 * 处理表格行操作命令
 * @param command 命令
 * @param row 行数据
 */
const handleRowCommand = async (command: "view" | "edit" | "delete", row: Api.User.UserManageListItem) => {
    if (command === "view") return handleView(row);
    if (command === "edit") return handleEdit(row);
    if (command === "delete") return handleDelete(row);
};

const handlePageChange = (newPage: number) => {
    page.value = newPage;
};

const handleSizeChange = (newSize: number) => {
    pageSize.value = newSize;
    page.value = 1;
};

watch([page, pageSize], () => {
    fetchUsers();
});

watch(
    [searchNickName, searchSchool, searchMajor, searchAccount],
    () => {
        page.value = 1;
        debounce(() => {
            fetchUsers();
        });
    },
    { deep: false }
);

onMounted(() => {
    fetchUsers();
});
</script>

<template>
    <section class="admin-users" aria-label="用户管理">
        <!-- Toolbar -->
        <div class="toolbar oj-glass-panel">
            <div class="toolbar-left">
                <el-input
                    v-model="searchNickName"
                    class="toolbar-field"
                    :prefix-icon="Search"
                    clearable
                    placeholder="用户名（NickName）"
                />
                <el-input
                    v-model="searchSchool"
                    class="toolbar-field"
                    :prefix-icon="School"
                    clearable
                    placeholder="学校（School）"
                />
                <el-input
                    v-model="searchMajor"
                    class="toolbar-field"
                    :prefix-icon="Collection"
                    clearable
                    placeholder="专业（Major）"
                />
                <el-input
                    v-model="searchAccount"
                    class="toolbar-field"
                    :prefix-icon="User"
                    clearable
                    placeholder="账号（Account）"
                />
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
                        v-if="!users.length && !loadError"
                        :description="hasSearch ? '暂无匹配的用户' : '暂无用户数据'"
                    />

                    <el-empty v-else-if="!users.length && loadError" :description="loadError">
                        <el-button type="primary" plain @click="fetchUsers">重试加载</el-button>
                    </el-empty>

                    <div v-else>
                        <el-table :data="users" class="users-table" table-layout="fixed">
                            <el-table-column label="用户" min-width="200">
                                <template #default="scope">
                                    <div class="user-cell">
                                        <el-avatar class="user-avatar" :size="32">
                                            {{ scope.row.NickName?.slice(0, 1) }}
                                        </el-avatar>
                                        <div class="user-meta">
                                            <div class="user-name">{{ scope.row.NickName }}</div>
                                            <div class="user-sub">ID: {{ scope.row._id }}</div>
                                        </div>
                                    </div>
                                </template>
                            </el-table-column>

                            <el-table-column prop="School" label="学校" min-width="140" />
                            <el-table-column prop="Major" label="专业" min-width="160" />

                            <el-table-column label="个人简介" min-width="340">
                                <template #default="scope">
                                    <span class="muted-ellipsis" :title="scope.row.PersonalProfile">
                                        {{ scope.row.PersonalProfile || "-" }}
                                    </span>
                                </template>
                            </el-table-column>

                            <el-table-column label="加入时间" min-width="160">
                                <template #default="scope">
                                    <span class="time-text">{{ DateUtils.formatDateTime(scope.row.JoinTime) }}</span>
                                </template>
                            </el-table-column>

                            <el-table-column label="操作" width="80" fixed="right" align="center">
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
        <UserDetailDrawer v-model="detailDrawerOpen" :user-id="activeUserId" />
        <UserEditDialog v-model="editDialogOpen" :user-id="editUserId" @updated="fetchUsers" />
    </section>
</template>

<style scoped>
.admin-users {
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

.toolbar :deep(.el-input__inner) {
    color: var(--oj-input-text);
}

.toolbar :deep(.el-input__inner::placeholder) {
    color: var(--oj-input-placeholder);
}

.toolbar :deep(.el-input__prefix-inner) {
    color: var(--oj-text-color-secondary);
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
.users-table :deep(.el-table__inner-wrapper::before) {
    background-color: transparent;
}

.users-table :deep(.el-table__header-wrapper th.el-table__cell) {
    color: var(--oj-text-color-muted);
    border-bottom: 1px solid var(--oj-glass-border);
}

.users-table :deep(.el-table__body tr:hover > td.el-table__cell) {
    background: var(--oj-bg-hover);
}

.users-table :deep(.el-table__body td.el-table__cell) {
    border-bottom: 1px solid var(--oj-table-border);
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

.muted-ellipsis {
    display: inline-block;
    max-width: 100%;
    overflow: hidden;
    text-overflow: ellipsis;
    color: var(--oj-text-color-secondary);
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

.opt-btn {
    color: var(--oj-link-hover-color);
}

.pagination-bar {
    display: flex;
    justify-content: flex-end;
    padding: var(--oj-spacing-4);
    border-top: 1px solid var(--oj-glass-border);
}

@media (width <=640px) {
    .toolbar-field {
        width: 100%;
    }

    .pagination-bar {
        justify-content: center;
    }
}
</style>
