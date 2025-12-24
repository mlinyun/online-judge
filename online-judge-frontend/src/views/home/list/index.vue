<script setup lang="ts">
import { computed, onMounted, ref, watch } from "vue";
import { useRouter } from "vue-router";
import { Bell, Calendar, ChatDotSquare, ChatRound, View } from "@element-plus/icons-vue";
import { selectAnnouncementList } from "@/api/announcement";
import { DateUtils } from "@/utils/date/date-utils.ts";
import type { Api } from "@/types/api/api";

defineOptions({ name: "AnnouncementList" });

const router = useRouter();

const announcements = ref<Api.Announcement.SelectAnnouncementListItem[]>([]);
const loading = ref(false);
const error = ref(false);

const page = ref(1);
const pageSize = ref(10);
const total = ref(0);

const hasData = computed(() => announcements.value.length > 0);

const navigateToAnnouncement = (announcementId: Api.Announcement.AnnouncementId) => {
    router.push({ name: "announcement-detail", params: { id: announcementId } });
};

const fetchAnnouncements = async () => {
    loading.value = true;
    error.value = false;

    try {
        const response = await selectAnnouncementList({
            Page: page.value,
            PageSize: pageSize.value,
        });

        if (response.data.code === 0 && response.data.data) {
            announcements.value = response.data.data.List || [];
            total.value = response.data.data.Total || 0;
        } else {
            error.value = true;
        }
    } catch (err) {
        console.error("获取公告列表失败:", err);
        error.value = true;
    } finally {
        loading.value = false;
    }
};

const handleSizeChange = (size: number) => {
    pageSize.value = size;
    page.value = 1;
};

watch([page, pageSize], () => {
    fetchAnnouncements();
});

onMounted(() => {
    fetchAnnouncements();
});
</script>

<template>
    <div class="announcement-page">
        <div class="announcement-panel">
            <div class="announcement-header">
                <h1 class="announcement-title">
                    <el-icon class="announcement-title-icon">
                        <Bell />
                    </el-icon>
                    全部公告
                </h1>
            </div>

            <div v-if="loading" class="announcement-loading">
                <el-skeleton :rows="8" animated />
            </div>

            <div v-else-if="error" class="announcement-error">
                <el-result icon="error" title="加载失败" sub-title="请稍后重试">
                    <template #extra>
                        <el-button type="primary" @click="fetchAnnouncements">重试</el-button>
                    </template>
                </el-result>
            </div>

            <div v-else-if="!hasData" class="announcement-empty">
                <el-empty description="暂无公告" />
            </div>

            <div v-else class="announcement-list">
                <el-card
                    v-for="announcement in announcements"
                    :key="announcement._id"
                    class="announcement-card"
                    shadow="never"
                >
                    <div
                        class="announcement-card-inner"
                        role="button"
                        tabindex="0"
                        @click="navigateToAnnouncement(announcement._id)"
                        @keydown.enter="navigateToAnnouncement(announcement._id)"
                    >
                        <el-row :gutter="12" align="middle">
                            <el-col :span="3" class="announcement-leading">
                                <el-icon size="36" class="announcement-leading-icon">
                                    <ChatDotSquare />
                                </el-icon>
                            </el-col>
                            <el-col :span="15" class="announcement-main">
                                <el-row :gutter="12" class="announcement-top" align="middle">
                                    <el-col :span="18" class="announcement-title-col">
                                        <div class="announcement-item-title" :title="announcement.Title">
                                            {{ announcement.Title }}
                                        </div>
                                    </el-col>
                                </el-row>
                                <el-row class="announcement-bottom" align="middle">
                                    <el-col :span="24">
                                        <div class="announcement-date">
                                            <el-icon class="date-icon">
                                                <Calendar />
                                            </el-icon>
                                            <span>{{ DateUtils.formatDateTime(announcement.CreateTime) }}</span>
                                        </div>
                                    </el-col>
                                </el-row>
                            </el-col>
                            <el-col :span="6" class="announcement-stats-col">
                                <div class="announcement-stats">
                                    <span class="stat-item">
                                        <el-icon class="stat-icon">
                                            <View />
                                        </el-icon>
                                        <span>{{ announcement.Views }}</span>
                                    </span>
                                    <span class="stat-item">
                                        <el-icon class="stat-icon">
                                            <ChatRound />
                                        </el-icon>
                                        <span>{{ announcement.Comments }}</span>
                                    </span>
                                </div>
                            </el-col>
                        </el-row>
                    </div>
                </el-card>
            </div>

            <div class="announcement-pagination">
                <el-pagination
                    v-model:current-page="page"
                    v-model:page-size="pageSize"
                    :page-sizes="[10, 20, 50]"
                    layout="total, sizes, prev, pager, next, jumper"
                    :total="total"
                    background
                    @size-change="handleSizeChange"
                />
            </div>
        </div>
    </div>
</template>

<style scoped>
.announcement-page {
    padding: var(--oj-spacing-6);
}

.announcement-panel {
    overflow: hidden;
    background: var(--oj-glass-bg);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-2xl);
    box-shadow: var(--oj-shadow-xl);
    backdrop-filter: blur(var(--oj-glass-blur));
}

.announcement-header {
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: var(--oj-spacing-6) var(--oj-spacing-6) var(--oj-spacing-4);
    background: var(--oj-glass-bg-light);
    border-bottom: 1px solid var(--oj-glass-border);
}

.announcement-title {
    display: flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    margin: 0;
    font-size: var(--oj-font-size-xl);
    font-weight: var(--oj-font-weight-bold);
    color: var(--oj-text-color);
}

.announcement-title-icon {
    color: var(--oj-color-primary);
}

.announcement-loading,
.announcement-error,
.announcement-empty {
    padding: var(--oj-spacing-8) var(--oj-spacing-6);
}

.announcement-list {
    display: grid;
    gap: var(--oj-spacing-4);
    padding: var(--oj-spacing-5);
}

.announcement-card {
    background: var(--oj-glass-bg-light);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
}

.announcement-card :deep(.el-card__body) {
    padding: 0;
}

.announcement-card-inner {
    padding: var(--oj-spacing-5);
    cursor: pointer;
    border-radius: var(--oj-radius-xl);
    transition: background-color 0.2s ease;
}

.announcement-card-inner:hover {
    background-color: var(--oj-surface-hover);
}

.announcement-card-inner:focus-visible {
    outline: 2px solid var(--oj-color-primary);
    outline-offset: 2px;
}

.announcement-leading {
    display: flex;
    justify-content: center;
}

.announcement-leading-icon {
    color: var(--oj-color-primary);
}

.announcement-item-title {
    overflow: hidden;
    text-overflow: ellipsis;
    font-size: var(--oj-font-size-lg);
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
    white-space: nowrap;
    transition: color 0.2s ease;
}

.announcement-card-inner:hover .announcement-item-title {
    color: var(--oj-color-primary);
}

.announcement-date {
    display: inline-flex;
    gap: var(--oj-spacing-1);
    align-items: center;
    font-family: var(--oj-font-family-mono), monospace;
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-muted);
}

.date-icon :deep(svg) {
    width: 16px;
    height: 16px;
}

.announcement-stats {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-2);
    align-items: flex-end;
    justify-content: flex-end;
}

.stat-item {
    display: inline-flex;
    gap: var(--oj-spacing-1);
    align-items: center;
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-muted);
}

.stat-icon :deep(svg) {
    width: 16px;
    height: 16px;
}

.announcement-pagination {
    display: flex;
    justify-content: center;
    padding: var(--oj-spacing-4) var(--oj-spacing-6) var(--oj-spacing-6);
    border-top: 1px solid var(--oj-glass-border);
}

@media (width <=768px) {
    .announcement-page {
        padding: var(--oj-spacing-4);
    }

    .announcement-list {
        gap: var(--oj-spacing-3);
        padding: var(--oj-spacing-4);
    }
}
</style>
