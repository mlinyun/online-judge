<script setup lang="ts">
/**
 * 用户设置页
 *
 * @description 用户个人信息编辑 + 密码修改
 */
import { ElMessage } from "element-plus";
import type { FormInstance, FormRules } from "element-plus";
import { selectUserUpdateInfo, updateUserInfo, updateUserPassword } from "@/api/user";
import { useUserStore } from "@/stores/modules/user";
import type { Api } from "@/types/api/api";

defineOptions({ name: "UserSetting" });

const userStore = useUserStore();
const userInfo = computed(() => userStore.getUserInfo);

// ── Tab 控制 ──
const activeTab = ref<"profile" | "password">("profile");

// ── 个人信息表单 ──
const profileFormRef = ref<FormInstance>();
const profileLoading = ref(false);
const profileSaving = ref(false);

const AVATAR_OPTIONS = [
    "/api/image/1",
    "/api/image/2",
    "/api/image/3",
    "/api/image/4",
    "/api/image/5",
];

const profileForm = reactive<{
    Avatar: string;
    NickName: string;
    PersonalProfile: string;
    School: string;
    Major: string;
}>({
    Avatar: "",
    NickName: "",
    PersonalProfile: "",
    School: "",
    Major: "",
});

const profileRules = reactive<FormRules>({
    School: [
        { required: true, message: "请输入学校", trigger: "blur" },
        { min: 2, max: 50, message: "学校名称 2-50 个字符", trigger: "blur" },
    ],
    Major: [
        { required: true, message: "请输入专业", trigger: "blur" },
        { min: 2, max: 50, message: "专业名称 2-50 个字符", trigger: "blur" },
    ],
});

const fetchProfileInfo = async () => {
    profileLoading.value = true;
    try {
        const userId = userInfo.value._id;
        if (!userId || userId === "0") return;
        const res = await selectUserUpdateInfo(userId);
        if (res.data.success && res.data.data) {
            const d = res.data.data;
            profileForm.Avatar = d.Avatar ?? AVATAR_OPTIONS[0];
            profileForm.NickName = d.NickName || "";
            profileForm.PersonalProfile = d.PersonalProfile || "";
            profileForm.School = d.School || "";
            profileForm.Major = d.Major || "";
        }
    } catch (e) {
        console.error("获取用户信息失败:", e);
    } finally {
        profileLoading.value = false;
    }
};

const handleSaveProfile = async (formEl: FormInstance | undefined) => {
    if (!formEl) return;
    await formEl.validate(async (valid) => {
        if (!valid) return;
        profileSaving.value = true;
        try {
            const params: Api.User.UpdateUserInfoParams = {
                UserId: userInfo.value._id,
                Avatar: profileForm.Avatar,
                NickName: profileForm.NickName,
                PersonalProfile: profileForm.PersonalProfile,
                School: profileForm.School,
                Major: profileForm.Major,
            };
            const res = await updateUserInfo(params);
            if (res.data.success) {
                userStore.setUserInfo({
                    ...userInfo.value,
                    Avatar: profileForm.Avatar,
                    NickName: profileForm.NickName,
                });
            }
        } catch (e) {
            console.error("更新用户信息失败:", e);
        } finally {
            profileSaving.value = false;
        }
    });
};

// ── 密码修改表单 ──
const passwordFormRef = ref<FormInstance>();
const passwordSaving = ref(false);

const passwordForm = reactive({
    OldPassword: "",
    NewPassword: "",
    ConfirmPassword: "",
});

const passwordRules = reactive<FormRules>({
    OldPassword: [
        { required: true, message: "请输入旧密码", trigger: "blur" },
        { min: 6, max: 32, message: "密码长度为 6-32 个字符", trigger: "blur" },
    ],
    NewPassword: [
        { required: true, message: "请输入新密码", trigger: "blur" },
        { min: 6, max: 32, message: "密码长度为 6-32 个字符", trigger: "blur" },
    ],
    ConfirmPassword: [
        { required: true, message: "请确认新密码", trigger: "blur" },
        {
            validator: (_rule: unknown, value: string, callback: (error?: Error) => void) => {
                if (!value) callback(new Error("请确认新密码"));
                else if (value !== passwordForm.NewPassword) callback(new Error("两次密码不一致"));
                else callback();
            },
            trigger: "blur",
        },
    ],
});

const showOldPwd = ref(false);
const showNewPwd = ref(false);
const showConfirmPwd = ref(false);

const handleChangePassword = async (formEl: FormInstance | undefined) => {
    if (!formEl) return;
    await formEl.validate(async (valid) => {
        if (!valid) return;
        passwordSaving.value = true;
        try {
            const res = await updateUserPassword({
                OldPassWord: passwordForm.OldPassword,
                NewPassWord: passwordForm.NewPassword,
            });
            if (res.data.success) {
                ElMessage.success("密码修改成功，请重新登录");
                userStore.handleLogout();
            }
        } catch (e) {
            console.error("修改密码失败:", e);
        } finally {
            passwordSaving.value = false;
        }
    });
};

onMounted(() => {
    fetchProfileInfo();
});
</script>

<template>
    <div class="user-setting-page">
        <!-- 页面标题 -->
        <div class="page-header">
            <div class="header-content">
                <div class="header-icon">
                    <el-icon :size="28"><i-ep-setting /></el-icon>
                </div>
                <div>
                    <h1 class="page-title">个人设置</h1>
                    <p class="page-desc">管理您的个人资料和账户安全</p>
                </div>
            </div>
        </div>

        <!-- Tab 切换 -->
        <div class="setting-tabs">
            <button
                class="tab-btn"
                :class="{ active: activeTab === 'profile' }"
                @click="activeTab = 'profile'"
            >
                <el-icon><i-ep-user /></el-icon>
                <span>个人资料</span>
            </button>
            <button
                class="tab-btn"
                :class="{ active: activeTab === 'password' }"
                @click="activeTab = 'password'"
            >
                <el-icon><i-ep-lock /></el-icon>
                <span>修改密码</span>
            </button>
        </div>

        <!-- 个人资料 Tab -->
        <div v-show="activeTab === 'profile'" class="setting-panel">
            <el-skeleton :loading="profileLoading" animated :count="5">
                <template #template>
                    <div class="skeleton-row" v-for="i in 5" :key="i" />
                </template>
                <template #default>
                    <el-form
                        ref="profileFormRef"
                        :model="profileForm"
                        :rules="profileRules"
                        label-position="top"
                        class="setting-form"
                    >
                        <!-- 头像选择 -->
                        <el-form-item label="头像">
                            <div class="avatar-selector">
                                <div
                                    v-for="(avatar, idx) in AVATAR_OPTIONS"
                                    :key="idx"
                                    class="avatar-option"
                                    :class="{ selected: profileForm.Avatar === avatar }"
                                    @click="profileForm.Avatar = avatar"
                                >
                                    <el-avatar :size="64" :src="avatar" shape="square" />
                                    <div class="avatar-check">
                                        <el-icon :size="16"><i-ep-check /></el-icon>
                                    </div>
                                </div>
                            </div>
                        </el-form-item>

                        <!-- 昵称（只读） -->
                        <el-form-item label="昵称">
                            <el-input
                                v-model="profileForm.NickName"
                                disabled
                                placeholder="昵称不可修改"
                            >
                                <template #prefix>
                                    <el-icon><i-ep-user /></el-icon>
                                </template>
                            </el-input>
                        </el-form-item>

                        <!-- 个性签名 -->
                        <el-form-item label="个性签名">
                            <el-input
                                v-model="profileForm.PersonalProfile"
                                type="textarea"
                                :rows="3"
                                maxlength="200"
                                show-word-limit
                                placeholder="写点什么介绍自己吧..."
                            />
                        </el-form-item>

                        <!-- 学校 -->
                        <el-form-item label="学校" prop="School">
                            <el-input
                                v-model="profileForm.School"
                                placeholder="请输入学校"
                                clearable
                            >
                                <template #prefix>
                                    <el-icon><i-ep-school /></el-icon>
                                </template>
                            </el-input>
                        </el-form-item>

                        <!-- 专业 -->
                        <el-form-item label="专业" prop="Major">
                            <el-input
                                v-model="profileForm.Major"
                                placeholder="请输入专业"
                                clearable
                            >
                                <template #prefix>
                                    <el-icon><i-ep-collection /></el-icon>
                                </template>
                            </el-input>
                        </el-form-item>

                        <!-- 保存按钮 -->
                        <div class="form-actions">
                            <el-button
                                type="primary"
                                :loading="profileSaving"
                                class="save-btn"
                                @click="handleSaveProfile(profileFormRef)"
                            >
                                <el-icon><i-ep-check /></el-icon>
                                <span>{{ profileSaving ? "保存中..." : "保存修改" }}</span>
                            </el-button>
                        </div>
                    </el-form>
                </template>
            </el-skeleton>
        </div>

        <!-- 修改密码 Tab -->
        <div v-show="activeTab === 'password'" class="setting-panel">
            <div class="password-notice">
                <el-icon :size="18"><i-ep-warning /></el-icon>
                <span>修改密码后需要重新登录所有设备</span>
            </div>
            <el-form
                ref="passwordFormRef"
                :model="passwordForm"
                :rules="passwordRules"
                label-position="top"
                class="setting-form"
            >
                <el-form-item label="当前密码" prop="OldPassword">
                    <el-input
                        v-model="passwordForm.OldPassword"
                        :type="showOldPwd ? 'text' : 'password'"
                        placeholder="请输入当前密码"
                    >
                        <template #prefix>
                            <el-icon><i-ep-lock /></el-icon>
                        </template>
                        <template #suffix>
                            <el-icon
                                class="pwd-toggle"
                                @click="showOldPwd = !showOldPwd"
                            >
                                <i-ep-view v-if="showOldPwd" />
                                <i-ep-hide v-else />
                            </el-icon>
                        </template>
                    </el-input>
                </el-form-item>

                <el-form-item label="新密码" prop="NewPassword">
                    <el-input
                        v-model="passwordForm.NewPassword"
                        :type="showNewPwd ? 'text' : 'password'"
                        placeholder="请输入新密码（6-32 位）"
                    >
                        <template #prefix>
                            <el-icon><i-ep-lock /></el-icon>
                        </template>
                        <template #suffix>
                            <el-icon
                                class="pwd-toggle"
                                @click="showNewPwd = !showNewPwd"
                            >
                                <i-ep-view v-if="showNewPwd" />
                                <i-ep-hide v-else />
                            </el-icon>
                        </template>
                    </el-input>
                </el-form-item>

                <el-form-item label="确认新密码" prop="ConfirmPassword">
                    <el-input
                        v-model="passwordForm.ConfirmPassword"
                        :type="showConfirmPwd ? 'text' : 'password'"
                        placeholder="请再次输入新密码"
                        @keyup.enter="handleChangePassword(passwordFormRef)"
                    >
                        <template #prefix>
                            <el-icon><i-ep-lock /></el-icon>
                        </template>
                        <template #suffix>
                            <el-icon
                                class="pwd-toggle"
                                @click="showConfirmPwd = !showConfirmPwd"
                            >
                                <i-ep-view v-if="showConfirmPwd" />
                                <i-ep-hide v-else />
                            </el-icon>
                        </template>
                    </el-input>
                </el-form-item>

                <div class="form-actions">
                    <el-button
                        type="primary"
                        :loading="passwordSaving"
                        class="save-btn"
                        @click="handleChangePassword(passwordFormRef)"
                    >
                        <el-icon><i-ep-lock /></el-icon>
                        <span>{{ passwordSaving ? "修改中..." : "修改密码" }}</span>
                    </el-button>
                </div>
            </el-form>
        </div>
    </div>
</template>

<style scoped>
.user-setting-page {
    max-width: 720px;
    padding: var(--oj-spacing-6) var(--oj-spacing-4);
    margin: 0 auto;
}

/* ── 页面标题 ── */
.page-header {
    margin-bottom: var(--oj-spacing-6);
}

.header-content {
    display: flex;
    gap: var(--oj-spacing-4);
    align-items: center;
}

.header-icon {
    display: flex;
    align-items: center;
    justify-content: center;
    width: 52px;
    height: 52px;
    color: var(--oj-color-primary);
    background: var(--oj-color-primary-soft);
    border-radius: var(--oj-radius-xl);
}

.page-title {
    margin: 0;
    font-size: var(--oj-font-size-2xl);
    font-weight: var(--oj-font-weight-bold);
    color: var(--oj-text-color);
}

.page-desc {
    margin: var(--oj-spacing-1) 0 0;
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-color-secondary);
}

/* ── Tab 按钮 ── */
.setting-tabs {
    display: flex;
    gap: var(--oj-spacing-2);
    margin-bottom: var(--oj-spacing-6);
    border-bottom: 1px solid var(--oj-card-border);
}

.tab-btn {
    display: inline-flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    padding: var(--oj-spacing-3) var(--oj-spacing-4);
    font-size: var(--oj-font-size-sm);
    font-weight: var(--oj-font-weight-medium);
    color: var(--oj-text-color-secondary);
    cursor: pointer;
    background: transparent;
    border: none;
    border-bottom: 2px solid transparent;
    transition: all 0.25s ease;
}

.tab-btn:hover {
    color: var(--oj-text-color);
}

.tab-btn.active {
    color: var(--oj-color-primary);
    border-bottom-color: var(--oj-color-primary);
}

/* ── 面板 ── */
.setting-panel {
    padding: var(--oj-spacing-6);
    background: var(--oj-card-bg);
    border: 1px solid var(--oj-card-border);
    border-radius: var(--oj-radius-xl);
    backdrop-filter: blur(var(--oj-glass-blur));
}

/* ── 表单 ── */
.setting-form :deep(.el-form-item__label) {
    font-weight: 500;
    color: var(--oj-text-color-secondary);
}

.setting-form :deep(.el-input__wrapper),
.setting-form :deep(.el-textarea__inner) {
    background-color: rgb(var(--oj-color-primary-rgb) / 5%);
    border-radius: var(--oj-radius-lg);
    box-shadow: 0 0 0 1px rgb(var(--oj-color-primary-rgb) / 15%) inset;
    transition: all 0.25s ease;
}

.setting-form :deep(.el-input__wrapper:hover),
.setting-form :deep(.el-textarea__inner:hover) {
    background-color: rgb(var(--oj-color-primary-rgb) / 8%);
    box-shadow: 0 0 0 1px rgb(var(--oj-color-primary-rgb) / 25%) inset;
}

.setting-form :deep(.el-input__wrapper.is-focus),
.setting-form :deep(.el-textarea__inner:focus) {
    background-color: rgb(var(--oj-color-primary-rgb) / 6%);
    box-shadow:
        0 0 0 1px rgb(var(--oj-color-primary-rgb) / 40%) inset,
        0 0 0 3px rgb(var(--oj-color-primary-rgb) / 12%),
        0 0 16px rgb(var(--oj-color-primary-rgb) / 15%);
}

.setting-form :deep(.el-input__inner),
.setting-form :deep(.el-textarea__inner) {
    color: var(--oj-text-color);
}

.setting-form :deep(.el-input__inner::placeholder),
.setting-form :deep(.el-textarea__inner::placeholder) {
    color: var(--oj-text-color-muted);
    opacity: 0.6;
}

/* ── 头像选择器 ── */
.avatar-selector {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-4);
}

.avatar-option {
    position: relative;
    padding: 4px;
    cursor: pointer;
    border: 2px solid var(--oj-card-border);
    border-radius: var(--oj-radius-xl);
    transition: all 0.25s ease;
}

.avatar-option:hover {
    border-color: var(--oj-color-primary);
    box-shadow: 0 0 12px rgb(var(--oj-color-primary-rgb) / 20%);
    transform: translateY(-2px);
}

.avatar-option.selected {
    border-color: var(--oj-color-primary);
    box-shadow: 0 0 12px rgb(var(--oj-color-primary-rgb) / 30%);
}

.avatar-check {
    position: absolute;
    right: -4px;
    bottom: -4px;
    display: flex;
    align-items: center;
    justify-content: center;
    width: 24px;
    height: 24px;
    color: var(--oj-text-inverse);
    background: var(--oj-color-primary);
    border-radius: 50%;
    opacity: 0;
    transition: opacity 0.2s ease;
}

.avatar-option.selected .avatar-check {
    opacity: 1;
}

/* ── 密码提示 ── */
.password-notice {
    display: flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    padding: var(--oj-spacing-3) var(--oj-spacing-4);
    margin-bottom: var(--oj-spacing-6);
    font-size: var(--oj-font-size-sm);
    color: var(--oj-color-warning);
    background: var(--oj-color-warning-soft);
    border: 1px solid var(--oj-color-warning-border);
    border-radius: var(--oj-radius-lg);
}

.pwd-toggle {
    color: var(--oj-text-color-muted);
    cursor: pointer;
    transition: color 0.2s ease;
}

.pwd-toggle:hover {
    color: var(--oj-color-primary);
}

/* ── 操作按钮 ── */
.form-actions {
    display: flex;
    justify-content: flex-end;
    padding-top: var(--oj-spacing-4);
    margin-top: var(--oj-spacing-2);
    border-top: 1px solid var(--oj-card-border);
}

.save-btn {
    min-width: 140px;
    font-weight: 600;
}

.save-btn:hover {
    box-shadow: 0 0 20px rgb(var(--oj-color-primary-rgb) / 35%);
}

/* ── 骨架屏 ── */
.skeleton-row {
    height: 44px;
    margin-bottom: var(--oj-spacing-4);
    background: rgb(var(--oj-color-primary-rgb) / 8%);
    border-radius: var(--oj-radius-lg);
}

/* ── 响应式 ── */
@media (width >= 640px) {
    .user-setting-page {
        padding: var(--oj-spacing-8);
    }
}
</style>
