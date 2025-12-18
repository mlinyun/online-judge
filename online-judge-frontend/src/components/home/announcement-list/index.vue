<script setup lang="ts">
import { ref, onMounted, computed } from "vue";
import { useRouter } from "vue-router";
import { ChatDotSquare, View, ChatRound, Bell, ArrowRight, Calendar } from "@element-plus/icons-vue";
import { selectAnnouncementList } from "@/api/announcement";
import { DateUtils } from "@/utils/date/date-utils.ts";
import type { Api } from "@/types/api/api";

defineOptions({ name: "AnnouncementList" });

const router = useRouter();

// 响应式数据
const announcements = ref<Api.Announcement.SelectAnnouncementListItem[]>([]);
const loading = ref(false);
const error = ref(false);

// 获取公告列表
const fetchAnnouncements = async () => {
    loading.value = true;
    error.value = false;

    try {
        // 调用 API 获取公告列表，分页参数：第 1 页，5 条数据
        const response = await selectAnnouncementList({
            Page: 1,
            PageSize: 5,
        });
        // 判断请求是否成功
        if (response.data.code === 0 && response.data.data) {
            announcements.value = response.data.data.List || [];
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


// 导航到公告详情
const navigateToAnnouncement = (announcementId: Api.Announcement.AnnouncementId) => {
    router.push({ name: "announcement-detail", params: { id: announcementId } });
};

// 查看所有公告
const viewAllAnnouncements = () => {
    router.push("/announcement/list");
};

// 计算是否有数据
const hasData = computed(() => announcements.value.length > 0);

// 组件挂载时获取数据
onMounted(() => {
    fetchAnnouncements();
});
</script>

<template>
    <div class="announcement-section">
        <div class="announcement-header">
            <h2 class="announcement-title">
                <el-icon class="announcement-title-icon">
                    <Bell />
                </el-icon>
                最新公告
            </h2>
            <el-button class="view-all-btn" link type="primary" @click="viewAllAnnouncements">
                查看全部
                <el-icon class="view-all-icon">
                    <ArrowRight />
                </el-icon>
            </el-button>
        </div>

        <!-- 加载状态 -->
        <div v-if="loading" class="announcement-loading">
            <el-skeleton :rows="4" animated />
        </div>

        <!-- 错误状态 -->
        <div v-else-if="error" class="announcement-error">
            <el-result icon="error" title="加载失败" sub-title="请稍后重试">
                <template #extra>
                    <el-button type="primary" @click="fetchAnnouncements">重试</el-button>
                </template>
            </el-result>
        </div>

        <!-- 空状态 -->
        <div v-else-if="!hasData" class="announcement-empty">
            <el-empty description="暂无公告" />
        </div>

        <!-- 公告列表 -->
        <div v-else class="announcement-list">
            <el-card v-for="announcement in announcements" :key="announcement._id" class="announcement-card"
                shadow="never">
                <div class="announcement-card-inner" role="button" tabindex="0"
                    @click="navigateToAnnouncement(announcement._id)"
                    @keydown.enter="navigateToAnnouncement(announcement._id)">
                    <el-row :gutter="12" align="middle">
                        <el-col :span="3" class="announcement-leading">
                            <el-icon size="32" class="announcement-leading-icon">
                                <ChatDotSquare />
                            </el-icon>
                        </el-col>
                        <el-col :span="15" class="announcement-main">
                            <el-row :gutter="12" class="announcement-top" align="middle">
                                <el-col :span="24" class="announcement-title-col">
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
    </div>
</template>

<style scoped>
.announcement-section {
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
    font-size: var(--oj-font-size-xl);
    font-weight: var(--oj-font-weight-bold);
    color: var(--oj-text-color);
}

.announcement-title-icon {
    color: var(--oj-color-primary);
}

.view-all-btn {
    font-size: var(--oj-font-size-sm);
}

.view-all-icon {
    margin-left: var(--oj-spacing-1);
}

.announcement-list {
    display: grid;
    gap: var(--oj-spacing-3);
    padding: var(--oj-spacing-4);
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

.announcement-leading-icon :deep(svg) {
    width: 36px;
    height: 36px;
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
    font-family: var(--oj-font-family-mono);
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

/* Loading state */
.announcement-loading {
    padding: var(--oj-spacing-8) var(--oj-spacing-6);
    color: var(--oj-text-secondary);
}

/* Error state */
.announcement-error {
    padding: var(--oj-spacing-8) var(--oj-spacing-6);
}

/* Empty state */
.announcement-empty {
    padding: var(--oj-spacing-8) var(--oj-spacing-6);
}

@media (width >=768px) {
    .announcement-list {
        gap: var(--oj-spacing-4);
        padding: var(--oj-spacing-5);
    }
}
</style>
