<script setup lang="ts">
/**
 * 用户下拉菜单组件
 * 显示用户信息和相关操作
 */
import { useRouter } from "vue-router";
import { useUserStore } from "@/stores/modules/user";
import { userLogout } from "@/api/user";

defineOptions({ name: "UserDropdown" });

const router = useRouter();
// 用户状态管理
const userStore = useUserStore();

// 用户状态管理获取登录状态和用户信息
const isLoggedIn = userStore.getIsLoggedIn;
const userInfo = userStore.getUserInfo;

// 处理菜单命令
const handleCommand = (command: string) => {
    switch (command) {
        case "profile":
            router.push("/user/profile");
            break;
        case "submissions":
            router.push("/status?user=me");
            break;
        case "settings":
            router.push("/user/settings");
            break;
        case "admin":
            router.push("/admin");
            break;
        case "logout":
            // 调用登出接口
            handleLogout();
            break;
    }
};

// 跳转登录页
const goToLogin = () => {
    router.push("/auth/login");
};

// 跳转注册页
const goToRegister = () => {
    router.push("/auth/register");
};

/**
 * 处理退出登录
 */
const handleLogout = async () => {
    const response = await userLogout();
    // 判断登出是否成功
    if (response.data.success && response.data.code == 0) {
        // 清除用户状态管理
        userStore.handleLogout();
    }
};
</script>

<template>
    <!-- 已登录状态 -->
    <el-dropdown v-if="isLoggedIn" trigger="click" @command="handleCommand">
        <el-button round size="large" class="user-btn">
            <el-avatar :src="userInfo.Avatar" :alt="userInfo.NickName" class="user-avatar" />
            <span class="user-name">{{ userInfo.NickName }}</span>
            <el-icon class="dropdown-arrow" :size="12">
                <i-ep-arrow-down />
            </el-icon>
        </el-button>

        <template #dropdown>
            <el-dropdown-menu>
                <el-dropdown-item command="profile">
                    <el-icon>
                        <i-ep-user />
                    </el-icon>
                    <span>个人主页</span>
                </el-dropdown-item>
                <el-dropdown-item command="submissions">
                    <el-icon>
                        <i-ep-list />
                    </el-icon>
                    <span>我的提交</span>
                </el-dropdown-item>
                <el-dropdown-item command="settings">
                    <el-icon>
                        <i-ep-setting />
                    </el-icon>
                    <span>设置</span>
                </el-dropdown-item>
                <el-dropdown-item command="admin" divided>
                    <el-icon>
                        <i-ep-operation />
                    </el-icon>
                    <span>管理后台</span>
                </el-dropdown-item>
                <el-dropdown-item command="logout" divided>
                    <el-icon class="logout-icon">
                        <i-ep-switch-button />
                    </el-icon>
                    <span class="logout-text">退出登录</span>
                </el-dropdown-item>
            </el-dropdown-menu>
        </template>
    </el-dropdown>

    <!-- 未登录状态 -->
    <div v-else class="auth-buttons">
        <el-button class="login-btn" @click="goToLogin">登录</el-button>
        <el-button class="register-btn" @click="goToRegister">注册</el-button>
    </div>
</template>

<style scoped>
.user-btn {
    display: flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    padding: var(--oj-spacing-1) var(--oj-spacing-2);
    cursor: pointer;
    background: transparent;
    border: none;
    border-radius: var(--oj-radius-full);
    transition: all 0.2s;
}

.user-btn:hover {
    background: var(--oj-bg-hover);
}

.user-avatar {
    width: 32px;
    height: 32px;
    border: 2px solid var(--oj-color-primary);
    border-radius: var(--oj-radius-full);
}

.user-name {
    display: none;
    font-size: var(--oj-font-size-sm);
    font-weight: 500;
    margin: 0 var(--oj-spacing-2);
    color: var(--oj-text-secondary);
    transition: color 0.2s;
}

.user-btn:hover .user-name {
    color: var(--oj-text-color);
}

@media (width >=768px) {
    .user-name {
        display: block;
    }
}

.dropdown-arrow {
    color: var(--oj-text-muted);
}

:deep(.el-dropdown-menu__item) {
    display: flex;
    gap: var(--oj-spacing-2);
    align-items: center;
}

.logout-icon {
    color: var(--oj-color-danger);
}

.logout-text {
    color: var(--oj-color-danger);
}

/* 未登录按钮 */
.auth-buttons {
    display: flex;
    gap: var(--oj-spacing-2);
    align-items: center;
}

.login-btn {
    padding: var(--oj-spacing-1-5) var(--oj-spacing-4);
    font-family: var(--oj-font-family-mono), monospace;
    font-size: var(--oj-font-size-sm);
    color: var(--oj-color-primary);
    cursor: pointer;
    background: transparent;
    border: 1px solid var(--oj-color-primary);
    border-radius: var(--oj-radius-md);
    transition: all 0.3s;
}

.login-btn:hover {
    color: var(--oj-text-color);
    background: var(--oj-color-primary);
}

.register-btn {
    padding: var(--oj-spacing-1-5) var(--oj-spacing-4);
    font-family: var(--oj-font-family-mono), monospace;
    font-size: var(--oj-font-size-sm);
    color: white;
    cursor: pointer;
    background: var(--oj-color-primary);
    border: 1px solid var(--oj-color-primary);
    border-radius: var(--oj-radius-md);
    box-shadow: var(--oj-shadow-neon);
    transition: all 0.3s;
}

.register-btn:hover {
    background: var(--oj-color-primary-dark);
}
</style>
