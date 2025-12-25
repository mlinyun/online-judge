<script setup lang="ts">
import { computed, ref, watch } from "vue";
import { ArrowLeft, Close, RefreshRight } from "@element-plus/icons-vue";
import { DateUtils } from "@/utils/date/date-utils";
import type { Api } from "@/types/api/api";

defineOptions({ name: "CommentDetailDrawer" });

type ParentType = Api.Comment.SelectCommentListByAdminItem["ParentType"];

type ParentRow = Api.Comment.SelectCommentListByAdminItem & {
    __rowType: "parent";
};

type ChildRow = Api.Comment.ChildCommentListByAdminItem & {
    __rowType: "child";
    ParentId: Api.Comment.SelectCommentListByAdminItem["ParentId"];
    ParentType: ParentType;
    FatherCommentId: Api.Comment.CommentId;
};

type CommentRow = ParentRow | ChildRow;

const props = defineProps<{
    modelValue: boolean;
    row?: CommentRow | null;
    parentRow?: ParentRow | null;
}>();

const emit = defineEmits<{
    (e: "update:modelValue", value: boolean): void;
    (e: "refresh"): void;
}>();

const open = computed({
    get: () => props.modelValue,
    set: (val: boolean) => emit("update:modelValue", val),
});

const normalizeUser = (raw: unknown): Api.User.SimpleUserInfo | null => {
    if (!raw) return null;
    if (Array.isArray(raw)) return (raw[0] as Api.User.SimpleUserInfo) ?? null;
    return raw as Api.User.SimpleUserInfo;
};

const resolvedParent = computed<ParentRow | null>(() => {
    if (props.parentRow) return props.parentRow;
    if (props.row?.__rowType === "parent") return props.row;
    return null;
});

const viewMode = ref<"parent" | "child">("parent");
const activeChild = ref<ChildRow | null>(null);

const parentUser = computed<Api.User.SimpleUserInfo | null>(() => {
    return normalizeUser((resolvedParent.value as { User?: unknown } | null)?.User);
});

const activeChildUser = computed<Api.User.SimpleUserInfo | null>(() => {
    return normalizeUser((activeChild.value as { User?: unknown } | null)?.User);
});

const getChildUser = (child: ChildRow): Api.User.SimpleUserInfo | null => {
    return normalizeUser((child as { User?: unknown }).User);
};

const mapChildFromParent = (parent: ParentRow, item: Api.Comment.ChildCommentListByAdminItem): ChildRow => {
    return {
        ...item,
        __rowType: "child",
        ParentId: parent.ParentId,
        ParentType: parent.ParentType,
        FatherCommentId: parent._id,
    };
};

const children = computed<ChildRow[]>(() => {
    const parent = resolvedParent.value;
    if (!parent) return [];
    return (parent.Child_Comments ?? []).map((item) => mapChildFromParent(parent, item));
});

const childTotal = computed(() => {
    const parent = resolvedParent.value;
    return Number(parent?.Child_Total ?? 0);
});

const visibleChildren = computed(() => {
    if (childTotal.value <= 0) return [];

    return children.value.filter((child) => {
        const user = getChildUser(child);
        const hasUser = Boolean(user?._id) || Boolean(user?.NickName);
        const hasId = Boolean(child._id);
        const hasContent = Boolean(child.Content?.trim?.());

        return hasId && (hasContent || hasUser);
    });
});

const ensureViewState = () => {
    const row = props.row;

    if (!row) {
        viewMode.value = "parent";
        activeChild.value = null;
        return;
    }

    if (row.__rowType === "child") {
        viewMode.value = "child";
        activeChild.value = row;
        return;
    }

    viewMode.value = "parent";
    activeChild.value = null;
};

watch(
    () => [props.modelValue, props.row, props.parentRow] as const,
    ([visible]) => {
        if (!visible) return;
        ensureViewState();
    },
    { immediate: false }
);

const avatarText = computed(() => {
    if (viewMode.value === "child") {
        const name = activeChildUser.value?.NickName || "C";
        return name.slice(0, 1);
    }
    const name = parentUser.value?.NickName || "C";
    return name.slice(0, 1);
});

const headerAvatar = computed(() => {
    if (viewMode.value === "child") return activeChildUser.value?.Avatar;
    return parentUser.value?.Avatar;
});

const titleText = computed(() => {
    if (viewMode.value === "child") return "评论详情（回复）";
    return "评论详情（评论）";
});

const subtitleText = computed(() => {
    if (viewMode.value === "child") {
        const child = activeChild.value;
        if (child) return `ID: ${String(child._id)} · 评论: ${String(child.FatherCommentId)}`;
        return "-";
    }

    const parent = resolvedParent.value;
    if (parent) return `ID: ${String(parent._id)} · 回复: ${parent.Child_Total ?? 0}`;
    return "-";
});

const getParentTypeLabel = (type: ParentType): string => {
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
};

const showChild = (child: ChildRow) => {
    viewMode.value = "child";
    activeChild.value = child;
};

const backToParent = () => {
    if (!resolvedParent.value) return;
    viewMode.value = "parent";
    activeChild.value = null;
};

const handleRefresh = () => {
    emit("refresh");
};
</script>

<template>
    <el-drawer v-model="open" :append-to-body="true" :with-header="false" direction="rtl" size="520px"
        class="comment-drawer">
        <div class="drawer-shell">
            <header class="drawer-header">
                <div class="header-left">
                    <el-avatar class="avatar" :size="44" :src="headerAvatar">{{ avatarText }}</el-avatar>
                    <div class="header-meta">
                        <div class="header-title">{{ titleText }}</div>
                        <div class="header-sub">{{ subtitleText }}</div>
                    </div>
                </div>

                <div class="header-right">
                    <el-button v-if="viewMode === 'child' && resolvedParent" class="icon-btn" circle :icon="ArrowLeft"
                        @click="backToParent" />
                    <el-button class="icon-btn" circle :icon="RefreshRight" @click="handleRefresh" />
                    <el-button class="icon-btn" circle :icon="Close" @click="open = false" />
                </div>
            </header>

            <div class="drawer-body">
                <el-empty v-if="!row" description="请选择一条评论查看详情" />

                <template v-else>
                    <section v-if="viewMode === 'parent'" class="card oj-glass-panel">
                        <div class="card-title">基本信息</div>
                        <div class="kv-grid">
                            <div class="kv">
                                <div class="k">用户</div>
                                <div class="v">{{ parentUser?.NickName || "-" }}</div>
                            </div>
                            <div class="kv">
                                <div class="k">用户ID</div>
                                <div class="v mono">{{ parentUser?._id || "-" }}</div>
                            </div>
                            <div class="kv">
                                <div class="k">父类型</div>
                                <div class="v">
                                    {{ resolvedParent ? getParentTypeLabel(resolvedParent.ParentType) : "-" }}
                                </div>
                            </div>
                            <div class="kv">
                                <div class="k">父级ID</div>
                                <div class="v mono">{{ String(resolvedParent?.ParentId || "-") }}</div>
                            </div>
                            <div class="kv">
                                <div class="k">发布时间</div>
                                <div class="v">
                                    {{
                                        resolvedParent?.CreateTime
                                            ? DateUtils.formatDateTime(resolvedParent.CreateTime)
                                            : "-"
                                    }}
                                </div>
                            </div>
                            <div class="kv">
                                <div class="k">点赞</div>
                                <div class="v">{{ resolvedParent?.Likes ?? 0 }}</div>
                            </div>
                        </div>

                        <div class="profile">
                            <div class="k">内容</div>
                            <div class="v profile-text">{{ resolvedParent?.Content || "-" }}</div>
                        </div>
                    </section>

                    <section v-if="viewMode === 'parent' && childTotal > 0" class="card oj-glass-panel">
                        <div class="card-title">回复</div>

                        <div class="child-list">
                            <div v-for="child in visibleChildren" :key="child._id" class="child-item">
                                <div class="child-top">
                                    <div class="child-user">
                                        <el-avatar :size="26" :src="getChildUser(child)?.Avatar">
                                            {{ getChildUser(child)?.NickName?.slice(0, 1) || "U" }}
                                        </el-avatar>
                                        <div class="child-meta">
                                            <div class="child-name">
                                                {{ getChildUser(child)?.NickName || "(未知用户)" }}
                                            </div>
                                            <div class="child-id mono">{{ getChildUser(child)?._id || "-" }}</div>
                                        </div>
                                    </div>

                                    <div class="child-actions">
                                        <div class="child-time">{{ DateUtils.formatDateTime(child.CreateTime) }}</div>
                                        <el-button class="child-view" link type="primary"
                                            @click="showChild(child)">查看</el-button>
                                    </div>
                                </div>
                                <div class="child-content" :title="child.Content">{{ child.Content }}</div>
                            </div>
                        </div>
                    </section>

                    <section v-if="viewMode === 'child'" class="card oj-glass-panel">
                        <div class="card-title">回复信息</div>

                        <div class="kv-grid">
                            <div class="kv">
                                <div class="k">用户</div>
                                <div class="v">{{ activeChildUser?.NickName || "-" }}</div>
                            </div>
                            <div class="kv">
                                <div class="k">用户ID</div>
                                <div class="v mono">{{ activeChildUser?._id || "-" }}</div>
                            </div>
                            <div class="kv">
                                <div class="k">评论ID</div>
                                <div class="v mono">{{ String(activeChild?.FatherCommentId || "-") }}</div>
                            </div>
                            <div class="kv">
                                <div class="k">父类型</div>
                                <div class="v">
                                    {{ activeChild ? getParentTypeLabel(activeChild.ParentType) : "-" }}
                                </div>
                            </div>
                            <div class="kv">
                                <div class="k">父级ID</div>
                                <div class="v mono">{{ String(activeChild?.ParentId || "-") }}</div>
                            </div>
                            <div class="kv">
                                <div class="k">发布时间</div>
                                <div class="v">
                                    {{
                                        activeChild?.CreateTime ? DateUtils.formatDateTime(activeChild.CreateTime) : "-"
                                    }}
                                </div>
                            </div>
                        </div>

                        <div class="profile">
                            <div class="k">内容</div>
                            <div class="v profile-text">{{ activeChild?.Content || "-" }}</div>
                        </div>

                        <div v-if="resolvedParent" class="parent-preview">
                            <div class="k">评论内容预览</div>
                            <div class="v parent-preview-text">{{ resolvedParent.Content }}</div>
                        </div>
                    </section>
                </template>
            </div>
        </div>
    </el-drawer>
</template>

<style scoped>
.comment-drawer {
    --el-color-primary: rgb(var(--oj-color-primary-rgb));
    --el-drawer-padding-primary: 0px;
}

.comment-drawer :deep(.el-drawer) {
    overflow: hidden;
    background: var(--oj-glass-bg);
    border-left: 1px solid var(--oj-glass-border);
    backdrop-filter: blur(var(--oj-glass-blur));
}

.comment-drawer :deep(.el-drawer__body) {
    padding: 0;
}

.drawer-shell {
    position: relative;
    display: flex;
    flex-direction: column;
    height: 100%;
    overflow: hidden;
    border-radius: var(--oj-radius-xl);
}

.drawer-shell::before {
    position: absolute;
    inset: 0;
    pointer-events: none;
    content: "";
    background:
        radial-gradient(620px circle at 20% 10%, rgb(var(--oj-color-primary-rgb) / 18%) 0%, transparent 60%),
        radial-gradient(520px circle at 110% 0%, rgb(var(--oj-color-primary-rgb) / 10%) 0%, transparent 58%),
        repeating-linear-gradient(90deg, rgb(var(--oj-color-primary-rgb) / 6%) 0 1px, transparent 1px 16px);
    opacity: 0.9;
}

.drawer-shell::after {
    position: absolute;
    inset: 0;
    pointer-events: none;
    content: "";
    box-shadow:
        0 0 0 1px rgb(var(--oj-color-primary-rgb) / 10%) inset,
        0 18px 48px rgb(var(--oj-color-primary-rgb) / 10%);
    opacity: 0.75;
}

.drawer-header {
    position: relative;
    z-index: 1;
    display: flex;
    gap: var(--oj-spacing-4);
    align-items: center;
    justify-content: space-between;
    padding: var(--oj-spacing-4);
    border-bottom: 1px solid var(--oj-glass-border);
}

.header-left {
    display: flex;
    gap: var(--oj-spacing-3);
    align-items: center;
    min-width: 0;
}

.avatar {
    border: 1px solid rgb(var(--oj-color-primary-rgb) / 60%);
}

.header-meta {
    display: flex;
    flex-direction: column;
    min-width: 0;
}

.header-title {
    overflow: hidden;
    text-overflow: ellipsis;
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
    white-space: nowrap;
}

.header-sub {
    overflow: hidden;
    text-overflow: ellipsis;
    font-family: var(--oj-font-family-mono), monospace;
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-muted);
    white-space: nowrap;
}

.header-right {
    display: flex;
    gap: var(--oj-spacing-2);
    align-items: center;
}

.icon-btn {
    --el-button-bg-color: rgb(var(--oj-color-primary-rgb) / 10%);
    --el-button-border-color: rgb(var(--oj-color-primary-rgb) / 25%);
    --el-button-hover-bg-color: rgb(var(--oj-color-primary-rgb) / 16%);
    --el-button-hover-border-color: rgb(var(--oj-color-primary-rgb) / 38%);
    --el-button-active-bg-color: rgb(var(--oj-color-primary-rgb) / 18%);
    --el-button-active-border-color: rgb(var(--oj-color-primary-rgb) / 45%);
    --el-button-text-color: var(--oj-text-color);
}

.drawer-body {
    position: relative;
    z-index: 1;
    flex: 1;
    padding: var(--oj-spacing-4);
    overflow: auto;
}

.card {
    padding: var(--oj-spacing-4);
    border-radius: var(--oj-radius-xl);
}

.card+.card {
    margin-top: var(--oj-spacing-4);
}

.card-title {
    margin-bottom: var(--oj-spacing-3);
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
}

.kv-grid {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: var(--oj-spacing-3);
}

.kv {
    min-width: 0;
}

.k {
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-muted);
}

.v {
    overflow: hidden;
    text-overflow: ellipsis;
    color: var(--oj-text-color-secondary);
    white-space: nowrap;
}

.mono {
    font-family: var(--oj-font-family-mono), monospace;
}

.profile {
    margin-top: var(--oj-spacing-3);
}

.profile-text {
    white-space: pre-wrap;
}

.child-list {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-3);
}

.child-item {
    padding: var(--oj-spacing-3);
    background: rgb(var(--oj-color-primary-rgb) / 6%);
    border: 1px solid rgb(var(--oj-color-primary-rgb) / 16%);
    border-radius: var(--oj-radius-lg);
}

.child-top {
    display: flex;
    gap: var(--oj-spacing-3);
    align-items: flex-start;
    justify-content: space-between;
}

.child-user {
    display: flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    min-width: 0;
}

.child-meta {
    min-width: 0;
}

.child-name {
    overflow: hidden;
    text-overflow: ellipsis;
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
    white-space: nowrap;
}

.child-id {
    overflow: hidden;
    text-overflow: ellipsis;
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-muted);
    white-space: nowrap;
}

.child-actions {
    display: flex;
    flex-shrink: 0;
    flex-direction: column;
    gap: var(--oj-spacing-1);
    align-items: flex-end;
}

.child-time {
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-muted);
}

.child-view {
    padding: 0;
}

.child-content {
    display: -webkit-box;
    margin-top: var(--oj-spacing-2);
    overflow: hidden;
    -webkit-line-clamp: 3;
    line-height: 1.45;
    color: var(--oj-text-color-secondary);
    -webkit-box-orient: vertical;
}

.parent-preview {
    margin-top: var(--oj-spacing-4);
}

.parent-preview-text {
    display: -webkit-box;
    overflow: hidden;
    -webkit-line-clamp: 4;
    white-space: normal;
    -webkit-box-orient: vertical;
}

.muted {
    color: var(--oj-text-color-muted);
}

@media (width <=640px) {
    .comment-drawer :deep(.el-drawer) {
        width: 92vw !important;
    }

    .kv-grid {
        grid-template-columns: 1fr;
    }
}
</style>
