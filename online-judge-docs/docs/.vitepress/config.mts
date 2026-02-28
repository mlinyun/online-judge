import { defineConfig } from "vitepress";

// https://vitepress.dev/reference/site-config
export default defineConfig({
  base: "/",
  lang: "zh-CN",
  title: "凌云在线编程评测平台",
  description:
    "OJ 是 Online Judge 系统的简称，用来在线检测程序源代码的正确性。",
  head: [["link", { rel: "icon", type: "image/x-icon", href: "/favicon.ico" }]],
  srcDir: "./src",
  markdown: {
    lineNumbers: true,
  },
  cleanUrls: true,
  sitemap: {
    hostname: "https://docs.oj.mlinyun.com",
  },
  themeConfig: {
    // https://vitepress.dev/reference/default-theme-config
    logo: "/online-judge-logo.svg",
    siteTitle: "凌云在线编程评测平台",
    search: {
      provider: "local",
      options: {
        translations: {
          button: {
            buttonText: "搜索",
            buttonAriaLabel: "搜索",
          },
          modal: {
            noResultsText: "无法找到相关结果",
            resetButtonTitle: "重置搜索条件",
            footer: {
              selectText: "选择",
              navigateText: "切换",
              closeText: "关闭",
            },
          },
        },
      },
    },
    nav: [
      {
        text: "首页",
        link: "/",
      },
      {
        text: "项目介绍",
        link: "/introduce/doc00-project-introduction",
      },
      {
        text: "项目设计",
        items: [
          {
            text: "文档01 项目演示",
            link: "/design/doc01-project-demonstration",
          },
          {
            text: "文档02 需求分析",
            link: "/design/doc02-requirement-analysis",
          },
          {
            text: "文档03 数据库设计",
            link: "/design/doc03-database-design",
          },
          {
            text: "文档04 接口文档",
            link: "/design/doc04-api-documentation",
          },
        ],
      },
      {
        text: "项目开发",
        items: [
          {
            text: "开发环境",
            items: [
              {
                text: "文档05 开发环境配置",
                link: "/develop/env/doc05-environment-setup",
              },
              {
                text: "文档06 Python 包管理工具 Conda",
                link: "/develop/env/doc06-python-conda",
              },
              {
                text: "文档07 CMake 构建工具",
                link: "/develop/env/doc07-cmake",
              },
              {
                text: "文档08 C++ 包管理器 Conan",
                link: "/develop/env/doc08-cpp-conan",
              },
              {
                text: "文档09 MongoDB 数据库安装与配置",
                link: "/develop/env/doc09-mongodb-installation",
              },
              {
                text: "文档10 Redis 数据库的安装与配置",
                link: "/develop/env/doc10-redis-installation",
              },
            ],
          },
          {
            text: "后端开发",
            items: [
              {
                text: "实验01 CMake 构建工具讲解",
                link: "/develop/backend/lab01-cmake-introduction",
              },
              {
                text: "实验02 构建 HTTP 服务器项目",
                link: "/develop/backend/lab02-http-server-project",
              },
              {
                text: "实验03 使用 MongoDB C++ 驱动程序操作数据库",
                link: "/develop/backend/lab03-mongodb-cpp-driver",
              },
              {
                text: "实验04 使用 redis-plus-plus 连接 Redis 数据库",
                link: "/develop/backend/lab04-redis-plus-plus",
              },
              {
                text: "实验05 初始化 Online Judge 后端项目",
                link: "/develop/backend/lab05-online-judge-backend-init",
              },
              {
                text: "实验06 C++ 代码格式化工具集成",
                link: "/develop/backend/lab06-cpp-formatting-tools",
              },
              {
                text: "实验07 完善后端项目基础框架",
                link: "/develop/backend/lab07-backend-framework-improvement",
              },
              {
                text: "实验08 用户模块实现",
                link: "/develop/backend/lab08-user-module",
              },
              {
                text: "实验09 题目模块实现",
                link: "/develop/backend/lab09-problem-module",
              },
              {
                text: "实验10 公告模块实现",
                link: "/develop/backend/lab10-announcement-module",
              },
              {
                text: "实验11 讨论模块实现",
                link: "/develop/backend/lab11-discussion-module",
              },
              {
                text: "实验12 题解模块实现",
                link: "/develop/backend/lab12-solution-module",
              },
              {
                text: "实验13 评论模块实现",
                link: "/develop/backend/lab13-comment-module",
              },
              {
                text: "实验14 测评记录模块实现",
                link: "/develop/backend/lab14-judge-record-module",
              },
              {
                text: "实验15 安全沙箱讲解",
                link: "/develop/backend/lab15-security-sandbox",
              },
              {
                text: "实验16 判题模块实现",
                link: "/develop/backend/lab16-judge-module",
              },
              {
                text: "实验17 Token 鉴权实现",
                link: "/develop/backend/lab17-token-authentication",
              },
              {
                text: "实验18 Redis 缓存实现",
                link: "/develop/backend/lab18-redis-cache",
              },
            ],
          },
          {
            text: "前端开发",
            items: [
              {
                text: "文档12 前端项目结构",
                link: "/develop/frontend/doc12-frontend-project-structure",
              },
            ],
          },
        ],
      },
      {
        text: "项目部署",
        items: [
          {
            text: "文档11 项目部署方案",
            link: "/deploy/doc11-project-deployment-plan",
          },
          {
            text: "文档12 后端项目部署",
            link: "/deploy/doc12-backend-deployment",
          },
          {
            text: "文档13 前端项目部署",
            link: "/deploy/doc13-frontend-deployment",
          },
        ],
      },
    ],

    sidebar: {
      "/introduce/": [
        {
          text: "项目介绍",
          items: [
            {
              text: "文档00 项目介绍",
              link: "/introduce/doc00-project-introduction",
            },
          ],
        },
      ],
      "/design/": [
        {
          text: "项目设计",
          items: [
            {
              text: "文档01 项目演示",
              link: "/design/doc01-project-demonstration",
            },
            {
              text: "文档02 需求分析",
              link: "/design/doc02-requirement-analysis",
            },
            {
              text: "文档03 数据库设计",
              link: "/design/doc03-database-design",
            },
            {
              text: "文档04 接口文档",
              link: "/design/doc04-api-documentation",
            },
          ],
        },
      ],
      "/develop/": [
        {
          text: "项目开发",
          items: [
            {
              text: "开发环境",
              items: [
                {
                  text: "文档05 开发环境配置",
                  link: "/develop/env/doc05-environment-setup",
                },
                {
                  text: "文档06 Python 包管理工具 Conda",
                  link: "/develop/env/doc06-python-conda",
                },
                {
                  text: "文档07 CMake 构建工具",
                  link: "/develop/env/doc07-cmake",
                },
                {
                  text: "文档08 C++ 包管理器 Conan",
                  link: "/develop/env/doc08-cpp-conan",
                },
                {
                  text: "文档09 MongoDB 数据库安装与配置",
                  link: "/develop/env/doc09-mongodb-installation",
                },
                {
                  text: "文档10 Redis 数据库的安装与配置",
                  link: "/develop/env/doc10-redis-installation",
                },
              ],
              collapsed: true,
            },
            {
              text: "后端开发",
              items: [
                {
                  text: "实验01 CMake 构建工具讲解",
                  link: "/develop/backend/lab01-cmake-introduction",
                },
                {
                  text: "实验02 构建 HTTP 服务器项目",
                  link: "/develop/backend/lab02-http-server-project",
                },
                {
                  text: "实验03 使用 MongoDB C++ 驱动程序操作数据库",
                  link: "/develop/backend/lab03-mongodb-cpp-driver",
                },
                {
                  text: "实验04 使用 redis-plus-plus 连接 Redis 数据库",
                  link: "/develop/backend/lab04-redis-plus-plus",
                },
                {
                  text: "实验05 初始化 Online Judge 后端项目",
                  link: "/develop/backend/lab05-online-judge-backend-init",
                },
                {
                  text: "实验06 C++ 代码格式化工具集成",
                  link: "/develop/backend/lab06-cpp-formatting-tools",
                },
                {
                  text: "实验07 完善后端项目基础框架",
                  link: "/develop/backend/lab07-backend-framework-improvement",
                },
                {
                  text: "实验08 用户模块实现",
                  link: "/develop/backend/lab08-user-module",
                },
                {
                  text: "实验09 题目模块实现",
                  link: "/develop/backend/lab09-problem-module",
                },
                {
                  text: "实验10 公告模块实现",
                  link: "/develop/backend/lab10-announcement-module",
                },
                {
                  text: "实验11 讨论模块实现",
                  link: "/develop/backend/lab11-discussion-module",
                },
                {
                  text: "实验12 题解模块实现",
                  link: "/develop/backend/lab12-solution-module",
                },
                {
                  text: "实验13 评论模块实现",
                  link: "/develop/backend/lab13-comment-module",
                },
                {
                  text: "实验14 测评记录模块实现",
                  link: "/develop/backend/lab14-judge-record-module",
                },
                {
                  text: "实验15 安全沙箱讲解",
                  link: "/develop/backend/lab15-security-sandbox",
                },
                {
                  text: "实验16 判题模块实现",
                  link: "/develop/backend/lab16-judge-module",
                },
                {
                  text: "实验17 Token 鉴权实现",
                  link: "/develop/backend/lab17-token-authentication",
                },
                {
                  text: "实验18 Redis 缓存实现",
                  link: "/develop/backend/lab18-redis-cache",
                },
              ],
              collapsed: true,
            },
            {
              text: "前端开发",
              items: [
                {
                  text: "文档12 前端项目结构",
                  link: "/develop/frontend/doc12-frontend-project-structure",
                },
              ],
              collapsed: true,
            },
          ],
        },
      ],
      "/deploy/": [
        {
          text: "文档13 部署环境准备",
          link: "/deploy/deployment-environment",
        },
        {
          text: "文档14 后端项目部署",
          link: "/deploy/backend-deployment",
        },
        {
          text: "文档15 前端项目部署",
          link: "/deploy/frontend-deployment",
        },
      ],
    },

    outline: {
      label: "本页目录",
      level: [2, 3],
    },
    returnToTopLabel: "返回顶部",
    sidebarMenuLabel: "菜单",
    darkModeSwitchLabel: "主题",
    darkModeSwitchTitle: "切换到深色主题",
    lightModeSwitchTitle: "切换到浅色主题",
    socialLinks: [
      { icon: "github", link: "https://github.com/mlinyun/online-judge" },
    ],
    editLink: {
      pattern:
        "https://github.com/mlinyun/online-judge/blob/main/online-judge-docs/docs/src/:path",
      text: "在 GitHub 上编辑此页面",
    },
    docFooter: { prev: "上一篇", next: "下一篇" },
    lastUpdated: {
      text: "最后更新于",
      formatOptions: {
        dateStyle: "short",
        timeStyle: "short",
        timeZone: "Asia/Shanghai",
      },
    },
    footer: {
      message:
        '版权所有 © 2026 mlinyun &nbsp;&nbsp; \
        <a href="https://beian.miit.gov.cn/" target="_blank" rel="noopener noreferrer" style="text-decoration:none; color:inherit">粤ICP备2025419292号-1</a>',
      copyright:
        "文档内容仅供参考，未经书面许可，不得复制、转载或用于商业用途。文档中示例代码的许可以仓库中的 LICENSE 文件为准",
    },
  },
});
