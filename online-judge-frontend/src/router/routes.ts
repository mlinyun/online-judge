import type { RouteRecordRaw } from "vue-router";

// 布局组件
// 主布局组件
const MainLayout = () => import("@/layouts/main/index.vue");
// 认证布局组件
const AuthLayout = () => import("@/layouts/auth/index.vue");
// 管理后台布局组件
const AdminLayout = () => import("@/layouts/admin/index.vue");

// 主应用相关页面组件将使用路由懒加载，在路由配置中直接引入
// 首页（包含公告列表）
const HomeView = () => import("@views/home/index.vue");
// 公告列表页面
const AnnouncementList = () => import("@views/home/list/index.vue");
// 公告详情页面
const AnnouncementDetail = () => import("@views/home/detail/index.vue");
// 题目列表页
const ProblemList = () => import("@views/problem/list/index.vue");
// 题目详情页
const ProblemDetail = () => import("@views/problem/detail/index.vue");
// 讨论列表页
const DiscussionList = () => import("@views/discussion/list/index.vue");
// 讨论详情页
const DiscussionDetail = () => import("@views/discussion/detail/index.vue");
// 题解列表页
const SolutionList = () => import("@views/solution/list/index.vue");
// 题解详情页
const SolutionDetail = () => import("@views/solution/detail/index.vue");
// 测评状态页
const StatusRecord = () => import("@views/status-record/index.vue");
// 用户排名页
const UserRank = () => import("@views/user/rank/index.vue");
// 用户资料页
const UserProfile = () => import("@views/user/profile/index.vue");
// 个人设置页
const UserSetting = () => import("@views/user/setting/index.vue");

// 认证相关页面组件将使用路由懒加载，在路由配置中直接引入
// 用户登录页
const UserLogin = () => import("@views/user/login/index.vue");
// 用户注册页
const UserRegister = () => import("@views/user/register/index.vue");

// 管理后台相关页面组件将使用路由懒加载，在路由配置中直接引入
// 仪表盘页
const DashboardView = () => import("@views/admin/dashboard/index.vue");
// 题目管理页
const AdminProblem = () => import("@views/admin/problem/index.vue");
// 题解管理页
const AdminSolution = () => import("@views/admin/solution/index.vue");
// 公告管理页
const AdminAnnouncement = () => import("@views/admin/announcement/index.vue");
// 用户管理页
const AdminUser = () => import("@views/admin/user/index.vue");
// 讨论管理页
const AdminDiscussion = () => import("@views/admin/discussion/index.vue");
// 评论管理页
const AdminComment = () => import("@views/admin/comment/index.vue");
// 系统设置页
const AdminSetting = () => import("@views/admin/setting/index.vue");

// 编辑器相关页面组件
// 公告编辑器
const AnnouncementEditor = () => import("@views/editor/announcement/index.vue");
// 讨论编辑器
const DiscussionEditor = () => import("@views/editor/discuss/index.vue");
// 题解编辑器
const SolutionEditor = () => import("@views/editor/solution/index.vue");
// 题目编辑器
const ProblemEditor = () => import("@views/editor/problem/index.vue");

// 定义路由数组
export const routes: Array<RouteRecordRaw> = [
    // 主布局路由
    {
        path: "/",
        component: MainLayout,
        children: [
            {
                path: "",
                name: "home",
                component: HomeView,
                meta: { title: "首页" },
            },
            {
                path: "announcement/list",
                name: "announcement-list",
                component: AnnouncementList,
                meta: { title: "公告列表页" },
            },
            {
                path: "announcement/detail/:id",
                name: "announcement-detail",
                component: AnnouncementDetail,
                meta: { title: "公告详情页" },
            },
            {
                path: "problem/list",
                name: "problem-list",
                component: ProblemList,
                meta: { title: "题库" },
            },
            {
                path: "problem/detail/:id",
                name: "problem-detail",
                component: ProblemDetail,
                meta: { title: "题目详情" },
            },
            {
                path: "discussion/list",
                name: "discussion-list",
                component: DiscussionList,
                meta: { title: "讨论" },
            },
            {
                path: "discussion/detail/:id",
                name: "discussion-detail",
                component: DiscussionDetail,
                meta: { title: "讨论详情" },
            },
            {
                path: "solution/list",
                name: "solution-list",
                component: SolutionList,
                meta: { title: "题解" },
            },
            {
                path: "solution/detail/:id",
                name: "solution-detail",
                component: SolutionDetail,
                meta: { title: "题解详情" },
            },
            {
                path: "status-record",
                name: "status-record",
                component: StatusRecord,
                meta: { title: "测评状态" },
            },
            {
                path: "user/rank",
                name: "user-rank",
                component: UserRank,
                meta: { title: "排行榜" },
            },
            {
                path: "user/profile",
                name: "user-profile",
                component: UserProfile,
                meta: { title: "个人主页" },
            },
            {
                path: "/user/setting",
                name: "user-setting",
                component: UserSetting,
                meta: { title: "个人设置" },
            },
        ],
    },

    // 认证布局路由
    {
        path: "/auth",
        component: AuthLayout,
        redirect: "/auth/login",
        children: [
            {
                path: "login",
                name: "auth-login",
                component: UserLogin,
                meta: { title: "登录" },
            },
            {
                path: "register",
                name: "auth-register",
                component: UserRegister,
                meta: { title: "注册" },
            },
        ],
    },

    // 管理后台路由
    {
        path: "/admin",
        redirect: "/admin/dashboard",
        component: AdminLayout,
        meta: { title: "管理后台", requiresAuth: true, requiresAdmin: true },
        children: [
            {
                path: "dashboard",
                name: "admin-dashboard",
                component: DashboardView,
                meta: { title: "仪表盘" },
            },
            {
                path: "problem",
                name: "admin-problem",
                component: AdminProblem,
                meta: { title: "题目管理" },
            },
            {
                path: "problem/editor",
                name: "problem-editor",
                component: ProblemEditor,
                meta: { title: "题目编辑" },
            },
            {
                path: "solution",
                name: "admin-solution",
                component: AdminSolution,
                meta: { title: "题解管理" },
            },
            {
                path: "solution/editor",
                name: "solution-editor",
                component: SolutionEditor,
                meta: { title: "题解编辑" },
            },
            {
                path: "announcement",
                name: "admin-announcement",
                component: AdminAnnouncement,
                meta: { title: "公告管理" },
            },
            {
                path: "announcement/editor",
                name: "announcement-editor",
                component: AnnouncementEditor,
                meta: { title: "公告编辑" },
            },
            {
                path: "user",
                name: "admin-user",
                component: AdminUser,
                meta: { title: "用户管理" },
            },
            {
                path: "discussion",
                name: "admin-discussion",
                component: AdminDiscussion,
                meta: { title: "讨论管理" },
            },
            {
                path: "discussion/editor",
                name: "discussion-editor",
                component: DiscussionEditor,
                meta: { title: "讨论编辑" },
            },
            {
                path: "comment",
                name: "admin-comment",
                component: AdminComment,
                meta: { title: "评论管理" },
            },
            {
                path: "setting",
                name: "admin-setting",
                component: AdminSetting,
                meta: { title: "系统设置" },
            },
        ],
    },
    // 404 页面路由
    {
        path: "/:pathMatch(.*)*",
        redirect: "/",
    },
];
