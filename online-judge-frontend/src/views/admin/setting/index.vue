<script setup lang="ts">
/**
 * 管理后台 - 系统设置页面
 *
 * @description 系统信息展示，未来可扩展配置项
 */
defineOptions({ name: "AdminSetting" });

const apiStatus = ref<"checking" | "online" | "offline">("checking");

const checkApiStatus = async () => {
    apiStatus.value = "checking";
    try {
        const res = await fetch("/api/user/rank?PageNum=1&PageSize=1");
        apiStatus.value = res.ok ? "online" : "offline";
    } catch {
        apiStatus.value = "offline";
    }
};

const systemInfo = [
    { label: "平台名称", value: "凌云在线编程评测平台 (LYOJ)" },
    { label: "前端版本", value: "v2.0.0" },
    { label: "前端框架", value: "Vue 3.5 + TypeScript 5.9" },
    { label: "UI 组件库", value: "Element Plus 2.11" },
    { label: "图表库", value: "ECharts 6.0" },
    { label: "代码编辑器", value: "Monaco Editor 0.55" },
    { label: "后端语言", value: "C++17 (cpp-httplib)" },
    { label: "数据库", value: "MongoDB (mongo-cxx-driver 4.0)" },
    { label: "缓存", value: "Redis (redis-plus-plus 1.3)" },
    { label: "判题沙箱", value: "libJudger (seccomp-based)" },
];

onMounted(() => {
    checkApiStatus();
});
</script>

<template>
    <div class="admin-setting">
        <!-- 页面标题 -->
        <div class="page-header">
            <h2 class="page-title">系统设置</h2>
            <p class="page-desc">查看系统配置信息与 API 状态</p>
        </div>

        <!-- API 状态 -->
        <div class="status-section">
            <h3 class="section-title">
                <el-icon><i-ep-monitor /></el-icon>
                <span>API 状态</span>
            </h3>
            <div class="status-card" :class="[`status-${apiStatus}`]">
                <div class="status-indicator" />
                <div class="status-info">
                    <span class="status-label">后端 API</span>
                    <span class="status-text">
                        {{ apiStatus === "checking" ? "检测中..." : apiStatus === "online" ? "运行正常" : "无法连接" }}
                    </span>
                </div>
                <el-button
                    size="small"
                    :loading="apiStatus === 'checking'"
                    @click="checkApiStatus"
                >
                    <el-icon><i-ep-refresh /></el-icon>
                    <span>刷新</span>
                </el-button>
            </div>
        </div>

        <!-- 系统信息 -->
        <div class="info-section">
            <h3 class="section-title">
                <el-icon><i-ep-info-filled /></el-icon>
                <span>系统信息</span>
            </h3>
            <div class="info-table">
                <div v-for="item in systemInfo" :key="item.label" class="info-row">
                    <span class="info-label">{{ item.label }}</span>
                    <span class="info-value">{{ item.value }}</span>
                </div>
            </div>
        </div>

        <!-- 提示 -->
        <div class="setting-notice">
            <el-icon :size="18"><i-ep-info-filled /></el-icon>
            <span>更多系统设置功能将在后续版本中推出</span>
        </div>
    </div>
</template>

<style scoped>
.admin-setting {
    padding: var(--oj-spacing-4);
}

/* ── 页面标题 ── */
.page-header {
    margin-bottom: var(--oj-spacing-6);
}

.page-title {
    margin: 0 0 var(--oj-spacing-1);
    font-size: var(--oj-font-size-xl);
    font-weight: var(--oj-font-weight-bold);
    color: var(--oj-text-color);
}

.page-desc {
    margin: 0;
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-color-secondary);
}

/* ── 标题 ── */
.section-title {
    display: flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    margin: 0 0 var(--oj-spacing-4);
    font-size: var(--oj-font-size-base);
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
}

/* ── API 状态 ── */
.status-section {
    margin-bottom: var(--oj-spacing-6);
}

.status-card {
    display: flex;
    gap: var(--oj-spacing-4);
    align-items: center;
    padding: var(--oj-spacing-4) var(--oj-spacing-5);
    background: var(--oj-card-bg);
    border: 1px solid var(--oj-card-border);
    border-radius: var(--oj-radius-xl);
}

.status-indicator {
    width: 12px;
    height: 12px;
    flex-shrink: 0;
    border-radius: 50%;
    transition: all 0.3s ease;
}

.status-checking .status-indicator {
    background: var(--oj-color-warning);
    box-shadow: 0 0 8px var(--oj-color-warning);
    animation: pulse 1.5s ease-in-out infinite;
}

.status-online .status-indicator {
    background: var(--oj-color-success);
    box-shadow: 0 0 8px var(--oj-color-success);
}

.status-offline .status-indicator {
    background: var(--oj-color-danger);
    box-shadow: 0 0 8px var(--oj-color-danger);
}

@keyframes pulse {
    0%,
    100% {
        opacity: 1;
    }

    50% {
        opacity: 0.4;
    }
}

.status-info {
    display: flex;
    flex: 1;
    flex-direction: column;
    gap: 2px;
}

.status-label {
    font-size: var(--oj-font-size-sm);
    font-weight: var(--oj-font-weight-medium);
    color: var(--oj-text-color);
}

.status-text {
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-muted);
}

/* ── 系统信息表格 ── */
.info-section {
    margin-bottom: var(--oj-spacing-6);
}

.info-table {
    overflow: hidden;
    background: var(--oj-card-bg);
    border: 1px solid var(--oj-card-border);
    border-radius: var(--oj-radius-xl);
}

.info-row {
    display: flex;
    gap: var(--oj-spacing-4);
    padding: var(--oj-spacing-3) var(--oj-spacing-5);
    border-bottom: 1px solid var(--oj-card-border);
}

.info-row:last-child {
    border-bottom: none;
}

.info-row:hover {
    background: rgb(var(--oj-color-primary-rgb) / 3%);
}

.info-label {
    flex-shrink: 0;
    width: 120px;
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-color-muted);
}

.info-value {
    font-size: var(--oj-font-size-sm);
    font-weight: var(--oj-font-weight-medium);
    color: var(--oj-text-color);
}

/* ── 提示 ── */
.setting-notice {
    display: flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    padding: var(--oj-spacing-3) var(--oj-spacing-4);
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-color-muted);
    background: rgb(var(--oj-color-primary-rgb) / 5%);
    border: 1px solid rgb(var(--oj-color-primary-rgb) / 15%);
    border-radius: var(--oj-radius-lg);
}
</style>
