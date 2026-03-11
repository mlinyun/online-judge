<p align="center">
  <img src="https://static.mlinyun.com/online-judge/online-judge-logo.svg" alt="Online Judge" width="80"/>
</p>

# 凌云在线编程评测平台 (LYOJ)

<p align="center">
  <b>致力于为每一位算法爱好者提供稳定、高效、美观的练习平台</b>
</p>

[![C++](https://img.shields.io/badge/Backend-C%2B%2B17-00599C?logo=c%2B%2B&logoColor=white)](https://isocpp.org/)
[![Vue](https://img.shields.io/badge/Frontend-Vue%203-4FC08D?logo=vue.js&logoColor=white)](https://vuejs.org/)
[![Vite](https://img.shields.io/badge/Build-Vite-646CFF?logo=vite&logoColor=white)](https://vitejs.dev/)
[![cpp-httplib](https://img.shields.io/badge/cpp--httplib-0.27.0-FB8C00?logo=c%2B%2B&logoColor=white)](https://github.com/yhirose/cpp-httplib)
[![jsoncpp](https://img.shields.io/badge/jsoncpp-1.9.6-2E7D32?logo=json&logoColor=white)](https://github.com/open-source-parsers/jsoncpp)
[![mongo-cxx-driver](https://img.shields.io/badge/mongo--cxx--driver-4.0.0-47A248?logo=mongodb&logoColor=white)](https://github.com/mongodb/mongo-cxx-driver)
[![redis-plus-plus](https://img.shields.io/badge/redis--plus--plus-1.3.15-DC382D?logo=redis&logoColor=white)](https://github.com/sewenew/redis-plus-plus)
[![License](https://img.shields.io/badge/License-MIT-9C27B0?logo=open-source-initiative&logoColor=white)](LICENSE)

## 📖 项目简介

**凌云在线编程评测平台 (LYOJ)** 是一款高性能、现代化的算法在线评测平台。系统采用前后端分离架构，后端基于 C++17 构建，核心判题系统安全稳定；前端基于 Vue 3 和 TypeScript 开发，提供极致的用户交互体验与赛博朋克风格的现代化后台管理系统。

## ✨ 核心特性

### 🚀 判题系统 (Judger)

- **多语言支持**: 完美支持 C, C++, Java, Go, Python2/3, JavaScript 等主流编程语言。
- **安全沙箱**: 基于青岛大学开源沙箱核心进行深度定制，确保用户代码在受限环境（时间、内存、系统调用）下安全运行。
- **Special Judge (SPJ)**: 支持特约判题，解决多解、精度误差等复杂判定场景。
- **实时反馈**: 毫秒级判题响应，提供 AC, WA, TLE, MLE, RE, CE 等全方位评测结果。

### 🎨 前端交互 (Frontend)

- **现代化 UI**: 结合 Element Plus 与 Undraw UI，打造简洁而不失科技感的交互界面。
- **深度定制编辑器**: 集成 Monaco Editor (VS Code 同款核心)，提供代码高亮、智能补全与多主题切换。
- **全平台适配**: 响应式设计，适配 PC、平板与移动端。
- **社交化功能**: 内置讨论区、题解分享与评论系统，打造活跃的算法学习社区。

### ⚙️ 后台管理 (Admin Dashboard)

- **赛博朋克风格**: 沉浸式数据看板，实时监控系统状态。
- **全权管理**: 提供题目、公告、用户、讨论、题解、评论的一站式管理功能。
- **内容审核**: 完善的社交内容审核机制，确保社区健康。

## 🛠️ 功能概览

| 模块 | 具体功能 |
| :--- | :--- |
| **用户模块** | 登录、注册、个人主页、用户设置、排名页面、我的提交/讨论/题解查看 |
| **题目模块** | 题目列表、题目详情、题目编辑、多条件搜索、代码编辑与在线测评、提交情况饼图统计 |
| **测评记录** | 测评记录列表、测评详情对话框展示、多条件搜索 (状态/语言/题目等) |
| **公告模块** | 公告列表、公告详情展示、公告发布/编辑/删除 (管理端) |
| **讨论模块** | 讨论列表、讨论详情、发布/编辑讨论、删除讨论 |
| **题解模块** | 题解列表、题解详情、发布/编辑题解、删除题解 |
| **评论模块** | 评论列表显示、发布评论、删除评论 |
| **管理员模块** | 用户管理、题目管理、公告管理、讨论管理、题解管理、评论管理 |

## 🏗️ 技术架构

### 后端 (Backend)

- **核心语言**: C++17
- **HTTP 库**: [cpp-httplib](https://github.com/yhirose/cpp-httplib) (轻量级高并发 HTTP Server)
- **数据库**: [MongoDB](https://www.mongodb.com/) (持久化存储) & [Redis](https://redis.io/) (缓存与会话管理)
- **JSON 处理**: [jsoncpp](https://github.com/open-source-parsers/jsoncpp)
- **ID 生成**: Snowflake (分布式雪花算法)

### 前端 (Frontend)

- **框架**: [Vue 3](https://vuejs.org/) (Composition API)
- **构建工具**: [Vite](https://vitejs.dev/)
- **状态管理**: [Pinia](https://pinia.vuejs.org/)
- **路由**: [Vue Router 4](https://router.vuejs.org/)
- **组件库**: [Element Plus](https://element-plus.org/) & [Undraw UI](https://readpage.github.io/undraw-ui/)
- **图表**: ECharts 6

## 📦 快速开始

### 1. 环境准备

- **OS**: Linux (推荐 Ubuntu 20.04+)
- **Compiler**: GCC 9+ (支持 C++17)
- **Node.js**: 20.x+
- **Database**: MongoDB 5.0+, Redis 6.0+

### 2. 后端部署

```bash
cd online-judge-backend
mkdir build && cd build
cmake ..
make -j$(nproc)
# 启动服务
./bin/online-judge-backend
```

### 3. 前端部署

```bash
cd online-judge-frontend
pnpm install
# 开发环境
pnpm dev
# 生产环境打包
pnpm build
```

### 4. 数据库初始化

```bash
# 执行 MongoDB 初始化脚本
mongo online_judge ./online-judge-sql/online_judge.js
```

## 📸 项目预览

| 首页 | 题目详情 |
| :---: | :---: |
| ![首页](./doc/home_preview.png) | ![题目](./doc/problem_preview.png) |

*(注：预览图请参考 doc 目录)*

## 🤝 贡献指南

我们欢迎各种形式的贡献！无论是提交 Bug 反馈、功能建议，还是直接贡献代码，请先查阅 [CONTRIBUTING.md](CONTRIBUTING.md)。

## 📄 开源协议

本项目采用 MIT License 开源协议，详情请参阅 [LICENSE](LICENSE)。

---

<div align="center">
🙏 如果这个项目对你有帮助，欢迎点亮 ⭐️ Star、转发分享、或参与共建。

Made with ❤️ by <a href="https://github.com/mlinyun">mlinyun</a>
</div>
