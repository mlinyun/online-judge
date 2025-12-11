<script setup lang="ts">
import { ref, onMounted, computed } from "vue";
import { useRouter } from "vue-router";
import { selectAnnouncementList } from "@/api/announcement";
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

// 格式化日期
const formatDate = (dateString: string) => {
    const date = new Date(dateString);
    return date.toLocaleDateString("zh-CN", {
        year: "numeric",
        month: "2-digit",
        day: "2-digit",
    });
};

// 导航到公告详情
const navigateToAnnouncement = (announcementId: Api.Announcement.AnnouncementId) => {
    router.push(`announcement/detail/${announcementId}`);
};

// 查看所有公告
const viewAllAnnouncements = () => {
    router.push("/announcement-list");
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
                <span class="icon-bullhorn"></span>
                最新公告
            </h2>
            <button class="view-all-btn" @click="viewAllAnnouncements">
                查看全部
                <span class="icon-angle-right"></span>
            </button>
        </div>

        <!-- 加载状态 -->
        <div v-if="loading" class="announcement-loading">
            <div class="loading-spinner"></div>
            <p>加载中...</p>
        </div>

        <!-- 错误状态 -->
        <div v-else-if="error" class="announcement-error">
            <p>加载失败，请稍后重试</p>
            <button class="retry-btn" @click="fetchAnnouncements">重试</button>
            <p>{{ error }}</p>
        </div>

        <!-- 空状态 -->
        <div v-else-if="!hasData" class="announcement-empty">
            <p>暂无公告</p>
        </div>

        <!-- 公告列表 -->
        <div v-else class="announcement-list">
            <div
                v-for="announcement in announcements"
                :key="announcement._id"
                class="announcement-item"
                @click="navigateToAnnouncement(announcement._id)"
            >
                <div class="announcement-content">
                    <h3 class="announcement-item-title">{{ announcement.Title }}</h3>
                    <p class="announcement-preview">暂无内容预览</p>
                </div>
                <div class="announcement-meta">
                    <span class="announcement-date">{{ formatDate(announcement.CreateTime) }}</span>
                    <div class="announcement-stats">
                        <span class="stat-item">
                            <i class="icon-eye"></i>
                            {{ announcement.Views }}
                        </span>
                        <span class="stat-item">
                            <i class="icon-comment"></i>
                            {{ announcement.Comments }}
                        </span>
                    </div>
                </div>
            </div>
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

.icon-bullhorn::before {
    content: "📢";
}

.view-all-btn {
    display: flex;
    gap: var(--oj-spacing-1);
    align-items: center;
    font-size: var(--oj-font-size-sm);
    color: var(--oj-color-primary);
    cursor: pointer;
    background: transparent;
    border: none;
    transition: color 0.3s ease;
}

.view-all-btn:hover {
    color: var(--oj-text-color);
}

.icon-angle-right::before {
    font-size: var(--oj-font-size-lg);
    content: "›";
}

.announcement-list {
    border-top: 1px solid var(--oj-glass-border);
}

.announcement-item {
    padding: var(--oj-spacing-6);
    cursor: pointer;
    border-bottom: 1px solid var(--oj-glass-border);
    transition: background-color 0.2s ease;
}

.announcement-item:last-child {
    border-bottom: none;
}

.announcement-item:hover {
    background-color: var(--oj-surface-hover);
}

.announcement-content {
    flex: 1;
    margin-bottom: var(--oj-spacing-3);
}

.announcement-item-title {
    margin-bottom: var(--oj-spacing-2);
    font-size: var(--oj-font-size-lg);
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
    transition: color 0.3s ease;
}

.announcement-item:hover .announcement-item-title {
    color: var(--oj-color-primary);
}

.announcement-preview {
    display: -webkit-box;
    overflow: hidden;
    text-overflow: ellipsis;
    -webkit-line-clamp: 2;
    line-clamp: 2;
    font-size: var(--oj-font-size-sm);
    line-height: var(--oj-line-height-relaxed);
    color: var(--oj-text-secondary);
    -webkit-box-orient: vertical;
}

.announcement-meta {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-2);
    align-items: center;
    justify-content: space-between;
}

.announcement-date {
    font-family: var(--oj-font-family-mono);
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-muted);
}

.announcement-stats {
    display: flex;
    gap: var(--oj-spacing-3);
    align-items: center;
}

.stat-item {
    display: flex;
    gap: var(--oj-spacing-1);
    align-items: center;
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-muted);
}

.icon-eye::before {
    content: "👁️";
}

.icon-comment::before {
    content: "💬";
}

/* Loading state */
.announcement-loading {
    padding: var(--oj-spacing-16) var(--oj-spacing-6);
    color: var(--oj-text-secondary);
    text-align: center;
}

.loading-spinner {
    width: 40px;
    height: 40px;
    margin: 0 auto var(--oj-spacing-4);
    border: 3px solid var(--oj-glass-border);
    border-top-color: var(--oj-color-primary);
    border-radius: 50%;
    animation: spin 1s linear infinite;
}

@keyframes spin {
    to {
        transform: rotate(360deg);
    }
}

/* Error state */
.announcement-error {
    padding: var(--oj-spacing-16) var(--oj-spacing-6);
    color: var(--oj-text-secondary);
    text-align: center;
}

.retry-btn {
    padding: var(--oj-spacing-2) var(--oj-spacing-4);
    margin-top: var(--oj-spacing-4);
    color: var(--oj-text-inverse);
    cursor: pointer;
    background-color: var(--oj-color-primary);
    border: none;
    border-radius: var(--oj-radius-md);
    transition: all 0.3s ease;
}

.retry-btn:hover {
    background-color: var(--oj-color-primary-dark);
    transform: translateY(-1px);
}

/* Empty state */
.announcement-empty {
    padding: var(--oj-spacing-16) var(--oj-spacing-6);
    color: var(--oj-text-secondary);
    text-align: center;
}

@media (width >= 768px) {
    .announcement-item {
        display: flex;
        align-items: flex-start;
        justify-content: space-between;
    }

    .announcement-content {
        margin-right: var(--oj-spacing-4);
        margin-bottom: 0;
    }

    .announcement-meta {
        flex-shrink: 0;
        flex-direction: column;
        align-items: flex-end;
    }
}
</style>
