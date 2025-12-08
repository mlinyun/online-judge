<script setup lang="ts">
import { ref, onMounted } from "vue";
import { useRouter } from "vue-router";
import { selectUserRank } from "@/api/user";
import type { Api } from "@/types/api/api";
import { UserFilled } from "@element-plus/icons-vue";

// ==================== 状态定义 ====================
const router = useRouter();
const loading = ref(false);
const rankList = ref<Api.User.UserRankListItem[]>([]);
const total = ref(0);
const currentPage = ref(1);
const pageSize = ref(20);

// ==================== 数据获取 ====================
const fetchData = async () => {
    loading.value = true;
    try {
        // 调用 API 获取排行榜数据
        const res = await selectUserRank({
            Page: currentPage.value,
            PageSize: pageSize.value,
        });

        // 根据新的 ApiResponse 结构判断：{ success, code, message, data }
        if (res.data.success && res.data.data) {
            rankList.value = res.data.data.List;
            console.log("RankLis: ", rankList.value);
            total.value = res.data.data.Total;
        } else {
            // 请求失败，错误信息已在 http 拦截器中处理
            rankList.value = [];
            total.value = 0;
        }
    } catch (error) {
        // 网络错误或其他异常，错误信息已在 http 拦截器中处理
        console.error("获取排行榜数据失败:", error);
        rankList.value = [];
        total.value = 0;
    } finally {
        loading.value = false;
    }
};

// ==================== 事件处理 ====================
const handlePageChange = (page: number) => {
    currentPage.value = page;
    fetchData();
};

const handleSizeChange = (size: number) => {
    pageSize.value = size;
    currentPage.value = 1; // 重置到第一页
    fetchData();
};

// 行点击事件，跳转到用户详情页
const handleRowClick = (row: Api.User.UserRankListItem) => {
    router.push(`/user/info?UserId=${row._id}`);
};

// ==================== 辅助函数 ====================
const getRankColor = (rank: number) => {
    if (rank === 1) return "#FFD700"; // Gold
    if (rank === 2) return "#C0C0C0"; // Silver
    if (rank === 3) return "#CD7F32"; // Bronze
    return "var(--el-text-color-secondary)";
};

/**
 * 计算通过率
 * @param ac 通过题目数量
 * @param submit 提交判题数量
 */
const calculatePassRate = (ac: number, submit: number) => {
    if (submit === 0) return "0.00%";
    return ((ac / submit) * 100).toFixed(2) + "%";
};

// ==================== 生命周期 ====================
onMounted(() => {
    fetchData();
});
</script>

<template>
    <div class="ranking-board-wrapper">
        <el-card class="glass-card" shadow="hover">
            <!-- 头部区域 -->
            <template #header>
                <div class="header-content">
                    <div class="title-section">
                        <div class="icon-box">
                            <el-icon :size="24" color="#409EFF">
                                <i-ep-trophy />
                            </el-icon>
                        </div>
                        <div class="text-box">
                            <h2 class="main-title">用户排行榜</h2>
                            <span class="sub-title">Top Users Ranking</span>
                        </div>
                    </div>
                    <div class="stats-tag">
                        <el-tag effect="dark" round type="primary">Total: {{ total }}</el-tag>
                    </div>
                </div>
            </template>

            <!-- 表格区域 -->
            <div class="table-container">
                <el-skeleton :loading="loading" animated :rows="10">
                    <template #default>
                        <div v-if="!loading && rankList.length === 0" class="empty-state">
                            <el-empty description="暂无排名数据" :image-size="200" />
                        </div>

                        <el-table
                            v-else
                            :data="rankList"
                            :style="{ width: '100%' }"
                            class="ranking-table"
                            :row-class-name="({ row }) => `rank-row rank-${row.Rank}`"
                            @row-click="handleRowClick"
                            highlight-current-row
                        >
                            <!-- 排名列 -->
                            <el-table-column label="排名" width="100" align="center">
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
                                                {{ row.PersonalProfile || "这位大佬很低调，什么都没写~" }}
                                            </div>
                                        </div>
                                    </div>
                                </template>
                            </el-table-column>

                            <!-- 数据统计列 -->
                            <el-table-column label="通过/提交" width="200" align="center">
                                <template #default="{ row }">
                                    <div class="stats-cell">
                                        <div class="stat-item ac">
                                            <span class="label">AC</span>
                                            <span class="value">{{ row.ACNum }}</span>
                                        </div>
                                        <div class="divider">/</div>
                                        <div class="stat-item submit">
                                            <span class="label">Total</span>
                                            <span class="value">{{ row.SubmitNum }}</span>
                                        </div>
                                    </div>
                                </template>
                            </el-table-column>

                            <el-table-column label="通过率" width="120" align="center">
                                <template #default="{ row }">
                                    <el-tag
                                        :type="
                                            calculatePassRate(row.ACNum, row.SubmitNum) === '100.00%'
                                                ? 'success'
                                                : 'info'
                                        "
                                        effect="light"
                                        round
                                    >
                                        {{ calculatePassRate(row.ACNum, row.SubmitNum) }}
                                    </el-tag>
                                </template>
                            </el-table-column>
                        </el-table>
                    </template>
                </el-skeleton>
            </div>

            <!-- 分页区域 -->
            <div class="pagination-wrapper">
                <el-pagination
                    v-model:current-page="currentPage"
                    v-model:page-size="pageSize"
                    :total="total"
                    :page-sizes="[10, 20, 50, 100]"
                    layout="total, sizes, prev, pager, next, jumper"
                    @size-change="handleSizeChange"
                    @current-change="handlePageChange"
                    background
                />
            </div>
        </el-card>
    </div>
</template>

<style scoped>
/* 容器与卡片基础样式 */
.ranking-board-wrapper {
    box-sizing: border-box;
    width: 100%;
    padding: 20px;
}

.glass-card {
    overflow: hidden;
    background: rgb(255 255 255 / 70%);
    border: 1px solid rgb(255 255 255 / 30%);
    border-radius: 16px;
    box-shadow: 0 8px 32px 0 rgb(31 38 135 / 10%);
    backdrop-filter: blur(20px);
    transition: all 0.3s ease;
}

.dark .glass-card {
    background: rgb(30 30 30 / 60%);
    border: 1px solid rgb(255 255 255 / 5%);
    box-shadow: 0 8px 32px 0 rgb(0 0 0 / 30%);
}

.glass-card:hover {
    box-shadow: 0 12px 40px 0 rgb(31 38 135 / 15%);
    transform: translateY(-2px);
}

/* 头部样式 */
.header-content {
    display: flex;
    align-items: center;
    justify-content: space-between;
}

.title-section {
    display: flex;
    gap: 12px;
    align-items: center;
}

.icon-box {
    display: flex;
    align-items: center;
    justify-content: center;
    width: 48px;
    height: 48px;
    background: linear-gradient(135deg, #e6f7ff 0%, #bae7ff 100%);
    border-radius: 12px;
    box-shadow: 0 4px 12px rgb(64 158 255 / 20%);
}

.dark .icon-box {
    background: linear-gradient(135deg, #1a3d5c 0%, #0f2942 100%);
    box-shadow: none;
}

.text-box {
    display: flex;
    flex-direction: column;
}

.main-title {
    margin: 0;
    font-size: 20px;
    font-weight: 700;
    color: var(--el-text-color-primary);
    letter-spacing: 0.5px;
}

.sub-title {
    margin-top: 4px;
    font-size: 12px;
    color: var(--el-text-color-secondary);
    text-transform: uppercase;
    letter-spacing: 1px;
}

/* 表格样式重写与增强 */
.table-container {
    margin: 20px 0;
}

:deep(.el-table) {
    overflow: hidden;
    background-color: transparent !important;
    border-radius: 8px;

    --el-table-tr-bg-color: transparent;
    --el-table-header-bg-color: rgb(var(--el-color-primary-rgb), 0.05);
    --el-table-row-hover-bg-color: rgb(var(--el-color-primary-rgb), 0.08);
}

:deep(.el-table__inner-wrapper::before) {
    display: none;

    /* 移除底部边框 */
}

:deep(.el-table tr) {
    cursor: pointer;
    transition: all 0.2s ease;
}

/* 排名徽章 */
.rank-badge {
    display: flex;
    align-items: center;
    justify-content: center;
    font-size: 16px;
    font-weight: bold;
    color: var(--el-text-color-secondary);
}

.rank-number {
    width: 32px;
    height: 32px;
    font-family: Monaco, monospace;
    line-height: 32px;
    background: var(--el-fill-color-light);
    border-radius: 50%;
}

.top-three .rank-number {
    font-size: 0;
    background: transparent;

    /* Hide number if icon is shown, though v-if handles it */
}

/* 用户信息 */
.user-info {
    display: flex;
    gap: 16px;
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
    border-color: #ffd700;
    box-shadow: 0 0 15px rgb(255 215 0 / 30%);
}

.crown-icon {
    position: absolute;
    top: -5px;
    right: -6px;
    font-size: 16px;
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

    /* Fix flex text truncation */
}

.nickname-row {
    display: flex;
    gap: 8px;
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

/* 统计数据 */
.stats-cell {
    display: flex;
    gap: 8px;
    align-items: center;
    justify-content: center;
    font-family: "Roboto Mono", monospace;
}

.stat-item {
    display: flex;
    flex-direction: column;
    align-items: center;
}

.stat-item .label {
    margin-bottom: 2px;
    font-size: 10px;
    color: var(--el-text-color-placeholder);
}

.stat-item .value {
    font-size: 14px;
    font-weight: 600;
}

.stat-item.ac .value {
    color: var(--el-color-success);
}

.stat-item.submit .value {
    color: var(--el-text-color-regular);
}

.divider {
    font-size: 14px;
    color: var(--el-border-color);
}

/* 分页 */
.pagination-wrapper {
    display: flex;
    justify-content: center;
    margin-top: 24px;
}

/* 响应式调整 */
@media (width <= 768px) {
    .ranking-board-wrapper {
        padding: 10px;
    }

    .bio {
        display: none;
    }

    .stats-cell {
        flex-direction: column;
        gap: 2px;
    }

    .divider {
        display: none;
    }
}
</style>
