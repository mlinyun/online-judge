<script setup lang="ts">
import { computed, onMounted, onUnmounted, ref, watch } from "vue";
import { useRoute, useRouter } from "vue-router";
import { ChatRound, View } from "@element-plus/icons-vue";

import { selectSolutionList } from "@/api/solution";
import { DateUtils } from "@/utils/date/date-utils";
import type { Api } from "@/types/api/api";

defineOptions({ name: "SolutionList" });

const PAGE_SIZE = 10;

type SolutionItem = Api.Solution.SelectSolutionListItem;
type BackendSolutionItem = Omit<SolutionItem, "User"> & {
    User?: Api.User.SimpleUserInfo | Api.User.SimpleUserInfo[] | null;
};
type UiSolutionItem = Omit<SolutionItem, "User"> & { User?: Api.User.SimpleUserInfo };

const route = useRoute();
const router = useRouter();

const list = ref<UiSolutionItem[]>([]);
const total = ref<number>(0);
const currentPage = ref<number>(1);

const initialLoading = ref(false);
const loadingMore = ref(false);
const loadError = ref<string>("");

const hasMore = ref(true);

const sentinelRef = ref<HTMLElement | null>(null);
let intersectionObserver: IntersectionObserver | null = null;

const parentId = computed<Api.Problem.ProblemId | undefined>(() => {
    const raw = route.query.ParentId ?? route.query.parentId;
    if (raw == null) return undefined;
    const value = String(raw).trim();
    if (!value || value === "0") return undefined;
    return value as Api.Problem.ProblemId;
});

const userId = computed<Api.User.UserId | undefined>(() => {
    const raw = route.query.UserId ?? route.query.userId;
    if (raw == null) return undefined;
    const value = String(raw).trim();
    if (!value || value === "0") return undefined;
    return value as Api.User.UserId;
});

const routeKey = computed(() => `${parentId.value ?? ""}|${userId.value ?? ""}`);

const buildSearchInfo = (): Api.Solution.SelectSolutionListParams["SearchInfo"] | undefined => {
    if (!parentId.value && !userId.value) return undefined;
    return {
        ...(parentId.value ? { ParentId: parentId.value } : {}),
        ...(userId.value ? { UserId: userId.value } : {}),
    };
};

const canLoadMore = computed(() => {
    if (initialLoading.value || loadingMore.value) return false;
    return hasMore.value;
});

const normalizeIncoming = (incoming: BackendSolutionItem[]): UiSolutionItem[] => {
    return incoming.map((item) => {
        const rawUser = item.User;
        const user = Array.isArray(rawUser) ? rawUser[0] : (rawUser ?? undefined);
        return {
            ...(item as Omit<BackendSolutionItem, "User">),
            User: user,
        };
    });
};

const appendUnique = (incoming: UiSolutionItem[]) => {
    if (!incoming.length) return;
    const seen = new Set(list.value.map((it) => String(it._id)));
    const next = [...list.value];
    for (const item of incoming) {
        const key = String(item._id);
        if (seen.has(key)) continue;
        seen.add(key);
        next.push(item);
    }
    list.value = next;
};

const fetchPage = async (page: number): Promise<UiSolutionItem[]> => {
    const SearchInfo = buildSearchInfo();
    const res = await selectSolutionList({
        Page: page,
        PageSize: PAGE_SIZE,
        ...(SearchInfo ? { SearchInfo } : {}),
    });

    if (res.data.success && res.data.data) {
        total.value = Number(res.data.data.Total ?? 0);
        return normalizeIncoming((res.data.data.List ?? []) as BackendSolutionItem[]);
    }

    throw new Error(res.data.message || "加载失败");
};

const refresh = async () => {
    initialLoading.value = true;
    loadingMore.value = false;
    loadError.value = "";
    currentPage.value = 1;
    list.value = [];
    total.value = 0;
    hasMore.value = true;

    try {
        const incoming = await fetchPage(1);
        appendUnique(incoming);

        if (total.value > 0) {
            hasMore.value = list.value.length < total.value;
        } else {
            hasMore.value = incoming.length === PAGE_SIZE;
        }
    } catch (e) {
        console.error("获取题解列表失败:", e);
        loadError.value = e instanceof Error ? e.message : "网络异常，请稍后重试";
        hasMore.value = false;
    } finally {
        initialLoading.value = false;
    }
};

const loadMore = async () => {
    if (!canLoadMore.value) return;
    const nextPage = currentPage.value + 1;

    loadingMore.value = true;
    loadError.value = "";
    try {
        const incoming = await fetchPage(nextPage);
        appendUnique(incoming);
        currentPage.value = nextPage;

        if (total.value > 0) {
            hasMore.value = list.value.length < total.value;
        } else {
            hasMore.value = incoming.length === PAGE_SIZE;
        }
    } catch (e) {
        console.error("加载更多题解失败:", e);
        loadError.value = "网络异常，加载更多失败";
        // 不打断已有内容，但阻止继续无限请求
        hasMore.value = false;
    } finally {
        loadingMore.value = false;
    }
};

const handleCardClick = (item: UiSolutionItem) => {
    void router.push({ name: "solution-detail", params: { id: item._id } });
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
            rootMargin: "120px",
            threshold: 0,
        }
    );

    if (sentinelRef.value) {
        intersectionObserver.observe(sentinelRef.value);
    }
});

watch(routeKey, () => {
    void refresh();
});

onUnmounted(() => {
    intersectionObserver?.disconnect();
    intersectionObserver = null;
});
</script>

<template>
    <section class="solution-list-page" aria-label="题解列表">
        <div class="header oj-glass-panel">
            <div class="header-left">
                <div class="title">题解</div>
                <div class="subtitle" v-if="parentId || userId">
                    <span v-if="parentId" class="mono">Problem: {{ parentId }}</span>
                    <span v-if="userId" class="mono">User: {{ userId }}</span>
                </div>
            </div>

            <div class="header-right">
                <div class="meta-chip" :class="{ 'is-active': !!(parentId || userId) }">
                    <span class="meta-k">Total</span>
                    <span class="meta-v">{{ total }}</span>
                </div>
            </div>
        </div>

        <div class="list-panel oj-glass-panel">
            <el-skeleton :loading="initialLoading" animated>
                <template #template>
                    <div class="skeleton-card" />
                    <div class="skeleton-card" />
                    <div class="skeleton-card" />
                </template>

                <template #default>
                    <el-empty v-if="!list.length && !loadError" description="暂无题解" />

                    <el-empty v-else-if="!list.length && loadError" :description="loadError">
                        <el-button type="primary" plain @click="refresh">重试</el-button>
                    </el-empty>

                    <div v-else class="solution-list">
                        <el-card v-for="item in list" :key="String(item._id)" class="solution-card" shadow="never">
                            <div
                                class="solution-card-inner"
                                role="button"
                                tabindex="0"
                                @click="handleCardClick(item)"
                                @keydown.enter.prevent="handleCardClick(item)"
                            >
                                <div class="card-row">
                                    <div class="card-left">
                                        <el-avatar class="avatar" :size="56" :src="item.User?.Avatar" />
                                    </div>

                                    <div class="card-middle">
                                        <div class="card-title" :title="item.Title">{{ item.Title }}</div>
                                        <div class="card-meta">
                                            <span class="meta-item">{{ item.User?.NickName || "-" }}</span>
                                            <span class="meta-dot">·</span>
                                            <span class="meta-item">{{
                                                DateUtils.formatDateTime(item.CreateTime)
                                            }}</span>
                                        </div>
                                    </div>

                                    <div class="card-right">
                                        <div class="stat">
                                            <el-icon class="stat-ic">
                                                <View />
                                            </el-icon>
                                            <span class="stat-v">{{ item.Views }}</span>
                                        </div>
                                        <div class="stat">
                                            <el-icon class="stat-ic">
                                                <ChatRound />
                                            </el-icon>
                                            <span class="stat-v">{{ item.Comments }}</span>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </el-card>

                        <div ref="sentinelRef" class="sentinel" aria-hidden="true" />

                        <div v-if="loadingMore" class="load-tip">加载中…</div>
                        <div v-else-if="loadError" class="load-tip load-tip--error">{{ loadError }}</div>
                        <div v-else-if="!hasMore" class="load-tip">没有更多了</div>
                    </div>
                </template>
            </el-skeleton>
        </div>
    </section>
</template>

<style scoped>
.solution-list-page {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-6);
}

.header {
    position: relative;
    display: flex;
    gap: var(--oj-spacing-4);
    align-items: center;
    justify-content: space-between;
    padding: var(--oj-spacing-4);
    margin-top: var(--oj-spacing-8);
    overflow: hidden;
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
}

.header::before {
    position: absolute;
    inset: 0;
    pointer-events: none;
    content: "";
    background:
        radial-gradient(520px circle at 12% 20%, rgb(var(--oj-color-primary-rgb) / 16%) 0%, transparent 55%),
        radial-gradient(420px circle at 88% -10%, rgb(var(--oj-color-primary-rgb) / 10%) 0%, transparent 55%),
        repeating-linear-gradient(90deg, rgb(var(--oj-color-primary-rgb) / 6%) 0 1px, transparent 1px 16px);
    opacity: 0.9;
}

.header-left,
.header-right {
    position: relative;
    z-index: 1;
    min-width: 0;
}

.title {
    font-size: var(--oj-font-size-lg);
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
}

.subtitle {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-3);
    margin-top: var(--oj-spacing-1);
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-secondary);
}

.mono {
    font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace;
}

.meta-chip {
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

.list-panel {
    overflow: hidden;
    border-radius: var(--oj-radius-xl);
}

.solution-card {
    background: var(--oj-glass-bg-light);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
}

.solution-card :deep(.el-card__body) {
    padding: 0;
}

.solution-card-inner {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-3);
    padding: var(--oj-spacing-4);
    cursor: pointer;
    border-radius: var(--oj-radius-xl);
    transition:
        background-color 0.16s ease,
        box-shadow 0.16s ease,
        transform 0.16s ease;
}

.solution-card-inner:hover {
    background-color: var(--oj-surface-hover);
    box-shadow: 0 18px 48px rgb(var(--oj-color-primary-rgb) / 10%);
    transform: translateY(-1px);
}

.solution-card-inner:focus-visible {
    outline: 2px solid rgb(var(--oj-color-primary-rgb));
    outline-offset: 2px;
}

.card-row {
    display: grid;
    grid-template-columns: 56px 1fr 84px;
    gap: var(--oj-spacing-4);
    align-items: center;
    min-width: 0;
}

.card-left {
    display: flex;
    align-items: center;
    justify-content: flex-start;
}

.avatar {
    flex: 0 0 auto;
    border: 1px solid rgb(var(--oj-color-primary-rgb) / 18%);
    box-shadow: 0 0 0 1px rgb(var(--oj-color-primary-rgb) / 10%) inset;
}

.card-middle {
    display: flex;
    flex: 1;
    flex-direction: column;
    gap: 6px;
    min-width: 0;
}

.card-right {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-2);
    align-items: flex-end;
    justify-content: center;
    color: var(--oj-text-color-secondary);
}

.card-title {
    overflow: hidden;
    text-overflow: ellipsis;
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
    white-space: nowrap;
    transition: color 0.16s ease;
}

.solution-card-inner:hover .card-title {
    color: var(--oj-color-primary);
}

.card-meta {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-2);
    align-items: center;
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-secondary);
}

.meta-dot {
    opacity: 0.65;
}

.stat {
    display: inline-flex;
    gap: var(--oj-spacing-2);
    align-items: center;
}

.stat-ic {
    opacity: 0.9;
}

.stat-v {
    font-variant-numeric: tabular-nums;
}

.sentinel {
    height: 1px;
}

.load-tip {
    padding: var(--oj-spacing-3) 0;
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-secondary);
    text-align: center;
}

.load-tip--error {
    color: var(--oj-color-danger);
}

.skeleton-card {
    height: 96px;
    margin: var(--oj-spacing-4);
    border-radius: var(--oj-radius-xl);
}

.solution-list {
    display: grid;
    gap: var(--oj-spacing-3);
    padding: var(--oj-spacing-4);
}

@media (width <=640px) {
    .solution-card-inner {
        padding: var(--oj-spacing-3);
    }

    .card-row {
        grid-template-columns: 48px 1fr;
        gap: var(--oj-spacing-3);
    }

    .card-right {
        flex-direction: row;
        grid-column: 1 / -1;
        gap: var(--oj-spacing-4);
        justify-content: flex-start;
        padding-left: 48px;
    }
}
</style>
