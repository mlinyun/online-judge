<script setup lang="ts">
import { computed, ref, watch } from "vue";
import { useRouter } from "vue-router";
import { Edit } from "@element-plus/icons-vue";

import { selectDiscussList } from "@/api/discussion";
import { selectSolutionList } from "@/api/solution";
import { DateUtils } from "@/utils/date/date-utils";
import type { Api } from "@/types/api/api";

type Kind = "solution" | "discussion";

type Status = {
    loading: boolean;
    error: string;
    page: number;
    pageSize: number;
    total: number;
    list: Array<Api.Solution.SelectSolutionListItem | Api.Discuss.SelectDiscussListItem>;
};

const props = defineProps<{ modelValue: boolean; kind: Kind; problemId: Api.Problem.ProblemId | "" }>();
const emit = defineEmits<{ (e: "update:modelValue", v: boolean): void }>();

const router = useRouter();

const title = computed(() => (props.kind === "solution" ? "题解" : "讨论"));
const writeText = computed(() => (props.kind === "solution" ? "写题解" : "写讨论"));

const state = ref<Status>({
    loading: false,
    error: "",
    page: 1,
    pageSize: 10,
    total: 0,
    list: [],
});

const normalizeUser = (u: unknown): Api.User.SimpleUserInfo | undefined => {
    if (!u) return undefined;
    if (Array.isArray(u)) return u[0] as Api.User.SimpleUserInfo | undefined;
    return u as Api.User.SimpleUserInfo;
};

const fetchPage = async () => {
    const pid = String(props.problemId || "").trim();
    if (!pid) {
        state.value = { ...state.value, list: [], total: 0, error: "缺少题目 ID" };
        return;
    }

    state.value.loading = true;
    state.value.error = "";

    try {
        if (props.kind === "solution") {
            const res = await selectSolutionList({
                Page: state.value.page,
                PageSize: state.value.pageSize,
                SearchInfo: { ParentId: pid as Api.Problem.ProblemId },
            });

            if (res.data.success && res.data.data) {
                state.value.list = (res.data.data.List || []) as Api.Solution.SelectSolutionListItem[];
                state.value.total = res.data.data.Total || 0;
                return;
            }

            state.value.list = [];
            state.value.total = 0;
            state.value.error = res.data.message || "加载失败";
            return;
        }

        const res = await selectDiscussList({
            Page: state.value.page,
            PageSize: state.value.pageSize,
            SearchInfo: { ParentId: pid as Api.Problem.ProblemId },
        });

        if (res.data.success && res.data.data) {
            state.value.list = (res.data.data.List || []) as Api.Discuss.SelectDiscussListItem[];
            state.value.total = res.data.data.Total || 0;
            return;
        }

        state.value.list = [];
        state.value.total = 0;
        state.value.error = res.data.message || "加载失败";
    } catch (e) {
        console.error("加载关联列表失败:", e);
        state.value.list = [];
        state.value.total = 0;
        state.value.error = "网络异常，请稍后重试";
    } finally {
        state.value.loading = false;
    }
};

const handleClose = () => emit("update:modelValue", false);

const handleWrite = () => {
    const pid = String(props.problemId || "").trim();
    if (!pid) return;

    if (props.kind === "solution") {
        void router.push({ name: "solution-write", query: { mode: "insert", parentId: pid } });
        handleClose();
        return;
    }

    void router.push({ name: "discussion-write", query: { mode: "insert", parentId: pid } });
    handleClose();
};

const handlePageChange = (p: number) => {
    state.value.page = p;
    void fetchPage();
};

watch(
    () => props.modelValue,
    (open) => {
        if (!open) return;
        state.value.page = 1;
        void fetchPage();
    }
);

watch(
    () => props.kind,
    () => {
        if (!props.modelValue) return;
        state.value.page = 1;
        void fetchPage();
    }
);

const handleItemClick = (item: Api.Solution.SelectSolutionListItem | Api.Discuss.SelectDiscussListItem) => {
    if (props.kind === "solution") {
        void router.push({ name: "solution-detail", params: { id: item._id } });
        handleClose();
        return;
    }

    void router.push({ name: "discussion-detail", params: { id: item._id } });
    handleClose();
};
</script>

<template>
    <el-drawer :model-value="modelValue" :title="title" direction="ltr" size="480px" :with-header="true"
        @close="handleClose">
        <div class="drawer-body">
            <div class="drawer-actions">
                <el-button type="primary" size="small" :icon="Edit" :disabled="!String(problemId || '').trim()"
                    @click="handleWrite">
                    {{ writeText }}
                </el-button>
            </div>

            <el-skeleton v-if="state.loading" :rows="6" animated />

            <el-result v-else-if="state.error" icon="error" title="加载失败" :sub-title="state.error">
                <template #extra>
                    <el-button type="primary" @click="fetchPage">重试</el-button>
                </template>
            </el-result>

            <el-empty v-else-if="!state.list.length" description="暂无数据" />

            <div v-else class="list">
                <el-card v-for="it in state.list" :key="String(it._id)" class="item-card" shadow="never"
                    @click="handleItemClick(it)">
                    <div class="item-row">
                        <div class="item-main">
                            <div class="item-title" :title="it.Title">{{ it.Title }}</div>
                            <div class="item-meta">
                                <span class="mono">#{{ it._id }}</span>
                                <span class="dot" />
                                <span>{{ DateUtils.formatDateTime(it.CreateTime) }}</span>
                            </div>
                        </div>
                        <div class="item-side">
                            <div class="stat">
                                <span class="k">浏览</span>
                                <span class="v">{{ it.Views }}</span>
                            </div>
                            <div class="stat">
                                <span class="k">评论</span>
                                <span class="v">{{ it.Comments }}</span>
                            </div>
                            <div class="user">{{ normalizeUser((it as any).User)?.NickName || "" }}</div>
                        </div>
                    </div>
                </el-card>

                <div class="pager">
                    <el-pagination background layout="prev, pager, next" :page-size="state.pageSize"
                        :total="state.total" :current-page="state.page" @current-change="handlePageChange" />
                </div>
            </div>
        </div>
    </el-drawer>
</template>

<style scoped>
.drawer-body {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-4);
}

.drawer-actions {
    display: flex;
    justify-content: flex-end;
}

.list {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-3);
}

.item-card {
    cursor: pointer;
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
}

.item-row {
    display: flex;
    gap: var(--oj-spacing-4);
    align-items: flex-start;
    justify-content: space-between;
}

.item-main {
    min-width: 0;
}

.item-title {
    overflow: hidden;
    text-overflow: ellipsis;
    font-size: var(--oj-font-size-base);
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
    white-space: nowrap;
}

.item-meta {
    display: inline-flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    margin-top: var(--oj-spacing-2);
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-secondary);
}

.mono {
    font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace;
}

.dot {
    width: 4px;
    height: 4px;
    background: rgb(var(--oj-color-primary-rgb) / 45%);
    border-radius: 999px;
}

.item-side {
    display: flex;
    flex: 0 0 auto;
    flex-direction: column;
    gap: var(--oj-spacing-2);
    align-items: flex-end;
}

.stat {
    display: inline-flex;
    gap: var(--oj-spacing-2);
    align-items: baseline;
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-secondary);
}

.stat .v {
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
}

.user {
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-secondary);
}

.pager {
    display: flex;
    justify-content: center;
    padding: var(--oj-spacing-2) 0;
}
</style>
