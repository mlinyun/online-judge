<script setup lang="ts">
/**
 * 管理后台 - 评论管理页面
 */
import { computed, onMounted, ref, watch } from "vue";
import { Delete, Edit, More, Search, User, View } from "@element-plus/icons-vue";
import { ElMessage, ElMessageBox } from "element-plus";

import { deleteComment, selectCommentListByAdmin } from "@/api/comment";
import type { Api } from "@/types/api/api";
import { DateUtils } from "@/utils/date/date-utils";

defineOptions({ name: "AdminComment" });

type ParentType = NonNullable<Api.Comment.SelectCommentListByAdminParams["SearchInfo"]>["ParentType"];

type ParentRow = Api.Comment.SelectCommentListByAdminItem & {
    __rowType: "parent";
    hasChildren: boolean;
};

type ChildRow = Api.Comment.ChildCommentListByAdminItem & {
    __rowType: "child";
    ParentId: Api.Comment.SelectCommentListByAdminItem["ParentId"];
    ParentType: ParentType;
};

type CommentRow = ParentRow | ChildRow;

const searchContent = ref("");
const searchUserId = ref("");
const searchParentType = ref<ParentType | "">("");

const page = ref(1);
const pageSize = ref(20);
const total = ref(0);

const loading = ref(false);
const loadError = ref<string | null>(null);
const comments = ref<ParentRow[]>([]);

const requestSeq = ref(0);
const childCache = ref(new Map<string, ChildRow[]>());

const hasSearch = computed(() => {
    return Boolean(searchParentType.value) || Boolean(searchContent.value.trim()) || Boolean(searchUserId.value.trim());
});

function getUserFromRow(row: CommentRow): Api.User.SimpleUserInfo | null {
    const raw = (row as { User?: unknown }).User;

    if (!raw) return null;
    if (Array.isArray(raw)) return (raw[0] as Api.User.SimpleUserInfo) ?? null;
    return raw as Api.User.SimpleUserInfo;
}

function getUserNickName(row: CommentRow): string {
    const user = getUserFromRow(row);
    return user?.NickName || "(未知用户)";
}

function getUserId(row: CommentRow): string {
    const user = getUserFromRow(row);
    return user?._id || "";
}

function getUserAvatar(row: CommentRow): string | undefined {
    const user = getUserFromRow(row);
    return user?.Avatar;
}

function getParentTypeLabel(type: ParentType): string {
    switch (type) {
        case "Announcement":
            return "公告";
        case "Discuss":
            return "讨论";
        case "Solution":
            return "题解";
        default:
            return String(type);
    }
}

function getParentTypeTagType(type: ParentType): "success" | "warning" | "info" {
    switch (type) {
        case "Announcement":
            return "info";
        case "Discuss":
            return "success";
        case "Solution":
            return "warning";
        default:
            return "info";
    }
}

function buildSearchInfo(): Api.Comment.SelectCommentListByAdminParams["SearchInfo"] | undefined {
    const content = searchContent.value.trim();
    const userId = searchUserId.value.trim();
    const parentType = searchParentType.value;

    const hasAny = Boolean(content) || Boolean(userId) || Boolean(parentType);
    if (!hasAny) return undefined;

    const info: NonNullable<Api.Comment.SelectCommentListByAdminParams["SearchInfo"]> = {};

    if (parentType) info.ParentType = parentType;
    if (content) info.Content = content;
    if (userId) info.UserId = userId;

    return info;
}

async function fetchComments(): Promise<void> {
    const current = ++requestSeq.value;
    loading.value = true;
    loadError.value = null;

    try {
        const SearchInfo = buildSearchInfo();
        const params: Api.Comment.SelectCommentListByAdminParams = {
            Page: page.value,
            PageSize: pageSize.value,
            ...(SearchInfo ? { SearchInfo } : {}),
        };

        const response = await selectCommentListByAdmin(params);
        if (current !== requestSeq.value) return;

        if (response.data.code === 0 && response.data.data) {
            const list = response.data.data.List || [];

            comments.value = list.map<ParentRow>((item) => ({
                ...item,
                __rowType: "parent",
                hasChildren: item.Child_Total > 0,
            }));
            total.value = response.data.data.Total || 0;
        } else {
            loadError.value = response.data.message || "加载失败";
            comments.value = [];
            total.value = 0;
        }
        childCache.value.clear();
    } catch (error) {
        if (current !== requestSeq.value) return;
        loadError.value = (error as Error)?.message || "加载失败";
        comments.value = [];
        total.value = 0;
    } finally {
        if (current === requestSeq.value) {
            loading.value = false;
        }
    }
}

function debounce<T extends (...args: never[]) => void>(fn: T, delayMs: number): T {
    let timer: number | undefined;
    return ((...args: never[]) => {
        if (timer) window.clearTimeout(timer);
        timer = window.setTimeout(() => fn(...args), delayMs);
    }) as T;
}

const debouncedSearch = debounce(() => {
    page.value = 1;
    fetchComments();
}, 350);

watch([searchContent, searchUserId, searchParentType], () => {
    debouncedSearch();
});

function handlePageChange(newPage: number): void {
    page.value = newPage;
    fetchComments();
}

function handleSizeChange(newSize: number): void {
    pageSize.value = newSize;
    page.value = 1;
    fetchComments();
}

async function handleDelete(row: CommentRow): Promise<void> {
    const id = row._id;
    const title = row.__rowType === "child" ? "删除子评论" : "删除评论";

    try {
        await ElMessageBox.confirm("确定要删除该评论吗？此操作不可撤销。", title, {
            type: "warning",
            confirmButtonText: "删除",
            cancelButtonText: "取消",
        });
    } catch {
        return;
    }

    try {
        await deleteComment(id);
        ElMessage.success("删除成功");

        const remaining = comments.value.length;
        if (page.value > 1 && remaining <= 1) {
            page.value -= 1;
        }
        await fetchComments();
    } catch (error) {
        ElMessage.error((error as Error)?.message || "删除失败");
    }
}

function handleView(row: CommentRow): void {
    const userName = getUserNickName(row);
    const userId = getUserId(row);

    const lines: string[] = [];
    lines.push(`ID: ${row._id}`);
    lines.push(`用户: ${userName}${userId ? ` (${userId})` : ""}`);

    if (row.__rowType === "parent") {
        lines.push(`类型: ${getParentTypeLabel(row.ParentType)}`);
        lines.push(`发布时间: ${DateUtils.formatDateTime(row.CreateTime)}`);
        lines.push(`点赞: ${row.Likes}`);
        lines.push(`子评论: ${row.Child_Total}`);
    } else {
        lines.push(`类型: 子评论`);
        lines.push(`发布时间: ${DateUtils.formatDateTime(row.CreateTime)}`);
    }

    lines.push("\n内容:");
    lines.push(row.Content);

    ElMessageBox.alert(lines.join("\n"), "评论详情", {
        confirmButtonText: "关闭",
    });
}

function handleEdit(row: CommentRow): void {
    ElMessageBox.prompt("请输入新的评论内容", "编辑评论", {
        inputValue: row.Content,
        inputType: "textarea",
        inputPlaceholder: "评论内容",
        confirmButtonText: "确定",
        cancelButtonText: "取消",
    })
        .then(() => {
            // 当前前端/类型定义中未发现“更新评论内容”的后端接口，先给出一致的 UI 与提示。
            ElMessage({
                type: "info",
                message: `评论编辑暂未接入后端接口（ID: ${row._id}）`,
                showClose: true,
            });
        })
        .catch(() => undefined);
}

function handleRowCommand(command: "view" | "edit" | "delete", row: CommentRow): void {
    if (command === "view") return handleView(row);
    if (command === "edit") return handleEdit(row);
    if (command === "delete") void handleDelete(row);
}

function loadChildren(row: ParentRow, _treeNode: unknown, resolve: (data: ChildRow[]) => void): void {
    const key = String(row._id);
    const cached = childCache.value.get(key);
    if (cached) {
        resolve(cached);
        return;
    }

    const children = (row.Child_Comments ?? []).map<ChildRow>((item) => ({
        ...item,
        __rowType: "child",
        ParentId: row.ParentId,
        ParentType: row.ParentType,
    }));

    childCache.value.set(key, children);
    resolve(children);
}

onMounted(() => {
    fetchComments();
});
</script>

<template>
    <section class="admin-comment" aria-label="评论管理">
        <div class="toolbar oj-glass-panel">
            <div class="toolbar-left">
                <el-input
                    v-model="searchContent"
                    class="toolbar-field"
                    :prefix-icon="Search"
                    clearable
                    placeholder="评论内容 (Content)"
                />

                <el-input
                    v-model="searchUserId"
                    class="toolbar-field"
                    :prefix-icon="User"
                    clearable
                    placeholder="用户ID (UserId)"
                />

                <el-select
                    v-model="searchParentType"
                    class="toolbar-field toolbar-select"
                    clearable
                    placeholder="所有类型"
                >
                    <el-option label="公告" value="Announcement" />
                    <el-option label="讨论" value="Discuss" />
                    <el-option label="题解" value="Solution" />
                </el-select>
            </div>
        </div>

        <div class="table-panel oj-glass-panel">
            <el-skeleton :loading="loading" animated>
                <template #template>
                    <div class="skeleton-block" />
                    <div class="skeleton-block" />
                    <div class="skeleton-block" />
                </template>

                <template #default>
                    <el-empty
                        v-if="!comments.length && !loadError"
                        :description="hasSearch ? '暂无匹配的评论' : '暂无评论数据'"
                    />

                    <el-empty v-else-if="!comments.length && loadError" :description="loadError">
                        <el-button type="primary" plain @click="fetchComments">重试加载</el-button>
                    </el-empty>

                    <div v-else>
                        <el-table
                            :data="comments"
                            class="comment-table"
                            table-layout="fixed"
                            row-key="_id"
                            lazy
                            :load="loadChildren"
                            :tree-props="{ children: 'children', hasChildren: 'hasChildren' }"
                        >
                            <el-table-column label="评论内容" min-width="360">
                                <template #default="scope">
                                    <p class="content-clamp" :title="scope.row.Content">{{ scope.row.Content }}</p>
                                </template>
                            </el-table-column>

                            <el-table-column label="类型" width="120" align="center">
                                <template #default="scope">
                                    <template v-if="scope.row.__rowType === 'parent'">
                                        <el-tag :type="getParentTypeTagType(scope.row.ParentType)">
                                            {{ getParentTypeLabel(scope.row.ParentType) }}
                                        </el-tag>
                                    </template>
                                    <span v-else class="muted-text">-</span>
                                </template>
                            </el-table-column>

                            <el-table-column label="用户" width="160">
                                <template #default="scope">
                                    <div class="user-cell">
                                        <el-avatar :size="28" :src="getUserAvatar(scope.row)">
                                            {{ getUserNickName(scope.row).slice(0, 1) }}
                                        </el-avatar>
                                        <div class="user-meta">
                                            <span class="user-name">{{ getUserNickName(scope.row) }}</span>
                                            <span class="user-id">{{ getUserId(scope.row) }}</span>
                                        </div>
                                    </div>
                                </template>
                            </el-table-column>

                            <el-table-column label="点赞" width="96" align="center">
                                <template #default="scope">
                                    <template v-if="scope.row.__rowType === 'parent'">
                                        <span class="muted-text">{{ scope.row.Likes }}</span>
                                    </template>
                                    <span v-else class="muted-text">-</span>
                                </template>
                            </el-table-column>

                            <el-table-column label="子评论" width="96" align="center">
                                <template #default="scope">
                                    <template v-if="scope.row.__rowType === 'parent'">
                                        <span class="muted-text">{{ scope.row.Child_Total }}</span>
                                    </template>
                                    <span v-else class="muted-text">-</span>
                                </template>
                            </el-table-column>

                            <el-table-column label="发布时间" width="170">
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
.admin-comment {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-6);
}

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

.table-panel {
    overflow: hidden;
    border-radius: var(--oj-radius-xl);
}

.skeleton-block {
    height: 44px;
    margin: var(--oj-spacing-4);
    border-radius: var(--oj-radius-lg);
}

.comment-table :deep(.el-table__inner-wrapper::before) {
    background-color: transparent;
}

.comment-table :deep(.el-table__header-wrapper th.el-table__cell) {
    color: var(--oj-text-color-muted);
    background: var(--oj-table-header-bg);
    border-bottom: 1px solid var(--oj-glass-border);
}

.comment-table :deep(.el-table__body tr:hover > td.el-table__cell) {
    background: var(--oj-table-row-hover-bg);
}

.comment-table :deep(.el-table__body td.el-table__cell) {
    border-bottom: 1px solid var(--oj-table-border);
}

.content-clamp {
    display: -webkit-box;
    margin: 0;
    overflow: hidden;
    -webkit-line-clamp: 2;
    line-height: 1.4;
    color: var(--oj-text-color);
    -webkit-box-orient: vertical;
}

.id-mono {
    font-family: var(--oj-font-family-mono), monospace;
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-secondary);
}

.user-cell {
    display: flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    min-width: 0;
}

.user-meta {
    display: flex;
    flex-direction: column;
    min-width: 0;
}

.user-name {
    overflow: hidden;
    text-overflow: ellipsis;
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
    white-space: nowrap;
}

.user-id {
    font-family: var(--oj-font-family-mono), monospace;
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-muted);
    white-space: nowrap;
}

.time-text {
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-secondary);
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
