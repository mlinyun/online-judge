// 该文件专门用于创建路由器
import {createRouter , createWebHashHistory} from 'vue-router'

// 跳转进度条
import NProgress from 'nprogress'
import 'nprogress/nprogress.css'
 
NProgress.configure({
    easing: 'ease', // 动画方式
    speed: 500, // 递增进度条的速度
    showSpinner: false, // 是否显示加载 icon
    trickleSpeed: 200, // 自动递增间隔
    minimum: 0.3 // 初始化时的最小百分比
})

// 路由懒加载
const HomePage = () => import('@/pages/HomePage.vue')
const ProblemList = () => import('@/pages/ProblemList.vue')
const Problem = () => import('@/pages/Problem.vue')
const StatusRecord = () => import('@//pages/StatusRecord.vue')
const DiscussList = () => import('@/pages/DiscussList.vue') 
const UserRank = () => import('@/pages/UserRank.vue') 
const UserHome = () => import('@/pages/UserHome.vue') 
const UserSetting = () => import('@/pages/UserSetting.vue')
const Admin = () => import('@/pages/Admin.vue')
const ProblemEditor = () => import('@/pages/Editor/ProblemEditor.vue')
const Announcement = () => import('@/pages/Announcement.vue')
const SolutionList = () => import('@/pages/SolutionList.vue') 
const Solution = () => import('@/pages/Solution.vue')
const Discuss = () => import('@/pages/Discuss.vue')

const AdminDiscuss = () => import('@/pages/Admin/AdminDiscuss.vue')
const AdminProblem = () => import('@/pages/Admin/AdminProblem.vue') 
const AdminUser = () => import('@/pages/Admin/AdminUser.vue')
const AdminAnnouncement = () => import('@/pages/Admin/AdminAnnouncement.vue') 
const AdminSolution = () => import('@/pages/Admin/AdminSolution.vue')
const AdminComment = () => import('@/pages/Admin/AdminComment.vue') 

const AnnouncementEditor = () => import('@/pages/Editor/AnnouncementEditor.vue')
const SolutionEditor = () => import('@/pages/Editor/SolutionEditor.vue')
const DiscussEditor = () => import('@/pages/Editor/DiscussEditor.vue')  

const routes = [
    {
        path:'/',
        name:'HomePage',
        component:HomePage,
        meta: { title: 'HomePage | XDOJ' },
    },
    {
        path:'/problemlist',
        name:'ProblemList',
        component:ProblemList,
        meta: { title: 'ProblemList | XDOJ' },
    },
    {
        path:'/problem',
        name:'Problem',
        component:Problem,
        meta: { title: 'Problem | XDOJ' },
    },
    {
        path:'/statusrecord',
        name:'StatusRecord',
        component:StatusRecord,
        meta: { title: 'StatusRecord | XDOJ' },
    },
    {
        path:'/discusslist',
        name:'DiscussList',
        component:DiscussList,
        meta: { title: 'DiscussList | XDOJ' },
    },
    {
        path:'/userrank',
        name:'UserRank',
        component:UserRank,
        meta: { title: 'UserRank | XDOJ' },
    },
    {
        path:'/userhome',
        name:'UserHome',
        component:UserHome,
        meta: { title: 'UserHome | XDOJ' },
    },
    {
        path:'/usersetting',
        name:'UserSetting',
        component:UserSetting,
        meta: { title: 'UserSetting | XDOJ' },
    },
    {
        path:'/admin',
        name:'Admin',
        component:Admin,
        meta: { title: 'Admin | XDOJ' },
        children:[
            {
                path:'admindiscuss',
                component: AdminDiscuss,
                meta: { title: 'AdminDiscuss | XDOJ' },
            },
            {
                path:'adminproblem',
                component: AdminProblem,
                meta: { title: 'AdminProblem | XDOJ' },
            },
            {
                path:'adminuser',
                component: AdminUser,
                meta: { title: 'AdminUser | XDOJ' },
            },
            {
                path:'adminannouncement',
                component: AdminAnnouncement,
                meta: { title: 'AdminAnnouncement | XDOJ' },
            },
            {
                path:'adminsolution',
                component: AdminSolution,
                meta: { title: 'AdminSolution | XDOJ' },
            },
            {
                path:'admincomment',
                component: AdminComment,
                meta: { title: 'AdminComment | XDOJ' },
            }
        ]
    },
    {
        path:'/problemeditor',
        name:'ProblemEditor',
        component:ProblemEditor,
        meta: { title: 'ProblemEditor | XDOJ' },
    },
    {
        path:'/announcement',
        name:'Announcement',
        component:Announcement,
        meta: { title: 'Announcement | XDOJ' },
    },
    {
        path:'/solution',
        name:'Solution',
        component:Solution,
        meta: { title: 'Solution | XDOJ' },
    },
    {
        path:'/discuss',
        name:'Discuss',
        component:Discuss,
        meta: { title: 'Discuss | XDOJ' },
    },
    {
        path:'/solutionlist',
        name:'SolutionList',
        component:SolutionList,
        meta: { title: 'SolutionList | XDOJ' },
    },
    {
        path:'/announcementeditor',
        name:'AnnouncementEditor',
        component:AnnouncementEditor,
        meta: { title: 'AnnouncementEditor | XDOJ' },
    },
    {
        path:'/solutioneditor',
        name:'SolutionEditor',
        component:SolutionEditor,
        meta: { title: 'SolutionEditor | XDOJ' },
    },
    {
        path:'/discusseditor',
        name:'DiscussEditor',
        component:DiscussEditor,
        meta: { title: 'DiscussEditor | XDOJ' },
    },    
]

//创建并暴露一个路由器
const router = createRouter({
    history:createWebHashHistory(),
	routes:routes
})

router.beforeEach((to, from, next) => {
    to.meta.title && (document.title = to.meta.title);
    NProgress.start() // 进度条开始
    next()
})

router.afterEach(() => {
    NProgress.done() // 进度条结束
})
   
export default router