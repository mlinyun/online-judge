<script setup lang="ts">
import { computed, ref, watch } from "vue";
import { Close, RefreshRight, UserFilled } from "@element-plus/icons-vue";
import { selectUserInfo } from "@/api/user";
import { DateUtils } from "@/utils/date/date-utils";
import type { Api } from "@/types/api/api";

defineOptions({ name: "UserDetailDrawer" });

const props = defineProps<{
    modelValue: boolean;
    userId?: Api.User.UserId;
}>();

const emit = defineEmits<{
    (e: "update:modelValue", value: boolean): void;
}>();

const open = computed({
    get: () => props.modelValue,
    set: (val: boolean) => emit("update:modelValue", val),
});

const loading = ref(false);
const loadError = ref("");
const user = ref<Api.User.SelectUserInfoResult | null>(null);

const requestSeq = ref(0);

const titleText = computed(() => {
    if (user.value?.NickName) return user.value.NickName;
    if (props.userId) return `用户 ${String(props.userId)}`;
    return "用户详情";
});

const passRateText = computed(() => {
    const submit = Number(user.value?.SubmitNum || 0);
    const ac = Number(user.value?.ACNum || 0);
    if (!submit) return "-";
    return `${((ac / submit) * 100).toFixed(1)}%`;
});

const solvesPreview = computed(() => {
    const list = user.value?.Solves || [];
    return list.slice(0, 8);
});

const fetchUser = async () => {
    if (!props.userId) return;

    const seq = ++requestSeq.value;
    loading.value = true;
    loadError.value = "";

    try {
        const response = await selectUserInfo(props.userId);
        if (seq !== requestSeq.value) return;

        if (response.data.code === 0 && response.data.data) {
            user.value = response.data.data;
        } else {
            user.value = null;
            loadError.value = response.data.message || "加载失败";
        }
    } catch (err) {
        console.error("获取用户详情失败:", err);
        user.value = null;
        loadError.value = "网络异常，请稍后重试";
    } finally {
        if (seq === requestSeq.value) loading.value = false;
    }
};

watch(
    () => [props.modelValue, props.userId] as const,
    ([visible, userId]) => {
        if (!visible) return;
        if (!userId) {
            user.value = null;
            loadError.value = "请选择一个用户";
            return;
        }
        fetchUser();
    },
    { immediate: false }
);
</script>

<template>
    <el-drawer
        v-model="open"
        :append-to-body="true"
        :with-header="false"
        direction="rtl"
        size="480px"
        class="user-drawer"
    >
        <div class="drawer-shell">
            <header class="drawer-header">
                <div class="header-left">
                    <el-avatar class="avatar" :size="44">
                        {{ user?.NickName?.slice(0, 1) || "U" }}
                    </el-avatar>
                    <div class="header-meta">
                        <div class="header-title">{{ titleText }}</div>
                        <div class="header-sub">ID: {{ String(user?._id || userId || "-") }}</div>
                    </div>
                </div>

                <div class="header-right">
                    <el-button class="icon-btn" circle :icon="RefreshRight" :disabled="loading" @click="fetchUser" />
                    <el-button class="icon-btn" circle :icon="Close" @click="open = false" />
                </div>
            </header>

            <div class="drawer-body">
                <el-skeleton :loading="loading" animated>
                    <template #template>
                        <div class="skeleton-block" />
                        <div class="skeleton-block" />
                        <div class="skeleton-block" />
                    </template>

                    <template #default>
                        <el-empty v-if="loadError" :description="loadError">
                            <el-button type="primary" plain @click="fetchUser">重试加载</el-button>
                        </el-empty>

                        <template v-else>
                            <section class="card oj-glass-panel">
                                <div class="card-title">
                                    <el-icon class="title-icon">
                                        <UserFilled />
                                    </el-icon>
                                    基本信息
                                </div>

                                <div class="kv-grid">
                                    <div class="kv">
                                        <div class="k">昵称</div>
                                        <div class="v">{{ user?.NickName || "-" }}</div>
                                    </div>
                                    <div class="kv">
                                        <div class="k">学校</div>
                                        <div class="v">{{ user?.School || "-" }}</div>
                                    </div>
                                    <div class="kv">
                                        <div class="k">专业</div>
                                        <div class="v">{{ user?.Major || "-" }}</div>
                                    </div>
                                    <div class="kv">
                                        <div class="k">加入时间</div>
                                        <div class="v">
                                            {{ user?.JoinTime ? DateUtils.formatDateTime(user.JoinTime) : "-" }}
                                        </div>
                                    </div>
                                </div>

                                <div class="profile">
                                    <div class="k">个人简介</div>
                                    <div class="v">{{ user?.PersonalProfile || "-" }}</div>
                                </div>
                            </section>

                            <section class="card oj-glass-panel">
                                <div class="card-title">数据概览</div>

                                <div class="stat-grid">
                                    <div class="stat">
                                        <div class="stat-k">提交</div>
                                        <div class="stat-v">{{ user?.SubmitNum ?? 0 }}</div>
                                    </div>
                                    <div class="stat">
                                        <div class="stat-k">通过</div>
                                        <div class="stat-v">{{ user?.ACNum ?? 0 }}</div>
                                    </div>
                                    <div class="stat">
                                        <div class="stat-k">通过率</div>
                                        <div class="stat-v">{{ passRateText }}</div>
                                    </div>
                                </div>

                                <div class="solves">
                                    <div class="k">已解决题目</div>
                                    <div class="v">
                                        <div class="solves-meta">共 {{ user?.Solves?.length ?? 0 }} 道</div>
                                        <div v-if="solvesPreview.length" class="solves-tags">
                                            <el-tag
                                                v-for="pid in solvesPreview"
                                                :key="pid"
                                                class="solve-tag"
                                                size="small"
                                                type="info"
                                            >
                                                {{ pid }}
                                            </el-tag>
                                            <span
                                                v-if="(user?.Solves?.length ?? 0) > solvesPreview.length"
                                                class="muted"
                                            >
                                                +{{ (user?.Solves?.length ?? 0) - solvesPreview.length }}
                                            </span>
                                        </div>
                                        <span v-else class="muted">-</span>
                                    </div>
                                </div>
                            </section>
                        </template>
                    </template>
                </el-skeleton>
            </div>
        </div>
    </el-drawer>
</template>

<style scoped>
.user-drawer {
    --el-color-primary: rgb(var(--oj-color-primary-rgb));
    --el-drawer-padding-primary: 0px;
}

.user-drawer :deep(.el-drawer) {
    overflow: hidden;
    background: var(--oj-glass-bg);
    border-left: 1px solid var(--oj-glass-border);
    backdrop-filter: blur(var(--oj-glass-blur));
}

.user-drawer :deep(.el-drawer__body) {
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

.card + .card {
    margin-top: var(--oj-spacing-4);
}

.card-title {
    display: flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    margin-bottom: var(--oj-spacing-3);
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
}

.title-icon {
    color: rgb(var(--oj-color-primary-rgb));
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

.profile {
    margin-top: var(--oj-spacing-3);
}

.profile .v {
    display: -webkit-box;
    overflow: hidden;
    -webkit-line-clamp: 4;
    white-space: normal;
    -webkit-box-orient: vertical;
}

.stat-grid {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: var(--oj-spacing-3);
}

.stat {
    padding: var(--oj-spacing-3);
    background: rgb(var(--oj-color-primary-rgb) / 6%);
    border: 1px solid rgb(var(--oj-color-primary-rgb) / 16%);
    border-radius: var(--oj-radius-lg);
}

.stat-k {
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-muted);
}

.stat-v {
    font-size: var(--oj-font-size-lg);
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
}

.solves {
    margin-top: var(--oj-spacing-4);
}

.solves-meta {
    margin-bottom: var(--oj-spacing-2);
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-muted);
}

.solves-tags {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-2);
    align-items: center;
}

.solve-tag {
    flex: 0 0 auto;
}

.muted {
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-muted);
}

.skeleton-block {
    height: 44px;
    margin-bottom: var(--oj-spacing-3);
    border-radius: var(--oj-radius-lg);
}

@media (width <=640px) {
    .user-drawer :deep(.el-drawer) {
        width: 92vw !important;
    }

    .kv-grid {
        grid-template-columns: 1fr;
    }

    .stat-grid {
        grid-template-columns: 1fr;
    }
}
</style>
