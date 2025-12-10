<script setup lang="ts">
defineOptions({ name: "UserLogin" });
import { reactive, ref } from "vue";
import { userLogin } from "@/api/user";
import type { Api } from "@/types/api/api";
import { useUserStore } from "@/stores/modules/user";
import { useRoute, useRouter } from "vue-router";
import type { FormInstance, FormRules } from "element-plus";
import { Cpu, DataAnalysis, Connection, Lock, Right, Trophy, View, Hide } from "@element-plus/icons-vue";

const route = useRoute();
const router = useRouter();
const userStore = useUserStore();

/** 表单引用 */
const loginFormRef = ref<FormInstance>();

/** 登录表单数据 */
const loginForm = reactive<Api.User.UserLoginParams>({
    Account: "",
    PassWord: "",
});

/** 表单验证规则 */
const loginRules = reactive<FormRules<Api.User.UserLoginParams>>({
    Account: [
        { required: true, message: "请输入账号", trigger: "blur" },
        { min: 3, max: 20, message: "账号长度为 3-20 个字符", trigger: "blur" },
    ],
    PassWord: [
        { required: true, message: "请输入密码", trigger: "blur" },
        { min: 6, max: 32, message: "密码长度为 6-32 个字符", trigger: "blur" },
    ],
});

/** 登录中状态 */
const loading = ref(false);

/** 显示密码 */
const showPassword = ref(false);

/** 记住我 */
const rememberMe = ref(false);

/**
 * 处理登录
 */
const handleLogin = async (formEl: FormInstance | undefined) => {
    if (!formEl) return;

    await formEl.validate(async (valid) => {
        if (!valid) return;

        loading.value = true;
        try {
            const response = await userLogin(loginForm);
            if (response.data.success && response.data.data) {
                const userInfoData = response.data.data;
                userStore.setUserInfo(userInfoData);
                userStore.setIsLoggedIn(true);
                userStore.setAccessToken(userInfoData.Token);

                // 跳转到重定向页面或首页
                const redirect = route.query.redirect as string;
                await router.push(redirect || "/");
            }
        } catch (error) {
            console.error("Login failed:", error);
        } finally {
            loading.value = false;
        }
    });
};

/** 跳转到注册页面 */
const goToRegister = () => {
    router.push("/auth/register");
};
</script>

<template>
    <div class="login-container">
        <!-- 左侧信息区域 -->
        <div class="info-section">
            <div class="brand-section">
                <div class="brand-logo">
                    <el-icon :size="64">
                        <component :is="Trophy" />
                    </el-icon>
                </div>
                <h1 class="brand-title">LYOJ 在线评测平台</h1>
                <p class="brand-subtitle">LingYun Online Judge Platform</p>
            </div>

            <div class="features-list">
                <div class="feature-item">
                    <el-icon :size="24"><component :is="Cpu" /></el-icon>
                    <span>智能代码评测</span>
                </div>
                <div class="feature-item">
                    <el-icon :size="24"><component :is="DataAnalysis" /></el-icon>
                    <span>实时数据分析</span>
                </div>
                <div class="feature-item">
                    <el-icon :size="24"><component :is="Connection" /></el-icon>
                    <span>云端同步协作</span>
                </div>
                <div class="feature-item">
                    <el-icon :size="24"><component :is="Lock" /></el-icon>
                    <span>安全可靠保障</span>
                </div>
            </div>
        </div>

        <!-- 右侧登录表单 -->
        <div class="form-section">
            <div class="form-container">
                <div class="form-header">
                    <h2>欢迎回来</h2>
                    <p>登录您的账户以继续使用</p>
                </div>

                <el-form ref="loginFormRef" :model="loginForm" :rules="loginRules" class="futuristic-form" size="large">
                    <!-- 表单标题装饰 -->
                    <div class="form-title-section">
                        <div class="title-line"></div>
                        <span class="title-text">用户认证</span>
                        <div class="title-line"></div>
                    </div>

                    <!-- 输入字段容器 -->
                    <div class="input-fields-container">
                        <el-form-item prop="Account" class="input-field-wrapper">
                            <el-input v-model="loginForm.Account" placeholder="用户名" clearable class="cyber-input">
                                <template #prefix>
                                    <div class="input-icon-wrapper">
                                        <el-icon><i-ep-user /></el-icon>
                                    </div>
                                </template>
                            </el-input>
                        </el-form-item>

                        <el-form-item prop="PassWord" class="input-field-wrapper">
                            <el-input
                                v-model="loginForm.PassWord"
                                :type="showPassword ? 'text' : 'password'"
                                placeholder="密码"
                                class="cyber-input"
                                @keyup.enter="handleLogin(loginFormRef)"
                            >
                                <template #prefix>
                                    <div class="input-icon-wrapper">
                                        <el-icon><i-ep-lock /></el-icon>
                                    </div>
                                </template>
                                <template #suffix>
                                    <el-button
                                        :icon="showPassword ? Hide : View"
                                        class="visibility-toggle"
                                        text
                                        @click="showPassword = !showPassword"
                                    />
                                </template>
                            </el-input>
                        </el-form-item>
                    </div>

                    <!-- 表单选项 -->
                    <div class="form-options-futuristic">
                        <el-checkbox v-model="rememberMe" label="记住我" size="small" class="cyber-checkbox" />
                        <el-button text class="forgot-password-link" size="small">忘记密码？</el-button>
                    </div>

                    <!-- 登录按钮 -->
                    <div class="submit-button-container">
                        <el-button
                            type="primary"
                            :loading="loading"
                            class="cyber-submit-btn"
                            size="large"
                            @click="handleLogin(loginFormRef)"
                        >
                            <div class="btn-glow"></div>
                            <div class="btn-text">
                                <span>{{ loading ? "登录中..." : "启动系统" }}</span>
                                <el-icon class="btn-icon"><component :is="Right" /></el-icon>
                            </div>
                            <div class="btn-circuit-pattern">
                                <div class="circuit-line circuit-1"></div>
                                <div class="circuit-line circuit-2"></div>
                                <div class="circuit-line circuit-3"></div>
                            </div>
                        </el-button>
                    </div>

                    <!-- 分割线 -->
                    <div class="form-divider">
                        <div class="divider-line"></div>
                        <div class="divider-text">
                            <span>或</span>
                            <div class="text-glow"></div>
                        </div>
                        <div class="divider-line"></div>
                    </div>

                    <!-- 注册链接 -->
                    <div class="register-link-container">
                        <div class="link-backdrop"></div>
                        <span class="link-text">还没有账户？</span>
                        <el-button text class="register-link-btn" @click="goToRegister">
                            立即注册
                            <div class="link-glow"></div>
                        </el-button>
                    </div>
                </el-form>
            </div>
        </div>
    </div>
</template>

<style scoped>
.login-container {
    display: grid;
    grid-template-columns: 1fr;
    gap: 2rem;
    width: 100%;
    max-width: 1200px;
    margin: 0 auto;
}

/* 左侧信息区域 */
.info-section {
    display: none;
    flex-direction: column;
    justify-content: center;
    gap: var(--oj-section-gap-md);
    padding: var(--oj-padding-xl);
    background: linear-gradient(135deg, var(--oj-color-primary) 0%, var(--oj-color-secondary) 100%);
    border-radius: var(--oj-radius-xl);
    color: white;
}

.brand-section {
    text-align: center;
}

.brand-logo {
    display: flex;
    justify-content: center;
    margin-bottom: 1.5rem;
    animation: float 3s ease-in-out infinite;
}

@keyframes float {
    0%,
    100% {
        transform: translateY(0);
    }
    50% {
        transform: translateY(-10px);
    }
}

.brand-title {
    margin: 0 0 0.5rem 0;
    font-size: 2rem;
    font-weight: 700;
    line-height: 1.2;
}

.brand-subtitle {
    margin: 0;
    font-size: 0.875rem;
    opacity: 0.9;
    letter-spacing: 0.05em;
}

.features-list {
    display: flex;
    flex-direction: column;
    gap: 1.5rem;
}

.feature-item {
    display: flex;
    align-items: center;
    gap: 1rem;
    padding: 1rem;
    background: rgba(255, 255, 255, 0.1);
    border-radius: var(--oj-radius-lg);
    backdrop-filter: blur(10px);
    transition: all 0.3s;
}

.feature-item:hover {
    background: rgba(255, 255, 255, 0.15);
    transform: translateX(5px);
}

.feature-item span {
    font-size: 1rem;
}

/* 右侧表单区域 */
.form-section {
    display: flex;
    align-items: center;
    justify-content: center;
    padding: 1rem;
}

.form-container {
    width: 100%;
    max-width: 420px;
}

.form-header {
    margin-bottom: 2rem;
    text-align: center;
}

.form-header h2 {
    margin: 0 0 0.5rem 0;
    font-size: 1.875rem;
    font-weight: 700;
    color: var(--oj-text-primary);
}

.form-header p {
    margin: 0;
    font-size: 0.875rem;
    color: var(--oj-text-secondary);
}

/* 表单标题装饰 */
.form-title-section {
    display: flex;
    align-items: center;
    gap: 1rem;
    margin-bottom: 2rem;
}

.title-line {
    flex: 1;
    height: 1px;
    background: linear-gradient(90deg, transparent, var(--oj-color-primary), transparent);
}

.title-text {
    font-size: 0.75rem;
    font-weight: 600;
    color: var(--oj-color-primary);
    text-transform: uppercase;
    letter-spacing: 0.1em;
}

/* 输入字段 */
.input-fields-container {
    display: flex;
    flex-direction: column;
    gap: 1.5rem;
    margin-bottom: 1.5rem;
}

.input-field-wrapper {
    position: relative;
    margin-bottom: 0 !important;
}

.input-icon-wrapper {
    position: relative;
    display: flex;
    align-items: center;
    padding: var(--oj-spacing-1);
}

/* 表单选项 */
.form-options-futuristic {
    display: flex;
    align-items: center;
    justify-content: space-between;
    margin-bottom: 1.5rem;
}

.forgot-password-link {
    color: var(--oj-color-primary) !important;
    font-size: 0.875rem;
}

/* 提交按钮 */
.submit-button-container {
    margin-bottom: 1.5rem;
}

.cyber-submit-btn {
    position: relative;
    width: 100%;
    height: 48px;
    overflow: hidden;
    font-size: 1rem;
    font-weight: 600;
    background: var(--oj-color-primary) !important;
    border: none !important;
    transition: all 0.3s;
}

.cyber-submit-btn:hover {
    transform: translateY(-2px);
    box-shadow: 0 8px 20px rgba(var(--oj-color-primary-rgb), 0.4);
}

.btn-glow {
    position: absolute;
    inset: 0;
    background: linear-gradient(45deg, transparent, rgba(255, 255, 255, 0.1), transparent);
    transform: translateX(-100%);
    transition: transform 0.6s;
}

.cyber-submit-btn:hover .btn-glow {
    transform: translateX(100%);
}

.btn-text {
    position: relative;
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 0.5rem;
    z-index: 1;
}

.btn-icon {
    transition: transform 0.3s;
}

.cyber-submit-btn:hover .btn-icon {
    transform: translateX(5px);
}

.btn-circuit-pattern {
    position: absolute;
    inset: 0;
    opacity: 0.1;
    pointer-events: none;
}

.circuit-line {
    position: absolute;
    background: white;
}

.circuit-1 {
    top: 10%;
    left: 0;
    width: 100%;
    height: 1px;
}

.circuit-2 {
    top: 50%;
    left: 0;
    width: 100%;
    height: 1px;
}

.circuit-3 {
    bottom: 10%;
    left: 0;
    width: 100%;
    height: 1px;
}

/* 分割线 */
.form-divider {
    position: relative;
    display: flex;
    align-items: center;
    gap: 1rem;
    margin-bottom: 1.5rem;
}

.divider-line {
    flex: 1;
    height: 1px;
    background: var(--oj-border-color);
}

.divider-text {
    position: relative;
    font-size: 0.875rem;
    color: var(--oj-text-secondary);
}

.text-glow {
    position: absolute;
    inset: -8px;
    background: var(--oj-color-primary);
    border-radius: 50%;
    opacity: 0;
    filter: blur(10px);
    animation: glow 2s ease-in-out infinite;
}

@keyframes glow {
    0%,
    100% {
        opacity: 0;
    }
    50% {
        opacity: 0.2;
    }
}

/* 注册链接 */
.register-link-container {
    position: relative;
    display: flex;
    align-items: center;
    justify-content: center;
    gap: var(--oj-gap-xs);
    padding: var(--oj-padding-sm);
    border-radius: var(--oj-radius-lg);
    background: var(--oj-glass-bg);
    border: 1px solid var(--oj-glass-border);
}

.link-backdrop {
    position: absolute;
    inset: 0;
    background: linear-gradient(135deg, var(--oj-color-primary), var(--oj-color-secondary));
    opacity: 0;
    border-radius: var(--oj-radius-lg);
    transition: opacity 0.3s;
    pointer-events: none;
}

.register-link-container:hover .link-backdrop {
    opacity: 0.05;
}

.link-text {
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-secondary);
}

.register-link-btn {
    position: relative;
    color: var(--oj-color-primary) !important;
    font-weight: 600;
}

.link-glow {
    position: absolute;
    inset: -4px;
    background: var(--oj-color-primary);
    border-radius: var(--oj-radius-md);
    opacity: 0;
    filter: blur(8px);
    transition: opacity 0.3s;
}

.register-link-btn:hover .link-glow {
    opacity: 0.2;
}

/* 桌面端布局 */
@media (min-width: 768px) {
    .login-container {
        grid-template-columns: 5fr 7fr;
        gap: 0;
    }

    .info-section {
        display: flex;
    }

    .form-section {
        padding: var(--oj-spacing-0) var(--oj-padding-xl);
    }
}

/* 移动端优化 */
@media (max-width: 767px) {
    .form-container {
        max-width: 100%;
    }

    .form-header h2 {
        font-size: 1.5rem;
    }

    .brand-title {
        font-size: 1.5rem;
    }
}

/* Element Plus 样式覆盖 */
:deep(.el-input__wrapper) {
    background-color: var(--oj-glass-bg);
    border: 1px solid var(--oj-glass-border);
    box-shadow: none !important;
    transition: all 0.3s;
}

:deep(.el-input__wrapper.is-focus) {
    border-color: var(--oj-color-primary);
    box-shadow: 0 0 0 1px var(--oj-color-primary) !important;
}

:deep(.el-input__inner) {
    color: var(--oj-text-primary);
}

:deep(.el-input__inner::placeholder) {
    color: var(--oj-text-secondary);
}

:deep(.el-checkbox__label) {
    font-size: 0.875rem;
    color: var(--oj-text-secondary);
}
</style>
