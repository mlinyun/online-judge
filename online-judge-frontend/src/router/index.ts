import {
    createRouter,
    createWebHistory,
    type NavigationGuardNext,
    type RouteLocationNormalizedGeneric,
} from "vue-router";
import { routes } from "@/router/routes";
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

/**
 * 路由前置守卫
 *
 * 用于处理页面跳转时的进度条显示
 */
router.beforeEach((to, from, next: NavigationGuardNext) => {
    // 开始顶部进度条
    NProgress.start();
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
