<script setup lang="ts">
import { computed, onMounted, ref, watch } from "vue";
import { useRouter } from "vue-router";

import { selectProblemList } from "@/api/problem";
import { getProblemTags } from "@/api/tag";
import { useUserStore } from "@/stores/modules/user";
import type { Api } from "@/types/api/api";

import ProblemSearchBar from "./components/problem-search-bar.vue";
import ProblemTable from "./components/problem-table.vue";
import ProblemSidebar from "./components/problem-sidebar.vue";

defineOptions({ name: "ProblemList" });

type ProblemRow = Api.Problem.SelectProblemListItem;

const router = useRouter();
const userStore = useUserStore();

// Search UI（按设计图：一个关键词输入 + 标签多选）
const keyword = ref<string>("");
const selectedTags = ref<string[]>([]);

// Pagination
const page = ref<number>(1);
const pageSize = ref<number>(20);
const total = ref<number>(0);

// Data
const rows = ref<ProblemRow[]>([]);
const allTags = ref<string[]>([]);
const hoveredRow = ref<ProblemRow | null>(null);

const loading = ref(false);
const loadError = ref("");

let debounceTimer: number | undefined;
const debounce = (fn: () => void, delay = 300) => {
    if (debounceTimer) window.clearTimeout(debounceTimer);
    debounceTimer = window.setTimeout(() => fn(), delay);
};

const hasSearch = computed(() => {
    return !!keyword.value.trim() || selectedTags.value.length > 0;
});

const solvedIds = computed(() => {
    const list = userStore.userInfo?.Solves ?? [];
    return new Set(list.map((x) => String(x)));
});

const solvedCount = computed(() => {
    const raw = userStore.userInfo?.Solves ?? [];
    return raw.length;
});

const normalizeKeywordToSearchInfo = (
    raw: string
): Pick<NonNullable<Api.Problem.SelectProblemListParams["SearchInfo"]>, "Id" | "Title"> => {
    const value = String(raw ?? "").trim();
    if (!value) return {};

    // 约定：纯数字优先作为 Id，否则作为 Title
    if (/^\d+$/.test(value)) {
        return { Id: value as Api.Problem.ProblemId };
    }
    return { Title: value };
};

const buildSearchInfo = (): Api.Problem.SelectProblemListParams["SearchInfo"] | undefined => {
    const kw = normalizeKeywordToSearchInfo(keyword.value);
    const Tags = selectedTags.value.length ? (selectedTags.value as Api.Tag.Tags) : undefined;

    if (!kw.Id && !kw.Title && !Tags?.length) return undefined;
    return {
        ...(kw.Id ? { Id: kw.Id } : {}),
        ...(kw.Title ? { Title: kw.Title } : {}),
        ...(Tags?.length ? { Tags } : {}),
    };
};

const requestSeq = ref(0);

const fetchTags = async () => {
    try {
        const res = await getProblemTags("Problem");
        if (res.data.code === 0 && res.data.data) {
            allTags.value = (res.data.data.Tags ?? []).slice().sort();
        }
    } catch (e) {
        console.warn("获取题目标签失败:", e);
        // 标签失败不阻断列表
        allTags.value = [];
    }
};

const fetchProblems = async () => {
    const seq = ++requestSeq.value;
    loading.value = true;
    loadError.value = "";

    try {
        const SearchInfo = buildSearchInfo();
        const res = await selectProblemList({
            Page: page.value,
            PageSize: pageSize.value,
            ...(SearchInfo ? { SearchInfo } : {}),
        });

        if (seq !== requestSeq.value) return;

        if (res.data.code === 0 && res.data.data) {
            rows.value = res.data.data.List ?? [];
            total.value = Number(res.data.data.Total ?? 0);
        } else {
            rows.value = [];
            total.value = 0;
            loadError.value = res.data.message || "加载失败";
        }
    } catch (e) {
        console.error("获取题目列表失败:", e);
        if (seq !== requestSeq.value) return;
        rows.value = [];
        total.value = 0;
        loadError.value = "网络异常，请稍后重试";
    } finally {
        if (seq === requestSeq.value) loading.value = false;
    }
};

const handleSearch = () => {
    if (page.value !== 1) {
        page.value = 1;
        return;
    }
    void fetchProblems();
};

const handleRowClick = (row: ProblemRow) => {
    void router.push({ name: "problem-detail", params: { id: row._id } });
};

const handleRowHover = (row: ProblemRow) => {
    hoveredRow.value = row;
};

const toggleTag = (tag: string) => {
    const set = new Set(selectedTags.value);
    if (set.has(tag)) set.delete(tag);
    else set.add(tag);
    selectedTags.value = Array.from(set);
};

watch([page, pageSize], () => {
    void fetchProblems();
});

watch(
    [keyword, selectedTags],
    () => {
        if (page.value !== 1) {
            page.value = 1;
            return;
        }
        debounce(() => {
            void fetchProblems();
        });
    },
    { deep: false }
);

watch(
    () => pageSize.value,
    () => {
        // 页容量变化回到第一页更符合预期
        if (page.value !== 1) page.value = 1;
    }
);

onMounted(async () => {
    await Promise.all([fetchTags(), fetchProblems()]);
});
</script>

<template>
    <section class="problem-list-page" aria-label="题库">
        <div class="layout">
            <div class="main">
                <ProblemSearchBar v-model:keyword="keyword" v-model:selected-tags="selectedTags" :all-tags="allTags"
                    :total="total" :has-search="hasSearch" @search="handleSearch" />

                <ProblemTable :rows="rows" :loading="loading" :load-error="loadError" :has-search="hasSearch"
                    :solved-ids="solvedIds" @row-click="handleRowClick" @row-hover="handleRowHover"
                    @retry="fetchProblems" />

                <div class="pagination oj-glass-panel">
                    <el-pagination v-model:current-page="page" v-model:page-size="pageSize"
                        :page-sizes="[10, 20, 30, 40]" layout="total, sizes, prev, pager, next, jumper" :total="total"
                        background />
                </div>
            </div>

            <div class="side">
                <ProblemSidebar :total="total" :solved-count="solvedCount" :all-tags="allTags"
                    :selected-tags="selectedTags" :hovered="hoveredRow" @toggle-tag="toggleTag" />
            </div>
        </div>
    </section>
</template>

<style scoped>
.problem-list-page {
    padding: var(--oj-spacing-6);
}

.layout {
    display: grid;
    grid-template-columns: minmax(0, 1fr) 320px;
    gap: var(--oj-spacing-6);
    align-items: start;
}

.main {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-5);
    min-width: 0;
}

.side {
    min-width: 0;
}

.pagination {
    padding: var(--oj-spacing-4);
    border-radius: var(--oj-radius-2xl);
}

.pagination :deep(.el-pagination) {
    flex-wrap: wrap;
    row-gap: var(--oj-spacing-2);
}

@media (width <=1024px) {
    .layout {
        grid-template-columns: minmax(0, 1fr);
    }
}

@media (width <=768px) {
    .problem-list-page {
        padding: var(--oj-spacing-4);
    }

    .layout {
        gap: var(--oj-spacing-4);
    }

    .main {
        gap: var(--oj-spacing-4);
    }
}
</style>
