<script setup lang="ts">
/**
 * 用户个人主页
 *
 * @description 展示用户信息、提交活跃度、通过题目列表、近期提交记录
 */
import { selectUserInfo, selectUserRankById } from "@/api/user";
import { selectStatusRecordList } from "@/api/status";
import { useUserStore } from "@/stores/modules/user";
import { DateUtils } from "@/utils/date/date-utils";
import type { Api } from "@/types/api/api";

defineOptions({ name: "UserProfile" });

type StatusRow = Api.Status.SelectStatusRecordListItem;
type StatusTagType = "success" | "warning" | "danger" | "info";

const route = useRoute();
const router = useRouter();
const userStore = useUserStore();

const loading = ref(true);
const profileData = ref<Api.User.SelectUserInfoResult | null>(null);
const userRank = ref<number>(0);
const recentSubmissions = ref<StatusRow[]>([]);
const submissionsLoading = ref(false);

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
        // 加载最近提交记录
        fetchRecentSubmissions(userId);
    } catch (e) {
        console.error("获取用户信息失败:", e);
    } finally {
        loading.value = false;
    }
};

/** 加载最近提交记录 */
const fetchRecentSubmissions = async (userId: Api.User.UserId) => {
    submissionsLoading.value = true;
    try {
        const res = await selectStatusRecordList({
            Page: 1,
            PageSize: 10,
            SearchInfo: { UserId: userId },
        });
        if (res.data.success && res.data.data) {
            recentSubmissions.value = res.data.data.List;
        }
    } catch {
        console.error("获取提交记录失败");
    } finally {
        submissionsLoading.value = false;
    }
};

/* ── 判题状态映射 ── */
const getStatusTitle = (status: number): string => {
    const map: Record<number, string> = {
        0: "Pending",
        1: "Compile Error",
        2: "Accepted",
        3: "Wrong Answer",
        4: "Runtime Error",
        5: "Time Limit Exceeded",
        6: "Memory Limit Exceeded",
        7: "System Error",
    };
    return map[status] ?? `Unknown (${status})`;
};

const getStatusTagType = (status: number): StatusTagType => {
    if (status === 0) return "info";
    if (status === 2) return "success";
    if (status === 3) return "danger";
    return "warning";
};

/** 提交活跃度热力图数据 (近 365 天) */
const heatmapData = computed(() => {
    if (!recentSubmissions.value.length) return [] as { date: string; opacity: number }[];
    // 统计每日提交次数
    const countMap = new Map<string, number>();
    for (const s of recentSubmissions.value) {
        const d = new Date(s.SubmitTime).toISOString().slice(0, 10);
        countMap.set(d, (countMap.get(d) ?? 0) + 1);
    }
    const maxCount = Math.max(...countMap.values(), 1);

    const days: { date: string; opacity: number }[] = [];
    const today = new Date();
    for (let i = 364; i >= 0; i--) {
        const d = new Date(today);
        d.setDate(d.getDate() - i);
        const key = d.toISOString().slice(0, 10);
        const count = countMap.get(key) ?? 0;
        days.push({ date: key, opacity: count > 0 ? 0.25 + (count / maxCount) * 0.75 : 0.08 });
    }
    return days;
});

/** 通过率 */
const passRate = computed(() => {
    if (!profileData.value || profileData.value.SubmitNum === 0) return "0.0";
    return ((profileData.value.ACNum / profileData.value.SubmitNum) * 100).toFixed(1);
});

/** 格式化加入时间 */
const formatJoinTime = (time: string): string => {
    if (!time) return "N/A";
    const d = new Date(time);
    return d.toLocaleDateString("zh-CN", { year: "numeric", month: "long", day: "numeric" });
};

/** 相对时间格式化 */
const formatRelativeTime = (time: string): string => {
    const now = Date.now();
    const t = new Date(time).getTime();
    const diff = now - t;
    if (diff < 0) return "刚刚";
    const seconds = Math.floor(diff / 1000);
    if (seconds < 60) return "刚刚";
    const minutes = Math.floor(seconds / 60);
    if (minutes < 60) return `${minutes}分钟前`;
    const hours = Math.floor(minutes / 60);
    if (hours < 24) return `${hours}小时前`;
    const days = Math.floor(hours / 24);
    if (days < 30) return `${days}天前`;
    return DateUtils.formatDateTime(time);
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
    }
);
</script>

<template>
    <div class="user-profile-page">
        <el-skeleton :loading="loading" animated :count="3">
            <template #template>
                <div class="skeleton-layout">
                    <div class="skeleton-left">
                        <div class="skeleton-card" style="height: 380px" />
                        <div class="skeleton-card" style="height: 180px" />
                    </div>
                    <div class="skeleton-right">
                        <div class="skeleton-card" style="height: 120px" />
                        <div class="skeleton-card" style="height: 200px" />
                        <div class="skeleton-card" style="height: 260px" />
                    </div>
                </div>
            </template>
            <template #default>
                <template v-if="profileData">
                    <div class="profile-layout">
                        <!-- ════════ 左列：用户信息 ════════ -->
                        <div class="profile-left">
                            <!-- 个人卡片 -->
                            <div class="profile-card">
                                <div class="profile-banner" />
                                <div class="profile-card-body">
                                    <el-avatar :size="96" :src="profileData.Avatar" class="profile-avatar" />
                                    <h1 class="profile-name">{{ profileData.NickName }}</h1>
                                    <p v-if="profileData.PersonalProfile" class="profile-bio">
                                        "{{ profileData.PersonalProfile }}"
                                    </p>

                                    <div class="profile-actions">
                                        <el-button
                                            v-if="isSelf"
                                            type="primary"
                                            class="action-btn-primary"
                                            @click="router.push({ name: 'user-setting' })"
                                        >
                                            <el-icon><i-ep-edit /></el-icon>
                                            编辑资料
                                        </el-button>
                                        <el-button class="action-btn-secondary">
                                            <el-icon><i-ep-share /></el-icon>
                                            分享主页
                                        </el-button>
                                    </div>

                                    <div class="profile-stats-grid">
                                        <div class="profile-stat-item">
                                            <span class="profile-stat-number">{{ profileData.ACNum }}</span>
                                            <span class="profile-stat-label">Solved</span>
                                        </div>
                                        <div class="profile-stat-item">
                                            <span class="profile-stat-number">{{ profileData.SubmitNum }}</span>
                                            <span class="profile-stat-label">Submissions</span>
                                        </div>
                                        <div class="profile-stat-item">
                                            <span class="profile-stat-number">
                                                {{ userRank > 0 ? `#${userRank}` : "-" }}
                                            </span>
                                            <span class="profile-stat-label">Rank</span>
                                        </div>
                                        <div class="profile-stat-item">
                                            <span class="profile-stat-number">{{ passRate }}%</span>
                                            <span class="profile-stat-label">Pass Rate</span>
                                        </div>
                                    </div>
                                </div>
                            </div>

                            <!-- 个人信息 -->
                            <div class="info-card">
                                <h3 class="info-card-title">个人信息</h3>
                                <ul class="info-list">
                                    <li class="info-item">
                                        <el-icon class="info-icon"><i-ep-school /></el-icon>
                                        <span>{{ profileData.School || "未设置" }}</span>
                                    </li>
                                    <li class="info-item">
                                        <el-icon class="info-icon"><i-ep-collection /></el-icon>
                                        <span>{{ profileData.Major || "未设置" }}</span>
                                    </li>
                                    <li class="info-item">
                                        <el-icon class="info-icon"><i-ep-calendar /></el-icon>
                                        <span>加入于 {{ formatJoinTime(profileData.JoinTime) }}</span>
                                    </li>
                                </ul>
                            </div>
                        </div>

                        <!-- ════════ 右列：统计 & 活动 ════════ -->
                        <div class="profile-right">
                            <!-- 活跃度热力图 -->
                            <div class="section-card">
                                <h3 class="section-title">
                                    <el-icon><i-ep-data-analysis /></el-icon>
                                    <span>提交活跃度</span>
                                </h3>
                                <div class="heatmap-container">
                                    <div
                                        v-for="(day, idx) in heatmapData"
                                        :key="idx"
                                        class="heatmap-cell"
                                        :title="`${day.date}`"
                                        :style="{
                                            backgroundColor: `rgb(var(--oj-color-primary-rgb) / ${day.opacity * 100}%)`,
                                        }"
                                    />
                                </div>
                            </div>

                            <!-- 已解决题目 -->
                            <div class="section-card">
                                <h3 class="section-title">
                                    <el-icon><i-ep-check /></el-icon>
                                    <span>已解决题目 ({{ profileData.Solves.length }})</span>
                                </h3>
                                <div v-if="profileData.Solves.length > 0" class="solves-grid">
                                    <span
                                        v-for="id in profileData.Solves"
                                        :key="id"
                                        class="solve-chip"
                                        @click="goToProblem(id)"
                                    >
                                        {{ id }}
                                    </span>
                                </div>
                                <el-empty v-else description="还没有通过任何题目" :image-size="64" />
                            </div>

                            <!-- 最近提交 -->
                            <div class="section-card">
                                <h3 class="section-title">
                                    <el-icon><i-ep-document /></el-icon>
                                    <span>最近提交</span>
                                </h3>
                                <el-skeleton :loading="submissionsLoading" animated :rows="4">
                                    <template #default>
                                        <div v-if="recentSubmissions.length > 0" class="submissions-table-wrap">
                                            <table class="submissions-table">
                                                <thead>
                                                    <tr>
                                                        <th>题目</th>
                                                        <th>结果</th>
                                                        <th>语言</th>
                                                        <th>时间</th>
                                                    </tr>
                                                </thead>
                                                <tbody>
                                                    <tr v-for="row in recentSubmissions" :key="row._id">
                                                        <td>
                                                            <a
                                                                class="problem-link"
                                                                @click.prevent="goToProblem(row.ProblemId)"
                                                            >
                                                                {{ row.ProblemTitle || row.ProblemId }}
                                                            </a>
                                                        </td>
                                                        <td>
                                                            <el-tag
                                                                :type="getStatusTagType(row.Status)"
                                                                size="small"
                                                                effect="light"
                                                            >
                                                                {{ getStatusTitle(row.Status) }}
                                                            </el-tag>
                                                        </td>
                                                        <td class="col-lang">{{ row.Language }}</td>
                                                        <td class="col-time">
                                                            {{ formatRelativeTime(row.SubmitTime) }}
                                                        </td>
                                                    </tr>
                                                </tbody>
                                            </table>
                                        </div>
                                        <el-empty v-else description="暂无提交记录" :image-size="64" />
                                    </template>
                                </el-skeleton>
                            </div>
                        </div>
                    </div>
                </template>
                <el-empty v-else description="用户不存在" />
            </template>
        </el-skeleton>
    </div>
</template>

<style scoped>
.user-profile-page {
    max-width: 1120px;
    padding: var(--oj-spacing-6) var(--oj-spacing-4);
    margin: 0 auto;
}

/* ── 骨架屏 ── */
.skeleton-layout {
    display: grid;
    grid-template-columns: 1fr 2fr;
    gap: var(--oj-spacing-6);
}

.skeleton-left,
.skeleton-right {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-6);
}

.skeleton-card {
    height: 160px;
    background: rgb(var(--oj-color-primary-rgb) / 8%);
    border-radius: var(--oj-radius-xl);
}

/* ── 双栏布局 ── */
.profile-layout {
    display: grid;
    grid-template-columns: 1fr 2fr;
    gap: var(--oj-spacing-6);
    align-items: start;
}

.profile-left,
.profile-right {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-6);
}

/* ── 个人卡片 ── */
.profile-card {
    position: relative;
    overflow: hidden;
    background: var(--oj-card-bg);
    border: 1px solid var(--oj-card-border);
    border-radius: var(--oj-radius-xl);
    backdrop-filter: blur(var(--oj-glass-blur));
}

.profile-banner {
    height: 80px;
    background: linear-gradient(
        135deg,
        rgb(var(--oj-color-primary-rgb) / 25%),
        rgb(var(--oj-color-secondary-rgb) / 25%)
    );
}

.profile-card-body {
    display: flex;
    flex-direction: column;
    align-items: center;
    padding: 0 var(--oj-spacing-6) var(--oj-spacing-6);
    margin-top: -48px;
    text-align: center;
}

.profile-avatar {
    flex-shrink: 0;
    border: 3px solid var(--oj-card-bg);
    border-radius: var(--oj-radius-xl);
    box-shadow: 0 4px 16px rgb(0 0 0 / 30%);
}

.profile-name {
    margin: var(--oj-spacing-3) 0 0;
    font-size: var(--oj-font-size-xl);
    font-weight: var(--oj-font-weight-bold);
    color: var(--oj-text-color);
}

.profile-bio {
    max-width: 100%;
    margin: var(--oj-spacing-3) 0 0;
    font-size: var(--oj-font-size-sm);
    font-style: italic;
    line-height: 1.5;
    color: var(--oj-text-color-muted);
}

.profile-actions {
    display: flex;
    gap: var(--oj-spacing-3);
    margin-top: var(--oj-spacing-5);
}

.action-btn-primary {
    background: var(--oj-color-primary);
    border: none;
    border-radius: var(--oj-radius-lg);
}

.action-btn-primary:hover {
    box-shadow: 0 0 16px rgb(var(--oj-color-primary-rgb) / 40%);
}

.action-btn-secondary {
    color: var(--oj-text-color-secondary);
    background: transparent;
    border: 1px solid var(--oj-border-color-solid);
    border-radius: var(--oj-radius-lg);
}

.action-btn-secondary:hover {
    color: var(--oj-text-color);
    background: var(--oj-surface-hover);
}

/* ── 个人卡片统计数据 2×2 ── */
.profile-stats-grid {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: var(--oj-spacing-4);
    width: 100%;
    padding-top: var(--oj-spacing-5);
    margin-top: var(--oj-spacing-5);
    border-top: 1px solid var(--oj-card-border);
}

.profile-stat-item {
    display: flex;
    flex-direction: column;
    align-items: center;
}

.profile-stat-number {
    font-size: var(--oj-font-size-xl);
    font-weight: var(--oj-font-weight-bold);
    color: var(--oj-text-color);
}

.profile-stat-label {
    margin-top: 2px;
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-muted);
    text-transform: uppercase;
    letter-spacing: 0.05em;
}

/* ── 个人信息卡片 ── */
.info-card {
    padding: var(--oj-spacing-6);
    background: var(--oj-card-bg);
    border: 1px solid var(--oj-card-border);
    border-radius: var(--oj-radius-xl);
    backdrop-filter: blur(var(--oj-glass-blur));
}

.info-card-title {
    margin: 0 0 var(--oj-spacing-4);
    font-size: var(--oj-font-size-lg);
    font-weight: var(--oj-font-weight-bold);
    color: var(--oj-text-color);
}

.info-list {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-3);
    padding: 0;
    margin: 0;
    list-style: none;
}

.info-item {
    display: flex;
    gap: var(--oj-spacing-3);
    align-items: center;
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-color-secondary);
}

.info-icon {
    flex-shrink: 0;
    width: 20px;
    color: var(--oj-color-secondary);
    text-align: center;
}

/* ── 右列区域卡片 ── */
.section-card {
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
    font-weight: var(--oj-font-weight-bold);
    color: var(--oj-text-color);
}

/* ── 活跃度热力图 ── */
.heatmap-container {
    display: flex;
    flex-wrap: wrap;
    gap: 3px;
    justify-content: center;
}

.heatmap-cell {
    width: 12px;
    height: 12px;
    border-radius: 2px;
    transition: opacity 0.15s ease;
}

.heatmap-cell:hover {
    outline: 1px solid var(--oj-color-primary);
    outline-offset: 1px;
}

/* ── 已解决题目 ── */
.solves-grid {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-2);
}

.solve-chip {
    padding: var(--oj-spacing-1) var(--oj-spacing-2);
    font-family: var(--oj-font-family-mono, monospace);
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-secondary);
    cursor: pointer;
    background: var(--oj-surface-hover);
    border-radius: var(--oj-radius-sm);
    transition: all 0.2s ease;
}

.solve-chip:hover {
    color: #fff;
    background: var(--oj-color-primary);
    transform: translateY(-1px);
}

/* ── 最近提交 ── */
.submissions-table-wrap {
    overflow-x: auto;
}

.submissions-table {
    width: 100%;
    font-size: var(--oj-font-size-sm);
    text-align: left;
    border-collapse: collapse;
}

.submissions-table thead tr {
    background: var(--oj-table-header-bg);
}

.submissions-table th {
    padding: var(--oj-spacing-2) var(--oj-spacing-3);
    font-size: var(--oj-font-size-xs);
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color-muted);
    text-transform: uppercase;
    letter-spacing: 0.04em;
}

.submissions-table td {
    padding: var(--oj-spacing-3);
    color: var(--oj-text-color-secondary);
    border-top: 1px solid var(--oj-card-border);
}

.submissions-table tbody tr {
    transition: background-color 0.15s ease;
}

.submissions-table tbody tr:hover {
    background: var(--oj-table-row-hover-bg);
}

.problem-link {
    color: var(--oj-text-color-secondary);
    cursor: pointer;
    transition: color 0.15s ease;
}

.problem-link:hover {
    color: var(--oj-color-primary);
}

.col-time {
    font-size: var(--oj-font-size-xs);
    white-space: nowrap;
}

.col-lang {
    white-space: nowrap;
}

/* ── 响应式 ── */
@media (width < 768px) {
    .profile-layout,
    .skeleton-layout {
        grid-template-columns: 1fr;
    }
}

@media (width >= 640px) {
    .user-profile-page {
        padding: var(--oj-spacing-8);
    }
}
</style>
