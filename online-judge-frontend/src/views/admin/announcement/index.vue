<script setup lang="ts">
/**
 * 管理后台 - 公告管理页面
 */
import { computed, onMounted, ref, watch } from "vue";
import { useRouter } from "vue-router";
import { Delete, Edit, More, Plus, Search, View } from "@element-plus/icons-vue";
import { deleteAnnouncement, selectAnnouncementListByAdmin, updateAnnouncementActive } from "@/api/announcement";
import { DateUtils } from "@/utils/date/date-utils";
import type { Api } from "@/types/api/api";

defineOptions({ name: "AdminAnnouncement" });

const router = useRouter();

const loading = ref(false);
const loadError = ref("");

const page = ref(1);
const pageSize = ref(20);
const total = ref(0);

type AnnouncementRow = Api.Announcement.SelectAnnouncementListByAdminItem;
const announcements = ref<AnnouncementRow[]>([]);

let debounceTimer: number | undefined;
const debounce = (fn: () => void, delay = 300) => {
    if (debounceTimer) window.clearTimeout(debounceTimer);
    debounceTimer = window.setTimeout(() => fn(), delay);
};

// 工具栏：SearchInfo 支持字段
const searchTitle = ref("");
const searchLevel = ref<number | undefined>(undefined);
const searchActive = ref<"all" | "active" | "inactive">("all");

const hasSearch = computed(() => {
    return !!searchTitle.value.trim() || typeof searchLevel.value === "number" || searchActive.value !== "all";
});

const buildSearchInfo = (): Api.Announcement.SelectAnnouncementListByAdminParams["SearchInfo"] | undefined => {
    const Title = searchTitle.value.trim();
    const Level = searchLevel.value;

    const Active = searchActive.value === "all" ? undefined : searchActive.value === "active";

    if (!Title && typeof Level !== "number" && typeof Active !== "boolean") return undefined;

    return {
        ...(Title ? { Title } : {}),
        ...(typeof Level === "number" ? { Level } : {}),
        ...(typeof Active === "boolean" ? { Active } : {}),
    };
};

const requestSeq = ref(0);

/**
 * 获取公告列表
 */
const fetchAnnouncements = async () => {
    const seq = ++requestSeq.value;
    loading.value = true;
    loadError.value = "";

    try {
        const SearchInfo = buildSearchInfo();
        const response = await selectAnnouncementListByAdmin({
            Page: page.value,
            PageSize: pageSize.value,
            ...(SearchInfo ? { SearchInfo } : {}),
        });

        if (seq !== requestSeq.value) return;

        if (response.data.code === 0 && response.data.data) {
            announcements.value = (response.data.data.List || []) as AnnouncementRow[];
            total.value = response.data.data.Total || 0;
        } else {
            announcements.value = [];
            total.value = 0;
            loadError.value = response.data.message || "加载失败";
        }
    } catch (err) {
        console.error("获取公告列表失败:", err);
        announcements.value = [];
        total.value = 0;
        loadError.value = "网络异常，请稍后重试";
    } finally {
        if (seq === requestSeq.value) loading.value = false;
    }
};

const handlePageChange = (newPage: number) => {
    page.value = newPage;
};

const handleSizeChange = (newSize: number) => {
    pageSize.value = newSize;
    page.value = 1;
};

/**
 * 处理发布公告
 */
const handlePublish = () => {
    // TODO: 后期实现公告发布功能
    ElMessage({
        type: "info",
        message: "公告编辑/发布页暂未实现",
        showClose: true,
    });
};

/**
 * 处理查看公告
 * @param row 公告行数据
 */
const handleView = (row: AnnouncementRow) => {
    // TODO: 后期实现公告查看功能
    router.push({ name: "announcement-detail", params: { id: row._id } });
};

/**
 * 处理编辑公告
 * @param row 公告行数据
 */
const handleEdit = (row: AnnouncementRow) => {
    // TODO: 后期实现公告编辑功能
    ElMessage({
        type: "info",
        message: `暂不支持编辑公告（ID: ${row._id}）`,
        showClose: true,
    });
};

/**
 * 处理删除公告
 * @param row 公告行数据
 */
const handleDelete = async (row: AnnouncementRow) => {
    try {
        await ElMessageBox.confirm(`确认删除公告「${row.Title}」？此操作不可恢复。`, "删除确认", {
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
        const response = await deleteAnnouncement(row._id);
        if (response.data.code === 0) {
            const isLastItemOnPage = announcements.value.length <= 1 && page.value > 1;
            if (isLastItemOnPage) page.value -= 1;
            await fetchAnnouncements();
        }
    } catch (err) {
        console.error("删除公告失败:", err);
    } finally {
        loading.value = false;
    }
};

/**
 * 处理表格行操作命令
 * @param command 命令
 * @param row 行数据
 */
const handleRowCommand = async (command: "view" | "edit" | "delete", row: AnnouncementRow) => {
    if (command === "view") return handleView(row);
    if (command === "edit") return handleEdit(row);
    if (command === "delete") return handleDelete(row);
};

/**
 * 处理切换公告可见性
 * @param row 公告行数据
 * @param nextActive 下一个可见性状态
 */
const handleToggleActive = async (row: AnnouncementRow, nextActive: boolean) => {
    const prev = row.Active;
    row.Active = nextActive;
    try {
        const response = await updateAnnouncementActive({
            AnnouncementId: row._id,
            Active: nextActive,
        });

        if (response.data.code !== 0) {
            row.Active = prev;
        }
    } catch (err) {
        console.error("更新公告可见性失败:", err);
        row.Active = prev;
    }
};

watch([page, pageSize], () => {
    fetchAnnouncements();
});

watch(
    [searchTitle, searchLevel, searchActive],
    () => {
        if (page.value !== 1) {
            page.value = 1;
            return;
        }
        debounce(() => {
            fetchAnnouncements();
        });
    },
    { deep: false }
);

onMounted(() => {
    fetchAnnouncements();
});
</script>

<template>
    <section class="admin-announcement" aria-label="公告管理">
        <!-- Toolbar -->
        <div class="toolbar oj-glass-panel">
            <div class="toolbar-left">
                <el-input
                    v-model="searchTitle"
                    class="toolbar-field"
                    :prefix-icon="Search"
                    clearable
                    placeholder="搜索公告标题..."
                />

                <el-select v-model="searchLevel" class="toolbar-field" clearable placeholder="选择公告级别">
                    <el-option :value="1" label="1" />
                    <el-option :value="2" label="2" />
                    <el-option :value="3" label="3" />
                    <el-option :value="4" label="4" />
                    <el-option :value="5" label="5" />
                </el-select>

                <el-select v-model="searchActive" class="toolbar-field" placeholder="选择激活状态">
                    <el-option value="all" label="全部" />
                    <el-option value="active" label="已激活" />
                    <el-option value="inactive" label="未激活" />
                </el-select>
            </div>

            <div class="toolbar-right">
                <el-button class="publish-btn" type="primary" :icon="Plus" @click="handlePublish">发布公告</el-button>
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
                        v-if="!announcements.length && !loadError"
                        :description="hasSearch ? '暂无匹配的公告' : '暂无公告数据'"
                    />

                    <el-empty v-else-if="!announcements.length && loadError" :description="loadError">
                        <el-button type="primary" plain @click="fetchAnnouncements">重试加载</el-button>
                    </el-empty>

                    <div v-else>
                        <el-table :data="announcements" class="announcement-table" table-layout="fixed">
                            <el-table-column label="ID" width="180">
                                <template #default="scope">
                                    <span class="id-mono" :title="scope.row._id">{{ String(scope.row._id) }}</span>
                                </template>
                            </el-table-column>

                            <el-table-column label="标题" min-width="260">
                                <template #default="scope">
                                    <span class="title-ellipsis" :title="scope.row.Title">{{ scope.row.Title }}</span>
                                </template>
                            </el-table-column>

                            <el-table-column prop="Views" label="浏览量" width="110" align="center" />
                            <el-table-column prop="Comments" label="评论数" width="110" align="center" />
                            <el-table-column prop="Level" label="公告级别" width="110" align="center" />

                            <el-table-column label="发布时间" width="170">
                                <template #default="scope">
                                    <span class="time-text">{{ DateUtils.formatDateTime(scope.row.CreateTime) }}</span>
                                </template>
                            </el-table-column>

                            <el-table-column label="可见性" width="120" align="center">
                                <template #default="scope">
                                    <template v-if="typeof scope.row.Active === 'boolean'">
                                        <el-switch
                                            class="active-switch"
                                            :model-value="scope.row.Active"
                                            @change="
                                                (val: string | number | boolean) =>
                                                    handleToggleActive(scope.row, Boolean(val))
                                            "
                                        />
                                    </template>
                                    <span v-else class="muted-text">-</span>
                                </template>
                            </el-table-column>

                            <el-table-column label="操作" width="86" fixed="right" align="right">
                                <template #default="scope">
                                    <el-dropdown
                                        trigger="click"
                                        @command="(cmd) => handleRowCommand(cmd as any, scope.row)"
                                    >
                                        <el-button link class="opt-btn" :icon="More">操作</el-button>
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
    </section>
</template>

<style scoped>
.admin-announcement {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-6);
}

/* Toolbar */
.toolbar {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-4);
    align-items: center;
    justify-content: space-between;
    padding: var(--oj-spacing-4);
    border-radius: var(--oj-radius-xl);
}

.toolbar-left {
    display: flex;
    flex: 1;
    flex-wrap: wrap;
    gap: var(--oj-spacing-3);
    align-items: center;
    min-width: 0;
}

.toolbar-field {
    width: min(320px, 100%);
}

.publish-btn {
    /* 强制使用项目主题主色（cyber-accent），避免被 Element Plus 默认 primary 覆盖 */
    --el-color-primary: var(--oj-button-primary-bg);
    --el-button-bg-color: var(--oj-button-primary-bg);
    --el-button-border-color: var(--oj-button-primary-bg);
    --el-button-hover-bg-color: var(--oj-button-primary-hover-bg);
    --el-button-hover-border-color: var(--oj-button-primary-hover-bg);
    --el-button-active-bg-color: var(--oj-button-primary-hover-bg);
    --el-button-active-border-color: var(--oj-button-primary-hover-bg);
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
.announcement-table :deep(.el-table__inner-wrapper::before) {
    background-color: transparent;
}

.announcement-table :deep(.el-table__header-wrapper th.el-table__cell) {
    color: var(--oj-text-color-muted);
    background: var(--oj-table-header-bg);
    border-bottom: 1px solid var(--oj-glass-border);
}

.announcement-table :deep(.el-table__body tr:hover > td.el-table__cell) {
    background: var(--oj-table-row-hover-bg);
}

.announcement-table :deep(.el-table__body td.el-table__cell) {
    border-bottom: 1px solid var(--oj-table-border);
}

.id-mono {
    display: inline-block;
    max-width: 100%;
    overflow: hidden;
    text-overflow: ellipsis;
    font-family: var(--oj-font-family-mono), monospace;
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color);
    white-space: nowrap;
}

.title-ellipsis {
    display: inline-block;
    max-width: 100%;
    overflow: hidden;
    text-overflow: ellipsis;
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
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

.active-switch {
    --el-switch-on-color: var(--oj-color-primary);
    --el-switch-off-color: var(--oj-color-danger-border);
}

.opt-btn {
    color: var(--oj-link-hover-color);
}

.pagination-bar {
    display: flex;
    justify-content: flex-end;
    padding: var(--oj-spacing-4);
    background: var(--oj-table-header-bg);
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
