<script setup lang="ts">
import { computed, onMounted, ref, watch } from "vue";
import { useRouter } from "vue-router";
import { Document, RefreshRight } from "@element-plus/icons-vue";

import { selectStatusRecordList } from "@/api/status";
import { DateUtils } from "@/utils/date/date-utils";
import { useUserStore } from "@/stores/modules/user";
import type { Api } from "@/types/api/api";

import StatusRecordDetailDialog from "@/views/status-record/components/status-record-detail-dialog.vue";

defineOptions({ name: "UserSubmissions" });

type StatusRow = Api.Status.SelectStatusRecordListItem;

type StatusTagType = "success" | "warning" | "danger" | "info";

const router = useRouter();
const userStore = useUserStore();

const isLoggedIn = computed(() => userStore.getIsLoggedIn);
const userId = computed<Api.User.UserId | undefined>(() => {
    const id = String(userStore.getUserInfo?._id ?? "").trim();
    if (!id || id === "0") return undefined;
    return id as Api.User.UserId;
});

const loading = ref(false);
const error = ref("");

const page = ref(1);
const pageSize = ref(20);
const total = ref(0);
const list = ref<StatusRow[]>([]);

const requestSeq = ref(0);

const detailDialogOpen = ref(false);
const activeStatusRecordId = ref<Api.Status.StatusRecordId | undefined>(undefined);

const getStatusTitle = (status: number): string => {
    if (status === 0) return "Pending";
    if (status === 1) return "Compile Error";
    if (status === 2) return "Accepted";
    if (status === 3) return "Wrong Answer";
    if (status === 4) return "Runtime Error";
    if (status === 5) return "Time Limit Exceeded";
    if (status === 6) return "Memory Limit Exceeded";
    if (status === 7) return "System Error";
    return `Unknown (${status})`;
};

const getStatusTagType = (status: number): StatusTagType => {
    if (status === 0) return "info";
    if (status === 2) return "success";
    if (status === 3) return "danger";
    return "warning";
};

const fetchRecords = async () => {
    if (!userId.value) {
        list.value = [];
        total.value = 0;
        error.value = isLoggedIn.value ? "缺少用户信息" : "请先登录";
        return;
    }

    const seq = ++requestSeq.value;
    loading.value = true;
    error.value = "";

    try {
        const res = await selectStatusRecordList({
            Page: page.value,
            PageSize: pageSize.value,
            SearchInfo: { UserId: userId.value },
        });

        if (seq !== requestSeq.value) return;

        if (res.data.code === 0 && res.data.data) {
            list.value = (res.data.data.List || []) as StatusRow[];
            total.value = res.data.data.Total || 0;
            return;
        }

        list.value = [];
        total.value = 0;
        error.value = res.data.message || "加载失败";
    } catch (e) {
        console.error("获取我的提交失败:", e);
        if (seq !== requestSeq.value) return;
        list.value = [];
        total.value = 0;
        error.value = "网络异常，请稍后重试";
    } finally {
        if (seq === requestSeq.value) loading.value = false;
    }
};

const handleRowClick = (row: StatusRow) => {
    activeStatusRecordId.value = row._id;
    detailDialogOpen.value = true;
};

const handleGoProblem = (problemId: Api.Problem.ProblemId) => {
    void router.push({ name: "problem-detail", params: { id: problemId } });
};

const handleRefresh = () => {
    void fetchRecords();
};

watch([page, pageSize], () => {
    void fetchRecords();
});

watch(
    () => userId.value,
    () => {
        page.value = 1;
        void fetchRecords();
    }
);

onMounted(() => {
    void fetchRecords();
});
</script>

<template>
    <section class="my-submissions-page" aria-label="我的提交记录">
        <header class="hero oj-glass-panel">
            <div class="hero-left">
                <div class="hero-title">我的提交</div>
                <div class="hero-sub">追踪你的每一次提交与判题结果</div>

                <div class="meta">
                    <div class="chip">
                        <span class="k">User</span>
                        <span class="v mono">{{ userId || "-" }}</span>
                    </div>
                    <div class="chip">
                        <span class="k">Total</span>
                        <span class="v">{{ total }}</span>
                    </div>
                </div>
            </div>

            <div class="hero-right">
                <el-button :icon="RefreshRight" @click="handleRefresh">刷新</el-button>
            </div>
        </header>

        <div class="panel oj-glass-panel">
            <div v-if="error && !loading" class="state">
                <el-result icon="warning" title="无法加载" :sub-title="error">
                    <template #extra>
                        <el-button v-if="!isLoggedIn" type="primary" @click="$router.push({ name: 'auth-login' })">
                            去登录
                        </el-button>
                        <el-button v-else type="primary" @click="handleRefresh">重试</el-button>
                    </template>
                </el-result>
            </div>

            <el-skeleton v-else-if="loading" animated>
                <template #template>
                    <div class="skeleton" />
                </template>
            </el-skeleton>

            <div v-else class="table-wrap">
                <el-table class="record-table" :data="list" border stripe table-layout="fixed" height="100%"
                    @row-click="handleRowClick">
                    <el-table-column prop="_id" label="记录ID" width="220">
                        <template #default="scope">
                            <span class="mono">{{ scope.row._id }}</span>
                        </template>
                    </el-table-column>

                    <el-table-column prop="ProblemTitle" label="题目" min-width="220">
                        <template #default="scope">
                            <div class="problem-cell">
                                <el-icon class="problem-ic">
                                    <Document />
                                </el-icon>
                                <button class="problem-btn" type="button" :title="scope.row.ProblemTitle"
                                    @click.stop="handleGoProblem(scope.row.ProblemId)">
                                    {{ scope.row.ProblemTitle }}
                                </button>
                            </div>
                        </template>
                    </el-table-column>

                    <el-table-column prop="Language" label="语言" width="110" />

                    <el-table-column prop="Status" label="状态" width="170">
                        <template #default="scope">
                            <el-tag size="small" effect="light" :type="getStatusTagType(scope.row.Status)">
                                {{ getStatusTitle(scope.row.Status) }}
                            </el-tag>
                        </template>
                    </el-table-column>

                    <el-table-column prop="RunTime" label="时间" width="110" />
                    <el-table-column prop="RunMemory" label="内存" width="110" />

                    <el-table-column prop="SubmitTime" label="提交时间" min-width="200">
                        <template #default="scope">
                            <span class="time">{{ DateUtils.formatDateTime(scope.row.SubmitTime) }}</span>
                        </template>
                    </el-table-column>
                </el-table>

                <div class="pager">
                    <el-pagination background layout="prev, pager, next" :page-size="pageSize" :total="total"
                        :current-page="page" @current-change="(p: number) => (page = p)" />
                </div>
            </div>
        </div>

        <StatusRecordDetailDialog v-model="detailDialogOpen" :status-record-id="activeStatusRecordId" />
    </section>
</template>

<style scoped>
.my-submissions-page {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-6);
    min-height: 0;
    padding: var(--oj-spacing-4);
}

.hero {
    display: flex;
    gap: var(--oj-spacing-6);
    align-items: flex-start;
    justify-content: space-between;
    padding: var(--oj-spacing-5);
    border-radius: var(--oj-radius-2xl);
}

.hero-left {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-3);
    min-width: 0;
}

.hero-title {
    font-size: var(--oj-font-size-2xl);
    font-weight: var(--oj-font-weight-bold);
    color: var(--oj-text-color);
}

.hero-sub {
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-color-secondary);
}

.meta {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-3);
}

.chip {
    display: inline-flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    padding: var(--oj-spacing-2) var(--oj-spacing-3);
    background: rgb(var(--oj-color-primary-rgb) / 6%);
    border: 1px solid rgb(var(--oj-color-primary-rgb) / 12%);
    border-radius: var(--oj-radius-xl);
}

.k {
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-muted);
    text-transform: uppercase;
    letter-spacing: var(--oj-letter-spacing-wide);
}

.v {
    font-size: var(--oj-font-size-sm);
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
}

.mono {
    font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace;
}

.panel {
    min-height: 0;
    padding: var(--oj-spacing-5);
    border-radius: var(--oj-radius-2xl);
}

.state {
    padding: var(--oj-spacing-6) 0;
}

.table-wrap {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-4);
    height: 62vh;
    min-height: 0;
}

.record-table {
    flex: 1;
    min-height: 0;
}

.problem-cell {
    display: inline-flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    min-width: 0;
}

.problem-ic {
    color: var(--oj-text-color-muted);
}

.problem-btn {
    min-width: 0;
    overflow: hidden;
    text-overflow: ellipsis;
    color: var(--oj-text-color);
    text-align: left;
    white-space: nowrap;
    cursor: pointer;
    background: transparent;
    border: none;
}

.problem-btn:hover {
    color: var(--oj-color-primary);
}

.time {
    color: var(--oj-text-color-secondary);
}

.pager {
    display: flex;
    justify-content: center;
}

.skeleton {
    height: 62vh;
    border-radius: var(--oj-radius-2xl);
}

@media (width <=640px) {
    .hero {
        flex-direction: column;
        align-items: stretch;
    }

    .table-wrap {
        height: 70vh;
    }
}
</style>
