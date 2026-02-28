import {
    createRouter,
    createWebHistory,
    type NavigationGuardNext,
    type RouteLocationNormalizedGeneric,
} from "vue-router";
import { routes } from "@/router/routes";
import { useUserStore } from "@/stores/modules/user";
import { loginUserByToken } from "@/api/user";
import { ElMessage } from "element-plus";
// 跳转进度条样
import NProgress from "nprogress";
import "nprogress/nprogress.css";

// 顶部进度条配置
NProgress.configure({
    easing: "ease", // 动画方式
    speed: 500, // 递增进度条的速度
    showSpinner: false, // 是否显示加载 icon
    trickleSpeed: 200, // 自动递增间隔
    minimum: 0.3, // 初始化时的最小百分比
});

/**
 * 创建路由实例
 */
const router = createRouter({
    history: createWebHistory(import.meta.env.BASE_URL),
    routes,
});

/** 是否正在恢复 Token 登录（防止重复调用） */
let isRestoringToken = false;

/**
 * 路由前置守卫
 *
 * - Token 自动恢复登录
 * - 已登录用户不允许访问认证页面
 * - 需要登录 / 管理员权限的页面拦截
 */
router.beforeEach(async (to, from, next: NavigationGuardNext) => {
    // 开始顶部进度条
    NProgress.start();

    const userStore = useUserStore();

    // Token 自动恢复登录：有持久化 Token 但 Store 未登录时尝试恢复
    if (!userStore.isLoggedIn && userStore.accessToken && !isRestoringToken) {
        isRestoringToken = true;
        try {
            const response = await loginUserByToken();
            if (response.data.success && response.data.data) {
                userStore.setUserInfo(response.data.data);
                userStore.setIsLoggedIn(true);
                userStore.setAccessToken(response.data.data.Token);
            } else {
                // Token 无效，清除状态
                userStore.setIsLoggedIn(false);
                userStore.setAccessToken("");
            }
        } catch {
            // Token 过期或请求失败，清除状态
            userStore.setIsLoggedIn(false);
            userStore.setAccessToken("");
        } finally {
            isRestoringToken = false;
        }
    }

    // 已登录用户不允许再访问认证页面（登录/注册）
    if (to.path.startsWith("/auth") && userStore.isLoggedIn) {
        return next("/");
    }

    // 需要登录的页面
    if (to.meta.requiresAuth && !userStore.isLoggedIn) {
        return next({ path: "/auth/login", query: { redirect: to.fullPath } });
    }

    // 需要管理员权限的页面（检查路由链中任一层级的 meta）
    const needAdmin = to.matched.some((record) => record.meta.requiresAdmin);
    if (needAdmin && !userStore.isAdmin) {
        ElMessage.warning("无权访问管理后台");
        return next("/");
    }

    next();
});

/**
 * 路由后置守卫
 *
 * 用于页面标题的动态设置
 */
router.afterEach((to: RouteLocationNormalizedGeneric) => {
    // 设置页面标题
    const defaultTitle = "凌云在线编程评测平台";
    document.title = to.meta.title ? `${to.meta.title} - ${defaultTitle}` : defaultTitle;
    // 结束顶部进度条
    NProgress.done();
    // 确保进度条完全结束，避免残影
    setTimeout(() => {
        NProgress.remove();
    }, 600);
});

export default router;
