<script setup lang="ts">
defineOptions({ name: "UserRegister" });
import { reactive, ref } from "vue";
import { userRegister } from "@/api/user";
import type { Api } from "@/types/api/api";
import { useRouter } from "vue-router";
import type { FormInstance, FormRules } from "element-plus";
import { UserFilled, TrophyBase, Promotion, Trophy, Document, Right, Hide, View } from "@element-plus/icons-vue";
import { ElMessage } from "element-plus";

const router = useRouter();

/** 表单引用 */
const registerFormRef = ref<FormInstance>();

/** 注册表单数据（包含确认密码） */
const registerForm = reactive({
    NickName: "",
    Account: "",
    PassWord: "",
    confirmPassword: "",
    PersonalProfile: "",
    School: "",
    Major: "",
});

/** 显示密码 */
const showPassword = ref(false);

/** 显示确认密码 */
const showConfirmPassword = ref(false);

/** 表单验证规则 */
const registerRules = reactive<FormRules>({
    NickName: [
        { required: true, message: "请输入昵称", trigger: "blur" },
        { min: 2, max: 20, message: "昵称长度为 2-20 个字符", trigger: "blur" },
    ],
    Account: [
        { required: true, message: "请输入账号", trigger: "blur" },
        { min: 3, max: 20, message: "账号长度为 3-20 个字符", trigger: "blur" },
        { pattern: /^[a-zA-Z0-9_]+$/, message: "账号只能包含字母、数字和下划线", trigger: "blur" },
    ],
    PassWord: [
        { required: true, message: "请输入密码", trigger: "blur" },
        { min: 6, max: 32, message: "密码长度为 6-32 个字符", trigger: "blur" },
    ],
    confirmPassword: [
        { required: true, message: "请确认密码", trigger: "blur" },
        {
            validator: (rule, value, callback) => {
                if (!value) {
                    callback(new Error("请确认密码"));
                } else if (value !== registerForm.PassWord) {
                    callback(new Error("两次输入的密码不一致"));
                } else {
                    callback();
                }
            },
            trigger: "blur",
        },
    ],
    School: [
        { required: true, message: "请输入学校", trigger: "blur" },
        { min: 2, max: 50, message: "学校名称长度为 2-50 个字符", trigger: "blur" },
    ],
    Major: [
        { required: true, message: "请输入专业", trigger: "blur" },
        { min: 2, max: 50, message: "专业名称长度为 2-50 个字符", trigger: "blur" },
    ],
});

/** 注册中状态 */
const loading = ref(false);

/**
 * 处理注册
 */
const handleRegister = async (formEl: FormInstance | undefined) => {
    if (!formEl) return;

    await formEl.validate(async (valid) => {
        if (!valid) return;

        loading.value = true;
        try {
            // 提取注册所需的字段，排除 confirmPassword
            // eslint-disable-next-line @typescript-eslint/no-unused-vars
            const { confirmPassword, ...registerData } = registerForm;
            const response = await userRegister(registerData as Api.User.UserRegisterParams);
            if (response.data.success && response.data.data) {
                ElMessage.success("注册成功！正在跳转到登录页面...");
                await router.push("/auth/login");
            }
        } catch (error) {
            console.error("Register failed:", error);
        } finally {
            loading.value = false;
        }
    });
};

/** 跳转到登录页面 */
const goToLogin = () => {
    router.push("/auth/login");
};
</script>

<template>
    <div class="register-container">
        <!-- 左侧信息区域 -->
        <div class="info-section">
            <div class="brand-section">
                <div class="brand-logo">
                    <el-icon :size="64">
                        <component :is="Trophy" />
                    </el-icon>
                </div>
                <h1 class="brand-title">加入 LYOJ</h1>
                <p class="brand-subtitle">开启你的编程之旅</p>
            </div>

            <div class="features-list">
                <div class="feature-item">
                    <el-icon :size="24"><component :is="Document" /></el-icon>
                    <span>海量算法题库</span>
                </div>
                <div class="feature-item">
                    <el-icon :size="24"><component :is="Promotion" /></el-icon>
                    <span>智能学习路径</span>
                </div>
                <div class="feature-item">
                    <el-icon :size="24"><component :is="UserFilled" /></el-icon>
                    <span>社区交流互动</span>
                </div>
                <div class="feature-item">
                    <el-icon :size="24"><component :is="TrophyBase" /></el-icon>
                    <span>竞赛排名系统</span>
                </div>
            </div>
        </div>

        <!-- 右侧注册表单 -->
        <div class="form-section">
            <div class="form-container">
                <div class="form-header">
                    <h2>创建账户</h2>
                    <p>填写信息以开始使用 LYOJ</p>
                </div>

                <el-form
                    ref="registerFormRef"
                    :model="registerForm"
                    :rules="registerRules"
                    class="futuristic-form"
                    size="large"
                >
                    <!-- 表单标题装饰 -->
                    <div class="form-title-section">
                        <div class="title-line"></div>
                        <span class="title-text">注册信息</span>
                        <div class="title-line"></div>
                    </div>

                    <!-- 输入字段容器 -->
                    <div class="input-fields-container">
                        <div class="input-row">
                            <el-form-item prop="NickName" class="input-field-wrapper">
                                <el-input
                                    v-model="registerForm.NickName"
                                    placeholder="用户昵称"
                                    clearable
                                    class="cyber-input"
                                >
                                    <template #prefix>
                                        <div class="input-icon-wrapper">
                                            <el-icon><i-ep-user /></el-icon>
                                        </div>
                                    </template>
                                </el-input>
                            </el-form-item>

                            <el-form-item prop="Account" class="input-field-wrapper">
                                <el-input
                                    v-model="registerForm.Account"
                                    placeholder="登录账号"
                                    clearable
                                    class="cyber-input"
                                >
                                    <template #prefix>
                                        <div class="input-icon-wrapper">
                                            <el-icon><i-ep-user /></el-icon>
                                        </div>
                                    </template>
                                </el-input>
                            </el-form-item>
                        </div>

                        <div class="input-row">
                            <el-form-item prop="PassWord" class="input-field-wrapper">
                                <el-input
                                    v-model="registerForm.PassWord"
                                    :type="showPassword ? 'text' : 'password'"
                                    placeholder="密码"
                                    class="cyber-input"
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

                            <el-form-item prop="confirmPassword" class="input-field-wrapper">
                                <el-input
                                    v-model="registerForm.confirmPassword"
                                    :type="showConfirmPassword ? 'text' : 'password'"
                                    placeholder="确认密码"
                                    class="cyber-input"
                                >
                                    <template #prefix>
                                        <div class="input-icon-wrapper">
                                            <el-icon><i-ep-lock /></el-icon>
                                        </div>
                                    </template>
                                    <template #suffix>
                                        <el-button
                                            :icon="showConfirmPassword ? Hide : View"
                                            class="visibility-toggle"
                                            text
                                            @click="showConfirmPassword = !showConfirmPassword"
                                        />
                                    </template>
                                </el-input>
                            </el-form-item>
                        </div>

                        <div class="input-row">
                            <el-form-item prop="School" class="input-field-wrapper">
                                <el-input
                                    v-model="registerForm.School"
                                    placeholder="学校"
                                    clearable
                                    class="cyber-input"
                                >
                                    <template #prefix>
                                        <div class="input-icon-wrapper">
                                            <el-icon><i-ep-school /></el-icon>
                                        </div>
                                    </template>
                                </el-input>
                            </el-form-item>

                            <el-form-item prop="Major" class="input-field-wrapper">
                                <el-input v-model="registerForm.Major" placeholder="专业" clearable class="cyber-input">
                                    <template #prefix>
                                        <div class="input-icon-wrapper">
                                            <el-icon><i-ep-reading /></el-icon>
                                        </div>
                                    </template>
                                </el-input>
                            </el-form-item>
                        </div>

                        <el-form-item prop="PersonalProfile" class="input-field-wrapper">
                            <el-input
                                v-model="registerForm.PersonalProfile"
                                type="textarea"
                                placeholder="个人简介（选填）"
                                :rows="2"
                                maxlength="200"
                                show-word-limit
                                class="cyber-textarea"
                            />
                        </el-form-item>
                    </div>

                    <!-- 注册按钮 -->
                    <div class="submit-button-container">
                        <el-button
                            type="primary"
                            :loading="loading"
                            class="cyber-submit-btn"
                            size="large"
                            @click="handleRegister(registerFormRef)"
                        >
                            <div class="btn-glow"></div>
                            <div class="btn-text">
                                <span>{{ loading ? "注册中..." : "立即注册" }}</span>
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

                    <!-- 登录链接 -->
                    <div class="login-link-container">
                        <div class="link-backdrop"></div>
                        <span class="link-text">已有账户？</span>
                        <el-button text class="register-link-btn" @click="goToLogin">
                            立即登录
                            <div class="link-glow"></div>
                        </el-button>
                    </div>
                </el-form>
            </div>
        </div>
    </div>
</template>

<style scoped>
.register-container {
    display: grid;
    grid-template-columns: 1fr;
    gap: var(--oj-gap-xl);
    width: 100%;
    max-width: 1200px;
    margin: 0 auto;
}

/* 左侧信息区域 */
.info-section {
    display: none;
    flex-direction: column;
    gap: var(--oj-section-gap-md);
    justify-content: center;
    padding: var(--oj-padding-xl);
    color: white;
    background: linear-gradient(135deg, var(--oj-color-primary) 0%, var(--oj-color-secondary) 100%);
    border-radius: var(--oj-radius-xl);
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
    margin: 0 0 0.5rem;
    font-size: 2rem;
    font-weight: 700;
    line-height: 1.2;
}

.brand-subtitle {
    margin: 0;
    font-size: var(--oj-font-size-sm);
    letter-spacing: 0.05em;
    opacity: 0.9;
}

.features-list {
    display: flex;
    flex-direction: column;
    gap: 1.5rem;
}

.feature-item {
    display: flex;
    gap: var(--oj-gap-md);
    align-items: center;
    padding: var(--oj-padding-md);
    background: rgb(255 255 255 / 10%);
    border-radius: var(--oj-radius-lg);
    backdrop-filter: blur(10px);
    transition: all 0.3s;
}

.feature-item:hover {
    background: rgb(255 255 255 / 15%);
    transform: translateX(5px);
}

.feature-item span {
    font-size: var(--oj-font-size-base);
}

/* 右侧表单区域 */
.form-section {
    display: flex;
    align-items: center;
    justify-content: center;
    padding: var(--oj-padding-md);
}

.form-container {
    width: 100%;
    max-width: 520px;
}

.form-header {
    margin-bottom: 2rem;
    text-align: center;
}

.form-header h2 {
    margin: 0 0 0.5rem;
    font-size: var(--oj-font-size-3xl);
    font-weight: 700;
    color: var(--oj-text-color);
}

.form-header p {
    margin: 0;
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-secondary);
}

/* 表单标题装饰 */
.form-title-section {
    display: flex;
    gap: var(--oj-gap-md);
    align-items: center;
    margin-bottom: 2rem;
}

.title-line {
    flex: 1;
    height: 1px;
    background: linear-gradient(90deg, transparent, var(--oj-color-primary), transparent);
}

.title-text {
    font-size: var(--oj-font-size-xs);
    font-weight: 600;
    color: var(--oj-color-primary);
    text-transform: uppercase;
    letter-spacing: 0.1em;
}

/* 输入字段 */
.input-fields-container {
    display: flex;
    flex-direction: column;
    gap: var(--oj-gap-lg);
    margin-bottom: 1.5rem;
}

.input-field-wrapper {
    position: relative;
    margin-bottom: 0 !important;
}

.input-row {
    display: grid;
    grid-template-columns: 1fr;
    gap: var(--oj-gap-md);
}

@media (width >= 640px) {
    .input-row {
        grid-template-columns: 1fr 1fr;
    }
}

.input-icon-wrapper {
    position: relative;
    display: flex;
    align-items: center;
    padding: var(--oj-spacing-1);
}

@keyframes pulse {
    0%,
    100% {
        opacity: 0;
        transform: scale(1);
    }

    50% {
        opacity: 0.3;
        transform: scale(1.2);
    }
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
    font-size: var(--oj-font-size-base);
    font-weight: 600;
    background: var(--oj-color-primary) !important;
    border: none !important;
    transition: all 0.3s;
}

.cyber-submit-btn:hover {
    box-shadow: 0 8px 20px rgb(var(--oj-color-primary-rgb), 0.4);
    transform: translateY(-2px);
}

.btn-glow {
    position: absolute;
    inset: 0;
    background: linear-gradient(45deg, transparent, rgb(255 255 255 / 10%), transparent);
    transform: translateX(-100%);
    transition: transform 0.6s;
}

.cyber-submit-btn:hover .btn-glow {
    transform: translateX(100%);
}

.btn-text {
    position: relative;
    z-index: 1;
    display: flex;
    gap: var(--oj-gap-xs);
    align-items: center;
    justify-content: center;
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
    pointer-events: none;
    opacity: 0.1;
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
    gap: var(--oj-gap-md);
    align-items: center;
    margin-bottom: 1.5rem;
}

.divider-line {
    flex: 1;
    height: 1px;
    background: var(--oj-border-color);
}

.divider-text {
    position: relative;
    font-size: var(--oj-font-size-sm);
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

/* 登录链接 */
.login-link-container {
    position: relative;
    display: flex;
    gap: var(--oj-gap-xs);
    align-items: center;
    justify-content: center;
    padding: var(--oj-padding-sm);
    background: var(--oj-glass-bg);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-lg);
}

.link-backdrop {
    position: absolute;
    inset: 0;
    pointer-events: none;
    background: linear-gradient(135deg, var(--oj-color-primary), var(--oj-color-secondary));
    border-radius: var(--oj-radius-lg);
    opacity: 0;
    transition: opacity 0.3s;
}

.login-link-container:hover .link-backdrop {
    opacity: 0.05;
}

.link-text {
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-secondary);
}

.register-link-btn {
    position: relative;
    font-weight: 600;
    color: var(--oj-color-primary) !important;
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
@media (width >= 768px) {
    .register-container {
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
@media (width <= 767px) {
    .form-container {
        max-width: 100%;
    }

    .form-header h2 {
        font-size: var(--oj-font-size-2xl);
    }

    .brand-title {
        font-size: var(--oj-font-size-2xl);
    }
}

/* Element Plus 样式覆盖 */
:deep(.el-input__wrapper),
:deep(.el-textarea__inner) {
    background-color: var(--oj-glass-bg);
    border: 1px solid var(--oj-glass-border);
    box-shadow: none !important;
    transition: all 0.3s;
}

:deep(.el-input__wrapper.is-focus),
:deep(.el-textarea__inner:focus) {
    border-color: var(--oj-color-primary);
    box-shadow: 0 0 0 1px var(--oj-color-primary) !important;
}

:deep(.el-input__inner),
:deep(.el-textarea__inner) {
    color: var(--oj-text-color);
}

:deep(.el-input__inner::placeholder),
:deep(.el-textarea__inner::placeholder) {
    color: var(--oj-text-secondary);
}

:deep(.el-form-item__error) {
    font-size: var(--oj-font-size-xs);
}
</style>
