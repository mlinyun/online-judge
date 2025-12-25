<script setup lang="ts">
import { computed, onMounted, onUnmounted, ref } from "vue";
import { selectUserRank } from "@/api/user";
import type { Api } from "@/types/api/api";
import type { TableInstance } from "element-plus";
import { UserFilled } from "@element-plus/icons-vue";

defineOptions({ name: "UserRank" });

const PAGE_SIZE = 20; // 每页加载 20 条
const MAX_ITEMS = 100; // 最多加载 100 条用户排名

// 用户排名数据列表
const rankList = ref<Api.User.UserRankListItem[]>([]);
// 用户排名总数
const total = ref<number>(0);
// 当前页码
const currentPage = ref<number>(1);

// 加载状态
const initialLoading = ref<boolean>(false);
// 是否正在加载更多
const loadingMore = ref<boolean>(false);
// 加载错误信息
const loadError = ref<string>("");

// 触底哨兵元素
const sentinelRef = ref<HTMLElement | null>(null);
let intersectionObserver: IntersectionObserver | null = null;

// 表格布局方式
const tableLayout = ref<TableInstance["tableLayout"]>("fixed");

const detailDrawerOpen = ref(false);
const activeUserId = ref<Api.User.UserId | undefined>(undefined);

const safeTotal = computed(() => total.value);

/**
 * 限制的总数，最大为 MAX_ITEMS
 */
const cappedTotal = computed(() => {
    const t = safeTotal.value;
    // Total=0 且当前没有任何数据，表示无可加载内容
    if (t <= 0 && rankList.value.length === 0) return 0;
    // Total 未知/0 但已拿到部分数据时，按 MAX_ITEMS 上限继续尝试加载
    if (t <= 0) return MAX_ITEMS;
    return Math.min(t, MAX_ITEMS);
});

/**
 * 是否可以加载更多
 */
const canLoadMore = computed(() => {
    if (initialLoading.value || loadingMore.value) return false;
    if (rankList.value.length >= cappedTotal.value) return false;
    // 已知总数时按总数限制；未知/0 时按 MAX_ITEMS 限制
    return rankList.value.length < MAX_ITEMS;
});

/**
 * 排名前 3 名用户
 */
const topThree = computed(() => {
    return [...rankList.value].filter((item) => item.Rank >= 1 && item.Rank <= 3).sort((a, b) => a.Rank - b.Rank);
});

/**
 * 合并并排序用户排名列表，去重后按 Rank 升序排列，只保留前 MAX_ITEMS 条
 * @param incoming 新获取的用户排名列表
 */
const mergeAndSort = (incoming: Api.User.UserRankListItem[]) => {
    const map = new Map<string, Api.User.UserRankListItem>();
    for (const item of rankList.value) map.set(String(item._id), item);
    for (const item of incoming) map.set(String(item._id), item);

    rankList.value = Array.from(map.values())
        .sort((a, b) => a.Rank - b.Rank)
        .slice(0, MAX_ITEMS);
};

/**
 * 获取指定页的用户排名
 * @param page 页码
 */
const fetchPage = async (page: number) => {
    const res = await selectUserRank({
        Page: page,
        PageSize: PAGE_SIZE,
    });

    if (res.data.success && res.data.data) {
        total.value = Number(res.data.data.Total ?? 0);
        mergeAndSort((res.data.data.List ?? []) as Api.User.UserRankListItem[]);
        loadError.value = "";
        return;
    }

    // 业务失败：拦截器已提示，这里只保留页面态
    loadError.value = res.data.message || "加载失败";
};

/**
 * 刷新用户排名列表，在初加载和重试时调用
 */
const refresh = async () => {
    initialLoading.value = true;
    loadingMore.value = false;
    loadError.value = "";
    currentPage.value = 1;
    rankList.value = [];
    total.value = 0;

    try {
        await fetchPage(1);
    } catch (e) {
        console.error("获取用户排名失败:", e);
        loadError.value = "网络异常，请稍后重试";
    } finally {
        initialLoading.value = false;
    }
};

/**
 * 加载更多用户排名
 */
const loadMore = async () => {
    if (!canLoadMore.value) return;

    // 最多加载到第 100 名，即 5 页（20 * 5）
    const maxPageByCap = Math.ceil(MAX_ITEMS / PAGE_SIZE);
    const maxPageByTotal =
        safeTotal.value > 0 ? Math.ceil(Math.min(safeTotal.value, MAX_ITEMS) / PAGE_SIZE) : maxPageByCap;
    const nextPage = currentPage.value + 1;
    if (nextPage > Math.min(maxPageByCap, maxPageByTotal)) return;

    loadingMore.value = true;
    try {
        await fetchPage(nextPage);
        currentPage.value = nextPage;
    } catch (e) {
        console.error("加载更多用户排名失败:", e);
        // 不打断已有内容，只提示页面态
        loadError.value = "网络异常，加载更多失败";
    } finally {
        loadingMore.value = false;
    }
};

onMounted(async () => {
    await refresh();

    intersectionObserver = new IntersectionObserver(
        (entries) => {
            const [entry] = entries;
            if (!entry?.isIntersecting) return;
            void loadMore();
        },
        {
            root: null,
            // 提前 100px 触发，滚动体验更平滑
            rootMargin: "100px",
            threshold: 0,
        }
    );

    if (sentinelRef.value) {
        intersectionObserver.observe(sentinelRef.value);
    }
});

onUnmounted(() => {
    intersectionObserver?.disconnect();
    intersectionObserver = null;
});

const handleRowClick = (row: Api.User.UserRankListItem) => {
    activeUserId.value = row._id;
    detailDrawerOpen.value = true;
};

/**
 * 获取排名颜色
 * @param rank 排名
 * @returns 颜色字符串
 */
const getRankColor = (rank: number) => {
    // 使用设计 Token，确保深/浅色主题一致
    if (rank === 1) return "var(--oj-color-warning-light)";
    if (rank === 2) return "var(--oj-text-color-secondary)";
    if (rank === 3) return "var(--oj-color-warning-dark)";
    return "var(--el-text-color-secondary)";
};

/**
 * 格式化通过率
 * @param item 用户排名项
 * @returns 通过率字符串
 */
const formatRate = (item: Api.User.UserRankListItem): string => {
    const submit = Number(item.SubmitNum ?? 0);
    const ac = Number(item.ACNum ?? 0);
    if (submit <= 0) return "0.0%";
    return `${((ac / submit) * 100).toFixed(1)}%`;
};

/**
 * 获取用户个性签名文本
 * @param item 用户排名项
 * @returns 个性签名文本
 */
const getProfileText = (item: Api.User.UserRankListItem): string => {
    const profile = (item.PersonalProfile ?? "").trim();
    return profile || "这位大佬很低调，什么都没写~";
};
</script>

<template>
    <div class="user-rank-page">
        <!-- Top 3 Podium -->
        <section v-if="topThree.length" class="podium-section">
            <div class="podium-grid">
                <!-- 2nd -->
                <div v-if="topThree[1]" class="podium-item podium-item--second">
                    <div class="podium-badge podium-badge--second">2</div>
                    <el-card class="podium-card podium-card--second">
                        <el-avatar
                            class="podium-avatar podium-avatar--second"
                            fit="fill"
                            shape="square"
                            :size="72"
                            :src="topThree[1].Avatar"
                        />
                        <div class="podium-name">{{ topThree[1].NickName }}</div>
                        <div class="podium-profile">{{ getProfileText(topThree[1]) }}</div>
                        <div class="podium-stat podium-stat--second">{{ topThree[1].ACNum }} Solved</div>
                    </el-card>
                </div>

                <!-- 1st -->
                <div v-if="topThree[0]" class="podium-item podium-item--first">
                    <div class="podium-badge podium-badge--first">
                        <el-icon :color="'var(--oj-bg-light-base)'" :size="24">
                            <i-ep-gold-medal />
                        </el-icon>
                    </div>
                    <el-card class="podium-card podium-card--first">
                        <el-avatar
                            class="podium-avatar podium-avatar--first"
                            fit="fill"
                            shape="square"
                            :size="80"
                            :src="topThree[0].Avatar"
                        />
                        <div class="podium-name podium-name--first">{{ topThree[0].NickName }}</div>
                        <div class="podium-profile">{{ getProfileText(topThree[0]) }}</div>
                        <div class="podium-stat podium-stat--first">{{ topThree[0].ACNum }} Solved</div>
                    </el-card>
                </div>

                <!-- 3rd -->
                <div v-if="topThree[2]" class="podium-item podium-item--third">
                    <div class="podium-badge podium-badge--third">3</div>
                    <el-card class="podium-card podium-card--third">
                        <el-avatar
                            class="podium-avatar podium-avatar--third"
                            fit="fill"
                            shape="square"
                            :size="72"
                            :src="topThree[2].Avatar"
                        />
                        <div class="podium-name">{{ topThree[2].NickName }}</div>
                        <div class="podium-profile">{{ getProfileText(topThree[2]) }}</div>
                        <div class="podium-stat podium-stat--third">{{ topThree[2].ACNum }} Solved</div>
                    </el-card>
                </div>
            </div>
        </section>

        <!-- Rank Table -->
        <el-card class="rank-table-card">
            <div class="rank-table-wrap">
                <el-skeleton :loading="initialLoading" animated>
                    <template #template>
                        <div class="skeleton-block" />
                        <div class="skeleton-block" />
                        <div class="skeleton-block" />
                    </template>

                    <template #default>
                        <el-empty v-if="!rankList.length && !loadError" description="暂无排名数据" />

                        <div v-else>
                            <el-table
                                :data="rankList"
                                class="rank-table"
                                :table-layout="tableLayout"
                                @row-click="handleRowClick"
                            >
                                <!-- 排名列 -->
                                <el-table-column prop="Rank" label="排名" width="120" align="center">
                                    <template #default="{ row }">
                                        <div class="rank-badge" :class="{ 'top-three': row.Rank <= 3 }">
                                            <el-icon v-if="row.Rank === 1" :size="28" :color="getRankColor(1)">
                                                <i-ep-trophy />
                                            </el-icon>
                                            <el-icon v-else-if="row.Rank === 2" :size="24" :color="getRankColor(2)">
                                                <i-ep-medal />
                                            </el-icon>
                                            <el-icon v-else-if="row.Rank === 3" :size="24" :color="getRankColor(3)">
                                                <i-ep-medal />
                                            </el-icon>
                                            <span v-else class="rank-number">{{ row.Rank }}</span>
                                        </div>
                                    </template>
                                </el-table-column>

                                <!-- 用户信息列 -->
                                <el-table-column label="用户" min-width="250">
                                    <template #default="{ row }">
                                        <div class="user-info">
                                            <div class="avatar-wrapper" :class="{ 'is-top': row.Rank <= 3 }">
                                                <el-avatar
                                                    :size="48"
                                                    :src="row.Avatar"
                                                    :icon="UserFilled"
                                                    class="user-avatar"
                                                />
                                                <div v-if="row.Rank === 1" class="crown-icon">👑</div>
                                            </div>
                                            <div class="info-content">
                                                <div class="nickname-row">
                                                    <span
                                                        class="nickname"
                                                        :style="{
                                                            color: row.Rank <= 3 ? getRankColor(row.Rank) : '',
                                                        }"
                                                    >
                                                        {{ row.NickName }}
                                                    </span>
                                                    <el-tag
                                                        v-if="row.Rank <= 3"
                                                        size="small"
                                                        effect="plain"
                                                        round
                                                        class="rank-tag"
                                                    >
                                                        TOP {{ row.Rank }}
                                                    </el-tag>
                                                </div>
                                                <div class="bio text-truncate" :title="row.PersonalProfile">
                                                    {{ getProfileText(row) }}
                                                </div>
                                            </div>
                                        </div>
                                    </template>
                                </el-table-column>

                                <el-table-column prop="ACNum" label="AC 数" width="120" align="center">
                                    <template #default="scope">
                                        <span class="metric metric--primary">{{ scope.row.ACNum }}</span>
                                    </template>
                                </el-table-column>

                                <el-table-column prop="SubmitNum" label="提交数" width="120" align="center">
                                    <template #default="scope">
                                        <span class="metric">{{ scope.row.SubmitNum }}</span>
                                    </template>
                                </el-table-column>

                                <el-table-column label="通过率" width="120" align="center">
                                    <template #default="scope">
                                        <span class="metric">{{ formatRate(scope.row) }}</span>
                                    </template>
                                </el-table-column>
                            </el-table>

                            <div class="rank-footer">
                                <el-button v-if="loadError" type="primary" plain @click="refresh">重试加载</el-button>
                                <span v-else-if="loadingMore" class="rank-footer-text">加载中…</span>
                                <span v-else-if="!canLoadMore" class="rank-footer-text"
                                    >已加载到第 {{ rankList.length }} 名</span
                                >
                                <span v-else class="rank-footer-text">继续下滑加载更多</span>
                            </div>
                        </div>

                        <!-- 触底哨兵：进入视口触发 loadMore -->
                        <div ref="sentinelRef" class="scroll-sentinel" />
                    </template>
                </el-skeleton>
            </div>
        </el-card>
        <UserDetailDrawer v-model="detailDrawerOpen" :userId="activeUserId" />
    </div>
</template>

<style scoped>
.user-rank-page {
    padding-top: var(--oj-spacing-8);
}

.podium-section {
    padding-top: var(--oj-spacing-12);
    margin-bottom: var(--oj-spacing-10);
}

.podium-grid {
    display: grid;
    grid-template-columns: 1fr;
    gap: var(--oj-spacing-6);
}

@media (width >=768px) {
    .podium-grid {
        grid-template-columns: repeat(3, 1fr);
        align-items: end;
    }

    .podium-item--first {
        order: 2;
    }

    .podium-item--second {
        order: 1;
    }

    .podium-item--third {
        order: 3;
    }
}

.podium-item {
    position: relative;
    transform: translate3d(0, 0, 0);
    transition: transform 260ms cubic-bezier(0.22, 1, 0.36, 1);
    will-change: transform;
}

.podium-card {
    position: relative;
    overflow: visible;
    text-align: center;
    background: var(--oj-card-bg);
    border: 1px solid var(--oj-card-border);
    border-radius: var(--oj-radius-xl);
    backdrop-filter: blur(var(--oj-glass-blur));
    transition:
        border-color 260ms ease,
        box-shadow 260ms ease;
    animation: podium-enter 520ms ease both;
    will-change: box-shadow, border-color;
}

.podium-card::before {
    position: absolute;
    inset: 0;
    pointer-events: none;
    content: "";
    background: var(--oj-gradient-card-hover);
    border-radius: inherit;
    opacity: 0.35;
}

.podium-card :deep(.el-card__body) {
    position: relative;
    z-index: 1;
    padding: var(--oj-spacing-6);
}

.podium-item--second .podium-card {
    animation-delay: 40ms;
}

.podium-item--first .podium-card {
    animation-delay: 80ms;
}

.podium-item--third .podium-card {
    animation-delay: 120ms;
}

.podium-card--first {
    background: linear-gradient(180deg, var(--oj-color-warning-soft) 0%, var(--oj-card-bg) 70%);
    border-color: var(--oj-color-warning-border);
    box-shadow: var(--oj-shadow-lg);
}

.podium-card--first::before {
    opacity: 0.5;
}

.podium-item:hover {
    transform: translate3d(0, calc(-1 * var(--oj-spacing-3)), 0);
}

.podium-item:hover .podium-card {
    border-color: var(--oj-surface-border-focus);
    box-shadow: var(--oj-shadow-neon);
}

.podium-item--first:hover .podium-card {
    box-shadow: var(--oj-shadow-neon-success);
}

.podium-badge {
    position: absolute;
    top: 0;
    left: 50%;
    z-index: 3;
    display: inline-flex;
    align-items: center;
    justify-content: center;
    width: var(--oj-spacing-10);
    height: var(--oj-spacing-10);
    font-weight: var(--oj-font-weight-bold);
    color: var(--oj-text-inverse);
    background: var(--oj-slate-700);
    border: 4px solid var(--oj-page-bg);
    border-radius: 50%;
    box-shadow: var(--oj-shadow-md);
    transform: translate(-50%, -50%);
    animation: podium-badge-pop 520ms ease both;
}

.podium-badge--first {
    width: var(--oj-spacing-12);
    height: var(--oj-spacing-12);
    font-size: var(--oj-font-size-lg);
    color: var(--oj-text-light-primary);
    background: var(--oj-color-warning-light);
    border-color: var(--oj-page-bg);
    box-shadow:
        0 0 0 1px var(--oj-color-warning-border),
        var(--oj-shadow-lg);
}

.podium-badge--second {
    color: var(--oj-text-inverse);
    background: var(--oj-slate-600);
}

.podium-badge--third {
    color: var(--oj-text-inverse);
    background: var(--oj-color-warning-dark);
}

.podium-avatar {
    display: inline-flex;
    margin-top: var(--oj-spacing-2);
    animation: podium-float 2800ms ease-in-out infinite;
}

.podium-avatar--first {
    animation-duration: 2400ms;
}

.podium-avatar--first :deep(img) {
    border: 3px solid var(--oj-color-warning-light);
}

.podium-avatar--second :deep(img) {
    border: 3px solid var(--oj-slate-600);
}

.podium-avatar--third :deep(img) {
    border: 3px solid var(--oj-color-warning-dark);
}

.podium-name {
    margin-top: var(--oj-spacing-4);
    font-size: var(--oj-font-size-lg);
    font-weight: var(--oj-font-weight-bold);
    color: var(--oj-text-color);
}

.podium-name--first {
    font-size: var(--oj-font-size-xl);
}

.podium-profile {
    margin-top: var(--oj-spacing-2);
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-color-muted);
}

.podium-stat {
    margin-top: var(--oj-spacing-3);
    font-weight: var(--oj-font-weight-bold);
    letter-spacing: var(--oj-letter-spacing-wide);
}

.podium-stat--first {
    color: var(--oj-color-warning-light);
}

.podium-stat--second {
    color: var(--oj-text-color-secondary);
}

.podium-stat--third {
    color: var(--oj-color-warning-dark);
}

@keyframes podium-enter {
    0% {
        opacity: 0;
        transform: translateY(var(--oj-spacing-4));
    }

    100% {
        opacity: 1;
        transform: translateY(0);
    }
}

@keyframes podium-badge-pop {
    0% {
        opacity: 0;
        transform: translate(-50%, -70%) scale(0.85);
    }

    70% {
        opacity: 1;
        transform: translate(-50%, -50%) scale(1.06);
    }

    100% {
        transform: translate(-50%, -50%) scale(1);
    }
}

@keyframes podium-float {
    0%,
    100% {
        transform: translateY(0);
    }

    50% {
        transform: translateY(calc(-1 * var(--oj-spacing-1)));
    }
}

@media (prefers-reduced-motion: reduce) {
    .podium-item,
    .podium-card,
    .podium-badge,
    .podium-avatar {
        animation: none !important;
    }

    .podium-item:hover {
        transform: none;
    }
}

.rank-table-card {
    background: var(--oj-card-bg);
    border: 1px solid var(--oj-card-border);
    border-radius: var(--oj-radius-xl);
    backdrop-filter: blur(var(--oj-glass-blur));
}

.rank-table-card :deep(.el-card__body) {
    padding: 0;
}

.rank-table-wrap {
    overflow: hidden;
}

.rank-table {
    width: 100%;
}

:deep(.rank-table.el-table) {
    background-color: transparent !important;
}

:deep(.rank-table .el-table__header-wrapper th) {
    color: var(--oj-text-color-secondary) !important;
    background-color: var(--oj-table-header-bg) !important;
    border-bottom: 1px solid var(--oj-table-border) !important;
}

:deep(.rank-table .el-table__body tr) {
    background-color: var(--oj-table-row-bg) !important;
}

:deep(.rank-table .el-table__body tr:hover > td) {
    background-color: var(--oj-table-row-hover-bg) !important;
}

:deep(.rank-table td.el-table__cell) {
    border-bottom: 1px solid var(--oj-table-border) !important;
}

/* 排名徽章 */
.rank-badge {
    display: flex;
    align-items: center;
    justify-content: center;
    font-size: var(--oj-font-size-base);
    font-weight: bold;
    color: var(--el-text-color-secondary);
}

/* 前三名特殊样式 */
.top-three {
    font-size: 0;
    background: transparent;
}

/* 排名数字圆圈 */
.rank-number {
    width: 32px;
    height: 32px;
    font-family: Monaco, monospace;
    line-height: var(--oj-line-height-loose);
    background: var(--el-fill-color-light);
    border-radius: 50%;
}

/* 用户信息 */
.user-info {
    display: flex;
    gap: var(--oj-gap-xl);
    align-items: center;
}

.avatar-wrapper {
    position: relative;
    transition: transform 0.3s ease;
}

.avatar-wrapper:hover {
    transform: scale(1.1) rotate(5deg);
}

.user-avatar {
    border: 2px solid var(--el-border-color-lighter);
    box-shadow: 0 2px 8px rgb(0 0 0 / 10%);
}

.is-top .user-avatar {
    border-color: var(--oj-color-warning-light);
    box-shadow: 0 0 15px var(--oj-warning-alpha-30);
}

.crown-icon {
    position: absolute;
    top: -5px;
    right: -6px;
    font-size: var(--oj-font-size-base);
    animation: float 2s ease-in-out infinite;
}

@keyframes float {
    0%,
    100% {
        transform: translateY(0);
    }

    50% {
        transform: translateY(-3px);
    }
}

.info-content {
    display: flex;
    flex: 1;
    flex-direction: column;
    gap: 4px;
    min-width: 0;
}

.nickname-row {
    display: flex;
    gap: var(--oj-gap-sm);
    align-items: center;
}

.nickname {
    font-size: 15px;
    font-weight: 600;
    color: var(--el-text-color-primary);
}

.rank-tag {
    height: 18px;
    padding: 0 6px;
    font-size: 10px;
}

.bio {
    overflow: hidden;
    text-overflow: ellipsis;
    font-size: 12px;
    color: var(--el-text-color-secondary);
    white-space: nowrap;
}

.metric {
    font-variant-numeric: tabular-nums;
    color: var(--oj-text-color-secondary);
}

.metric--primary {
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-color-primary);
}

.rank-footer {
    display: flex;
    align-items: center;
    justify-content: center;
    padding: var(--oj-spacing-4);
    background: var(--oj-table-header-bg);
    border-top: 1px solid var(--oj-table-border);
}

.rank-footer-text {
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-color-secondary);
}

.scroll-sentinel {
    width: 100%;
    height: 1px;
}

.skeleton-block {
    height: 18px;
    margin: var(--oj-spacing-4);
    background: var(--oj-surface-hover);
    border-radius: var(--oj-radius-md);
}
</style>
