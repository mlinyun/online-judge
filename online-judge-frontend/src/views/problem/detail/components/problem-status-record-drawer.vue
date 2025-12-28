<script setup lang="ts">
import { ref, watch } from "vue";

import { selectStatusRecordList } from "@/api/status";
import { DateUtils } from "@/utils/date/date-utils";
import type { Api } from "@/types/api/api";

import StatusRecordDetailDialog from "@/views/status-record/components/status-record-detail-dialog.vue";

type StatusRow = Api.Status.SelectStatusRecordListItem;

type StatusTagType = "success" | "warning" | "danger" | "info";

const props = defineProps<{ modelValue: boolean; problemId: Api.Problem.ProblemId | "" }>();
const emit = defineEmits<{ (e: "update:modelValue", v: boolean): void }>();

const loading = ref(false);
const error = ref("");

const page = ref(1);
const pageSize = ref(20);
const total = ref(0);
const list = ref<StatusRow[]>([]);

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

const fetchPage = async () => {
    const pid = String(props.problemId || "").trim();
    if (!pid) {
        error.value = "缺少题目 ID";
        list.value = [];
        total.value = 0;
        return;
    }

    loading.value = true;
    error.value = "";

    try {
        const res = await selectStatusRecordList({
            Page: page.value,
            PageSize: pageSize.value,
            SearchInfo: { ProblemId: pid as Api.Problem.ProblemId },
        });

        if (res.data.code === 0 && res.data.data) {
            list.value = (res.data.data.List || []) as StatusRow[];
            total.value = res.data.data.Total || 0;
            return;
        }

        list.value = [];
        total.value = 0;
        error.value = res.data.message || "加载失败";
    } catch (e) {
        console.error("获取提交记录失败:", e);
        list.value = [];
        total.value = 0;
        error.value = "网络异常，请稍后重试";
    } finally {
        loading.value = false;
    }
};

const handleClose = () => emit("update:modelValue", false);

const handlePageChange = (p: number) => {
    page.value = p;
    void fetchPage();
};

const handleRowClick = (row: StatusRow) => {
    activeStatusRecordId.value = row._id;
    detailDialogOpen.value = true;
};

watch(
    () => props.modelValue,
    (open) => {
        if (!open) return;
        page.value = 1;
        void fetchPage();
    }
);
</script>

<template>
    <el-drawer :model-value="modelValue" title="提交记录" direction="ltr" size="720px" :with-header="true"
        @close="handleClose">
        <div class="drawer-body">
            <el-skeleton v-if="loading" :rows="8" animated />

            <el-result v-else-if="error" icon="error" title="加载失败" :sub-title="error">
                <template #extra>
                    <el-button type="primary" @click="fetchPage">重试</el-button>
                </template>
            </el-result>

            <el-empty v-else-if="!list.length" description="暂无数据" />

            <div v-else class="table-wrap">
                <el-table class="record-table" :data="list" border stripe table-layout="fixed" height="100%"
                    @row-click="handleRowClick">
                    <el-table-column prop="_id" label="ID" width="210">
                        <template #default="scope">
                            <span class="mono">{{ scope.row._id }}</span>
                        </template>
                    </el-table-column>

                    <el-table-column prop="UserNickName" label="用户" min-width="120" />

                    <el-table-column prop="Language" label="语言" width="110" />

                    <el-table-column prop="Status" label="状态" width="160">
                        <template #default="scope">
                            <el-tag size="small" effect="light" :type="getStatusTagType(scope.row.Status)">
                                {{ getStatusTitle(scope.row.Status) }}
                            </el-tag>
                        </template>
                    </el-table-column>

                    <el-table-column prop="RunTime" label="时间" width="110" />

                    <el-table-column prop="RunMemory" label="内存" width="110" />

                    <el-table-column prop="SubmitTime" label="提交时间" min-width="180">
                        <template #default="scope">
                            {{ DateUtils.formatDateTime(scope.row.SubmitTime) }}
                        </template>
                    </el-table-column>
                </el-table>

                <div class="pager">
                    <el-pagination background layout="prev, pager, next" :page-size="pageSize" :total="total"
                        :current-page="page" @current-change="handlePageChange" />
                </div>
            </div>
        </div>

        <StatusRecordDetailDialog v-model="detailDialogOpen" :status-record-id="activeStatusRecordId" />
    </el-drawer>
</template>

<style scoped>
.drawer-body {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-4);
    height: 100%;
}

.table-wrap {
    display: flex;
    flex: 1;
    flex-direction: column;
    gap: var(--oj-spacing-3);
    min-height: 0;
}

.record-table {
    flex: 1;
    min-height: 0;
}

.mono {
    font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, "Liberation Mono", "Courier New", monospace;
}

.pager {
    display: flex;
    justify-content: center;
}
</style>
