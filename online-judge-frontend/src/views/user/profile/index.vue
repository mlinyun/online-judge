<script setup lang="ts">
/**
 * 用户个人主页
 *
 * @description 展示用户信息、提交统计、通过题目列表
 */
import { selectUserInfo, selectUserRankById } from "@/api/user";
import { useUserStore } from "@/stores/modules/user";
import { OjPieChart } from "@/components/common";
import type { PieDataItem } from "@/components/common/pie-chart/index.vue";
import type { Api } from "@/types/api/api";

defineOptions({ name: "UserProfile" });

const route = useRoute();
const router = useRouter();
const userStore = useUserStore();

const loading = ref(true);
const profileData = ref<Api.User.SelectUserInfoResult | null>(null);
const userRank = ref<number>(0);

/** 获取目标用户 ID：路由参数 > 当前登录用户 */
const getTargetUserId = (): Api.User.UserId => {
    const paramId = route.params.id as string;
    if (paramId) return paramId as Api.User.UserId;
    const storeId = userStore.getUserInfo._id;
    if (storeId && storeId !== "0") return storeId;
    return "0" as Api.User.UserId;
};

const fetchUserProfile = async () => {
    loading.value = true;
    try {
        const userId = getTargetUserId();
        if (userId === "0") {
            router.push({ name: "auth-login" });
            return;
        }
        const [infoRes, rankRes] = await Promise.all([selectUserInfo(userId), selectUserRankById(userId)]);
        if (infoRes.data.success && infoRes.data.data) {
            profileData.value = infoRes.data.data;
        }
        if (rankRes.data.success && rankRes.data.data) {
            userRank.value = rankRes.data.data.Rank;
        }
    } catch (e) {
        console.error("获取用户信息失败:", e);
    } finally {
        loading.value = false;
    }
};

/** 提交统计饼图数据 */
const pieData = computed<PieDataItem[]>(() => {
    if (!profileData.value) return [];
    const ac = profileData.value.ACNum;
    const total = profileData.value.SubmitNum;
    const failed = total - ac;
    if (total === 0) return [];
    return [
        { name: "通过", value: ac, color: "rgba(52, 211, 153, 0.85)" },
        { name: "未通过", value: failed > 0 ? failed : 0, color: "rgba(248, 113, 113, 0.85)" },
    ];
});

/** 格式化加入时间 */
const formatJoinTime = (time: string): string => {
    if (!time) return "N/A";
    const d = new Date(time);
    return d.toLocaleDateString("zh-CN", { year: "numeric", month: "long", day: "numeric" });
};

/** 是否是查看自己的主页 */
const isSelf = computed(() => {
    const paramId = route.params.id as string;
    if (!paramId) return true;
    return paramId === userStore.getUserInfo._id;
});

/** 跳转到题目详情 */
const goToProblem = (problemId: string) => {
    router.push({ name: "problem-detail", params: { id: problemId } });
};

onMounted(() => {
    fetchUserProfile();
});

// 路由参数变化时重新加载
watch(
    () => route.params.id,
    () => {
        fetchUserProfile();
    },
);
</script>

<template>
    <div class="user-profile-page">
        <el-skeleton :loading="loading" animated :count="3">
            <template #template>
                <div class="skeleton-card" />
                <div class="skeleton-card" style="height: 320px" />
                <div class="skeleton-card" style="height: 180px" />
            </template>
            <template #default>
                <template v-if="profileData">
                    <!-- 用户信息卡片 -->
                    <div class="profile-card">
                        <div class="profile-header">
                            <el-avatar :size="80" :src="profileData.Avatar" shape="square" class="profile-avatar" />
                            <div class="profile-info">
                                <div class="profile-name-row">
                                    <h1 class="profile-name">{{ profileData.NickName }}</h1>
                                    <el-tag
                                        v-if="userRank > 0"
                                        type="warning"
                                        effect="dark"
                                        size="small"
                                        round
                                    >
                                        <el-icon><i-ep-trophy /></el-icon>
                                        Rank #{{ userRank }}
                                    </el-tag>
                                </div>
                                <p v-if="profileData.PersonalProfile" class="profile-bio">
                                    {{ profileData.PersonalProfile }}
                                </p>
                                <div class="profile-meta">
                                    <span class="meta-item">
                                        <el-icon><i-ep-school /></el-icon>
                                        {{ profileData.School || "未设置" }}
                                    </span>
                                    <span class="meta-item">
                                        <el-icon><i-ep-collection /></el-icon>
                                        {{ profileData.Major || "未设置" }}
                                    </span>
                                    <span class="meta-item">
                                        <el-icon><i-ep-calendar /></el-icon>
                                        {{ formatJoinTime(profileData.JoinTime) }}
                                    </span>
                                </div>
                            </div>
                            <el-button
                                v-if="isSelf"
                                class="edit-btn"
                                @click="router.push({ name: 'user-setting' })"
                            >
                                <el-icon><i-ep-edit /></el-icon>
                                <span>编辑资料</span>
                            </el-button>
                        </div>
                    </div>

                    <!-- 统计 + 饼图 -->
                    <div class="stats-section">
                        <div class="stats-grid">
                            <div class="stat-card accent-cyan">
                                <div class="stat-number">{{ profileData.ACNum }}</div>
                                <div class="stat-label">通过题目</div>
                            </div>
                            <div class="stat-card accent-amber">
                                <div class="stat-number">{{ profileData.SubmitNum }}</div>
                                <div class="stat-label">总提交</div>
                            </div>
                            <div class="stat-card accent-emerald">
                                <div class="stat-number">
                                    {{
                                        profileData.SubmitNum > 0
                                            ? ((profileData.ACNum / profileData.SubmitNum) * 100).toFixed(1)
                                            : "0.0"
                                    }}%
                                </div>
                                <div class="stat-label">通过率</div>
                            </div>
                        </div>
                        <div v-if="pieData.length > 0" class="chart-wrapper">
                            <OjPieChart :data="pieData" title="提交统计" />
                        </div>
                        <div v-else class="empty-chart">
                            <el-icon :size="36"><i-ep-data-line /></el-icon>
                            <p>暂无提交数据</p>
                        </div>
                    </div>

                    <!-- 通过的题目 -->
                    <div class="solves-section">
                        <h2 class="section-title">
                            <el-icon><i-ep-check /></el-icon>
                            <span>通过的题目 ({{ profileData.Solves.length }})</span>
                        </h2>
                        <div v-if="profileData.Solves.length > 0" class="solves-grid">
                            <el-tag
                                v-for="id in profileData.Solves"
                                :key="id"
                                class="solve-tag"
                                effect="plain"
                                @click="goToProblem(id)"
                            >
                                #{{ id }}
                            </el-tag>
                        </div>
                        <el-empty v-else description="还没有通过任何题目" :image-size="80" />
                    </div>
                </template>
                <el-empty v-else description="用户不存在" />
            </template>
        </el-skeleton>
    </div>
</template>

<style scoped>
.user-profile-page {
    max-width: 840px;
    padding: var(--oj-spacing-6) var(--oj-spacing-4);
    margin: 0 auto;
}

/* ── 骨架屏 ── */
.skeleton-card {
    height: 160px;
    margin-bottom: var(--oj-spacing-6);
    background: rgb(var(--oj-color-primary-rgb) / 8%);
    border-radius: var(--oj-radius-xl);
}

/* ── Profile 卡片 ── */
.profile-card {
    padding: var(--oj-spacing-6);
    margin-bottom: var(--oj-spacing-6);
    background: var(--oj-card-bg);
    border: 1px solid var(--oj-card-border);
    border-radius: var(--oj-radius-xl);
    backdrop-filter: blur(var(--oj-glass-blur));
}

.profile-header {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-5);
    align-items: flex-start;
}

.profile-avatar {
    flex-shrink: 0;
    border: 2px solid var(--oj-color-primary);
    border-radius: var(--oj-radius-xl);
    box-shadow: 0 0 16px rgb(var(--oj-color-primary-rgb) / 25%);
}

.profile-info {
    flex: 1;
    min-width: 0;
}

.profile-name-row {
    display: flex;
    gap: var(--oj-spacing-3);
    align-items: center;
    margin-bottom: var(--oj-spacing-2);
}

.profile-name {
    margin: 0;
    font-size: var(--oj-font-size-2xl);
    font-weight: var(--oj-font-weight-bold);
    color: var(--oj-text-color);
}

.profile-bio {
    margin: 0 0 var(--oj-spacing-3);
    font-size: var(--oj-font-size-sm);
    line-height: 1.6;
    color: var(--oj-text-color-secondary);
}

.profile-meta {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-4);
}

.meta-item {
    display: inline-flex;
    gap: var(--oj-spacing-1);
    align-items: center;
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-color-muted);
}

.edit-btn {
    flex-shrink: 0;
    color: var(--oj-color-primary);
    background: var(--oj-color-primary-soft);
    border: 1px solid rgb(var(--oj-color-primary-rgb) / 30%);
}

.edit-btn:hover {
    background: rgb(var(--oj-color-primary-rgb) / 15%);
    box-shadow: 0 0 12px rgb(var(--oj-color-primary-rgb) / 20%);
}

/* ── Stats 区域 ── */
.stats-section {
    padding: var(--oj-spacing-6);
    margin-bottom: var(--oj-spacing-6);
    background: var(--oj-card-bg);
    border: 1px solid var(--oj-card-border);
    border-radius: var(--oj-radius-xl);
    backdrop-filter: blur(var(--oj-glass-blur));
}

.stats-grid {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: var(--oj-spacing-4);
    margin-bottom: var(--oj-spacing-6);
}

.stat-card {
    padding: var(--oj-spacing-4);
    text-align: center;
    border-radius: var(--oj-radius-lg);
}

.stat-card.accent-cyan {
    background: rgb(6 182 212 / 10%);
    border: 1px solid rgb(6 182 212 / 20%);
}

.stat-card.accent-amber {
    background: rgb(245 158 11 / 10%);
    border: 1px solid rgb(245 158 11 / 20%);
}

.stat-card.accent-emerald {
    background: rgb(52 211 153 / 10%);
    border: 1px solid rgb(52 211 153 / 20%);
}

.stat-number {
    font-size: var(--oj-font-size-2xl);
    font-weight: var(--oj-font-weight-bold);
    color: var(--oj-text-color);
}

.stat-label {
    margin-top: var(--oj-spacing-1);
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-muted);
}

.chart-wrapper {
    min-height: 280px;
}

.empty-chart {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-2);
    align-items: center;
    justify-content: center;
    min-height: 160px;
    color: var(--oj-text-color-muted);
}

/* ── 通过的题目 ── */
.solves-section {
    padding: var(--oj-spacing-6);
    background: var(--oj-card-bg);
    border: 1px solid var(--oj-card-border);
    border-radius: var(--oj-radius-xl);
    backdrop-filter: blur(var(--oj-glass-blur));
}

.section-title {
    display: flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    margin: 0 0 var(--oj-spacing-4);
    font-size: var(--oj-font-size-lg);
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
}

.solves-grid {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-2);
}

.solve-tag {
    cursor: pointer;
    transition: all 0.2s ease;
}

.solve-tag:hover {
    color: var(--oj-color-primary);
    border-color: var(--oj-color-primary);
    box-shadow: 0 0 8px rgb(var(--oj-color-primary-rgb) / 20%);
    transform: translateY(-1px);
}

/* ── 响应式 ── */
@media (width < 640px) {
    .stats-grid {
        grid-template-columns: 1fr;
    }

    .profile-header {
        flex-direction: column;
        align-items: center;
        text-align: center;
    }

    .profile-meta {
        justify-content: center;
    }
}

@media (width >= 640px) {
    .user-profile-page {
        padding: var(--oj-spacing-8);
    }
}
</style>
