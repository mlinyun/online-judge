/**
 * API 接口类型定义模块
 *
 * @description API 接口的响应类型定义
 *
 * ## 主要功能
 *
 * - 定义各类 API 响应数据结构
 * - 使用泛型支持不同数据类型
 * - 统一的接口响应格式约束
 *
 * ## 使用场景
 *
 * - API 请求响应类型约束
 * - 接口数据类型定义
 * - 响应数据解析
 *
 * @module types/api/api
 */
/** 导入基础 API 类型定义 */
import type { ApiResponse } from "@/types/common/response";

/**
 * API 命名空间定义
 */
declare namespace Api {
    /** 通用类型 */
    namespace Common {
        /** 分页查询请求参数 */
        interface PaginationParams {
            /** 页码 */
            Page: number;
            /** 每页数量 */
            PageSize: number;
        }
        /** 分页查询响应数据结构 */
        interface PaginationResponse<T> {
            /** 数据列表 */
            List: T[];
            /** 总数量 */
            Total: number;
        }

        /** 日期字符串格式：YYYY-MM-DD */
        type DateString = `${number}-${number}-${number}`;
        /** 时间字符串格式：HH:MM:SS */
        type TimeString = `${number}:${number}:${number}`;
        /** 日期时间字符串格式：YYYY-MM-DD HH:MM:SS */
        type DateTimeString = `${DateString} ${TimeString}`;

        /** 通用操作结果响应数据结构 */
        interface OperationResult {
            /** 操作结果 */
            Result: boolean;
        }
    }
    /** 用户相关类型 */
    namespace User {
        /** UserId 为 64 位雪花算法生成的 ID，用字符串表示 */
        type UserId = string;

        /** 用户注册请求参数 */
        interface UserRegisterParams {
            /** 用户昵称 */
            NickName: string;
            /** 账号 */
            Account: string;
            /** 密码 */
            PassWord: string;
            /** 个人简介 */
            PersonalProfile?: string;
            /** 学校 */
            School: string;
            /** 专业 */
            Major: string;
        }
        /** 用户注册响应数据结构 */
        interface UserRegisterResult {
            UserId: UserId;
        }
        /** 用户注册响应参数 */
        type UserRegisterResponse = ApiResponse<UserRegisterResult>;

        /** 用户登录请求参数 */
        interface UserLoginParams {
            /** 账号 */
            Account: string;
            /** 密码 */
            PassWord: string;
        }
        /** 用户登录响应数据结构 */
        interface UserLoginResult {
            /** 用户 ID */
            _id: UserId;
            /** 用户头像 */
            Avatar: string;
            /** 用户昵称 */
            NickName: string;
            /** 用户点赞的评论列表 */
            CommentLikes: string[];
            /** 用户解决的题目列表 */
            Solves: string[];
            /** 用户权限等级 */
            Authority: number;
            /** 认证 Token */
            Token: string;
        }
        /** 用户登录响应参数 */
        type UserLoginResponse = ApiResponse<UserLoginResult>;

        /** 查询用户信息响应数据结构 */
        interface SelectUserInfoResult {
            /** 用户 ID */
            _id: UserId;
            /** 用户头像 */
            Avatar: string;
            /** 用户昵称 */
            NickName: string;
            /** 个人简介 */
            PersonalProfile?: string;
            /** 学校 */
            School: string;
            /** 专业 */
            Major: string;
            /** 加入时间 */
            JoinTime: string;
            /** 用户解决的题目列表 */
            Solves: string[];
            /** 通过题目数量 */
            ACNum: number;
            /** 提交判断数量 */
            SubmitNum: number;
        }
        /** 查询用户信息响应参数 */
        type SelectUserInfoResponse = ApiResponse<SelectUserInfoResult>;

        /** 查询用户信息（在设置页面修改用户时使用）响应数据结构 */
        interface SelectUserUpdateInfoResult {
            /** 用户 ID */
            _id: UserId;
            /** 用户头像 */
            Avatar: string;
            /** 用户昵称 */
            NickName: string;
            /** 个人简介 */
            PersonalProfile?: string;
            /** 学校 */
            School: string;
            /** 专业 */
            Major: string;
        }
        /** 查询用户信息（在设置页面修改用户时使用）响应参数 */
        type SelectUserUpdateInfoResponse = ApiResponse<SelectUserUpdateInfoResult>;

        /** 更新用户信息请求参数 */
        interface UpdateUserInfoParams {
            /** 用户 ID */
            UserId: UserId;
            /** 用户头像 */
            Avatar?: string;
            /** 用户昵称 */
            NickName?: string;
            /** 个人简介 */
            PersonalProfile?: string;
            /** 学校 */
            School: string;
            /** 专业 */
            Major: string;
        }
        /** 更新用户信息响应参数 */
        type UpdateUserInfoResponse = ApiResponse<Common.OperationResult>;

        /** 删除用户响应参数 */
        type DeleteUserResponse = ApiResponse<Common.OperationResult>;

        /** 用户排名列表项 */
        interface UserRankListItem {
            /** 用户 ID */
            _id: UserId;
            /** 排名 */
            Rank: number;
            /** 用户头像 */
            Avatar: string;
            /** 用户昵称 */
            NickName: string;
            /** 个人简介 */
            PersonalProfile: string;
            /** 提交判断数量 */
            SubmitNum: number;
            /** 通过题目数量 */
            ACNum: number;
        }
        /** 用户排名列表响应数据结构 */
        type UserRankListResult = Common.PaginationResponse<UserRankListItem>;
        /** 用户排名列表响应参数 */
        type UserRankListResponse = ApiResponse<UserRankListResult>;

        /** 分页查询用户列表请求参数 */
        interface UserManageListParams extends Common.PaginationParams {
            /** 搜索信息 */
            SearchInfo?: {
                /** 用户昵称 */
                NickName?: string;
                /** 学校 */
                School?: string;
                /** 专业 */
                Major?: string;
                /** 用户账号 */
                Account?: string;
            };
        }
        /** 用户管理列表项 */
        interface UserManageListItem {
            /** 用户 ID */
            _id: UserId;
            /** 用户昵称 */
            NickName: string;
            /** 个人简介 */
            PersonalProfile: string;
            /** 学校 */
            School: string;
            /** 专业 */
            Major: string;
            /** 加入时间 */
            JoinTime: string;
        }
        /** 用户管理列表响应数据结构 */
        type UserManageListResult = Common.PaginationResponse<UserManageListItem>;
        /** 用户管理列表响应参数 */
        type UserManageListResponse = ApiResponse<UserManageListResult>;

        /** 通过 Token 登录用户响应数据结构 */
        type LoginUserByTokenResult = UserLoginResult;
        /** 通过 Token 登录用户响应参数 */
        type LoginUserByTokenResponse = ApiResponse<LoginUserByTokenResult>;

        /** 用户修改密码请求参数 */
        interface UpdateUserPasswordParams {
            /** 旧密码 */
            OldPassword: string;
            /** 新密码 */
            NewPassword: string;
        }
        /** 用户修改密码响应参数 */
        type UpdateUserPasswordResponse = ApiResponse<Common.OperationResult>;

        /** 用户退出登录响应参数 */
        type UserLogoutResponse = ApiResponse<Common.OperationResult>;

        /** 简单用户信息 */
        interface SimpleUserInfo {
            /** 用户 ID */
            _id: UserId;
            /** 用户昵称 */
            NickName: string;
            /** 用户头像 */
            Avatar: string;
        }
    }
    /** 标签相关类型 */
    namespace Tag {
        /** 标签列表 */
        type Tags = string[];

        /** 获取所有题目标签请求参数 */
        interface GetProblemTagsParams {
            TagType: "Problem";
        }
        /** 获取所有题目标签响应数据结构 */
        interface GetProblemTagsResult {
            /** 题目标签列表 */
            Tags: Tags;
        }
        /** 获取所有题目标签响应参数 */
        type GetProblemTagsResponse = ApiResponse<GetProblemTagsResult>;
    }

    /** 题目相关类型 */
    namespace Problem {
        /** ProblemId 为 64 位雪花算法生成的 ID，用字符串表示 */
        type ProblemId = string;

        /** 查询题目信息（单条）响应数据结构 */
        interface SelectProblemResult {
            /** 题目 ID */
            _id: ProblemId;
            /** 题目标题 */
            Title: string;
            /** 题目描述 */
            Description: string;
            /** 时间限制（毫秒） */
            TimeLimit: number;
            /** 内存限制（兆字节） */
            MemoryLimit: number;
            /** 测试用例数目 */
            JudgeNum: number;
            /** 提交数量 */
            SubmitNum: number;
            /** 通过数量 */
            ACNum: number;
            /** 题目创建者昵称 */
            UserNickName: string;
            /** 题目标签列表 */
            Tags: Tag.Tags;
        }
        /** 查询题目信息（单条）响应参数 */
        type SelectProblemResponse = ApiResponse<SelectProblemResult>;

        /** 测试信息 */
        interface TestInfo {
            /** 测试输入 */
            Input: string;
            /** 测试输出 */
            Output: string;
        }
        /** 查询题目信息（单条，管理员权限）响应数据结构 */
        interface SelectProblemByAdminResult {
            /** 题目 ID */
            _id: ProblemId;
            /** 题目标题 */
            Title: string;
            /** 题目描述 */
            Description: string;
            /** 题目创建者昵称 */
            UserNickName: string;
            /** 时间限制（毫秒） */
            TimeLimit: number;
            /** 内存限制（兆字节） */
            MemoryLimit: number;
            /** 测试用例数目 */
            JudgeNum: number;
            /** 题目标签列表 */
            Tags: Tag.Tags;
            /** 特判标志 */
            IsSPJ?: boolean;
            /** 特判代码 */
            SPJ?: string;
            /** 测试信息列表 */
            TestInfo: TestInfo[];
        }
        /** 查询题目信息（单条，管理员权限）响应参数 */
        type SelectProblemByAdminResponse = ApiResponse<SelectProblemByAdminResult>;

        /** 题目数据结构 */
        interface ProblemData {
            /** 题目 ID */
            ProblemId: ProblemId;
            /** 题目标题 */
            Title: string;
            /** 题目描述 */
            Description: string;
            /** 时间限制（毫秒） */
            TimeLimit: number;
            /** 内存限制（兆字节） */
            MemoryLimit: number;
            /** 测试用例数目 */
            JudgeNum: number;
            /** 题目标签列表 */
            Tags: Tag.Tags;
            /** 特判标志 */
            IsSPJ: boolean;
            /** 特判代码 */
            SPJ?: string;
            /** 测试信息列表 */
            TestInfo: TestInfo[];
        }
        /** 编辑题目请求参数 */
        interface EditProblemParams {
            /** 编辑题目类型 */
            EditType: "Insert" | "Update";
            /** 题目数据 */
            ProblemData: ProblemData;
        }
        /** 编辑题目响应数据结构 */
        interface EditProblemResult {
            /** 插入成功返回的题目 ID */
            ProblemId?: ProblemId;
            /** 更新结果 */
            Result?: boolean;
        }
        /** 编辑题目响应参数 */
        type EditProblemResponse = ApiResponse<EditProblemResult>;

        /** 删除题目响应参数 */
        type DeleteProblemResponse = ApiResponse<Common.OperationResult>;

        /** 分页获取题目列表请求参数 */
        interface SelectProblemListParams extends Common.PaginationParams {
            /** 搜索信息 */
            SearchInfo?: {
                /** 题目 ID */
                Id?: ProblemId;
                /** 题目标题 */
                Title?: string;
                /** 题目标签列表 */
                Tags?: Tag.Tags;
            };
        }
        /** 分页获取题目列表请求参数（管理员权限） */
        interface SelectProblemListByAdminParams extends Common.PaginationParams {
            /** 搜索信息 */
            SearchInfo?: {
                /** 题目 ID */
                Id?: ProblemId;
                /** 题目标题 */
                Title?: string;
                /** 题目标签列表 */
                Tags?: Tag.Tags;
                /** 题目创建者昵称 */
                UserNickName?: string;
            };
        }
        /** 题目列表项数据结构 */
        interface SelectProblemListItem {
            /** 题目 ID */
            _id: ProblemId;
            /** 题目标题 */
            Title: string;
            /** 提交数量 */
            SubmitNum: number;
            /** 通过数量 */
            ACNum: number;
            /** 编译错误数量 */
            CENum: number;
            /** 错误答案数量 */
            WANum: number;
            /** 运行错误数量 */
            RENum: number;
            /** 超时数量 */
            TLENum: number;
            /** 内存超限数量 */
            MLENum: number;
            /** 系统错误数量 */
            SENum: number;
            /** 题目标签列表 */
            Tags: Tag.Tags;
        }
        /** 分页获取题目列表响应数据结构 */
        type SelectProblemListResult = Common.PaginationResponse<SelectProblemListItem>;
        /** 分页获取题目列表响应参数 */
        type SelectProblemListResponse = ApiResponse<SelectProblemListResult>;
    }

    /** 公告相关类型 */
    namespace Announcement {
        /** AnnouncementId 为 64 位雪花算法生成的 ID，用字符串表示 */
        type AnnouncementId = string;

        /** 添加公告请求参数 */
        interface InsertAnnouncementParams {
            /** 公告标题 */
            Title: string;
            /** 公告内容 */
            Content: string;
            /** 公告等级 */
            Level: number;
            /** 公告是否激活 */
            Active: boolean;
        }
        /** 添加公告响应数据结构 */
        interface InsertAnnouncementResult {
            /** 添加成功返回的公告 ID */
            AnnouncementId: AnnouncementId;
        }
        /** 添加公告响应参数 */
        type InsertAnnouncementResponse = ApiResponse<InsertAnnouncementResult>;

        /** 查询公告详细信息响应数据结构 */
        interface SelectAnnouncementResult {
            /** 公告 ID */
            _id: AnnouncementId;
            /** 公告标题 */
            Title: string;
            /** 公告内容 */
            Content: string;
            /** 浏览量 */
            Views: number;
            /** 评论数 */
            Comments: number;
            /** 创建时间 */
            CreateTime: Common.DateTimeString;
            /** 更新时间 */
            UpdateTime: Common.DateTimeString;
        }
        /** 查询公告详细信息响应参数 */
        type SelectAnnouncementResponse = ApiResponse<SelectAnnouncementResult>;

        /** 查询公告详细信息（编辑）响应数据结构 */
        interface SelectAnnouncementByEditResult {
            /** 公告 ID */
            _id: AnnouncementId;
            /** 公告标题 */
            Title: string;
            /** 公告内容 */
            Content: string;
            /** 公告等级 */
            Level: number;
            /** 公告是否激活 */
            Active: boolean;
        }
        /** 查询公告详细信息（编辑）响应参数 */
        type SelectAnnouncementByEditResponse = ApiResponse<SelectAnnouncementByEditResult>;

        /** 更新公告请求参数 */
        interface UpdateAnnouncementParams {
            /** 公告 ID */
            AnnouncementId: AnnouncementId;
            /** 公告标题 */
            Title: string;
            /** 公告内容 */
            Content: string;
            /** 公告等级 */
            Level: number;
            /** 公告是否激活 */
            Active: boolean;
        }
        /** 更新公告响应参数 */
        type UpdateAnnouncementResponse = ApiResponse<Common.OperationResult>;

        /** 删除公告响应参数 */
        type DeleteAnnouncementResponse = ApiResponse<Common.OperationResult>;

        /** 分页查询公告列表项 */
        interface SelectAnnouncementListItem {
            /** 公告 ID */
            _id: AnnouncementId;
            /** 公告标题 */
            Title: string;
            /** 浏览量 */
            Views: number;
            /** 评论数 */
            Comments: number;
            /** 公告等级 */
            Level: number;
            /** 创建时间 */
            CreateTime: Common.DateTimeString;
        }
        /** 分页查询公告列表响应数据结构 */
        type SelectAnnouncementListResult = Common.PaginationResponse<SelectAnnouncementListItem>;
        /** 分页查询公告列表响应参数 */
        type SelectAnnouncementListResponse = ApiResponse<SelectAnnouncementListResult>;

        /** 分页获取公告列表（管理员权限）请求参数 */
        interface SelectAnnouncementListByAdminParams extends Common.PaginationParams {
            /** 搜索信息 */
            SearchInfo?: {
                /** 公告标题 */
                Title?: string;
                /** 公告等级 */
                Level?: number;
                /** 公告是否激活 */
                Active?: boolean;
            };
        }
        /** 分页获取公告列表（管理员权限）列表项 */
        interface SelectAnnouncementListByAdminItem extends SelectAnnouncementListItem {
            /** 公告是否激活 */
            Active: boolean;
        }
        /** 分页获取公告列表（管理员权限）响应数据结构 */
        type SelectAnnouncementListByAdminResult = Common.PaginationResponse<SelectAnnouncementListByAdminItem>;
        /** 分页获取公告列表（管理员权限）响应参数 */
        type SelectAnnouncementListByAdminResponse = ApiResponse<SelectAnnouncementListByAdminResult>;

        /** 设置公告激活状态请求参数 */
        interface UpdateAnnouncementActiveParams {
            /** 公告 ID */
            AnnouncementId: AnnouncementId;
            /** 公告是否激活 */
            Active: boolean;
        }
        /** 设置公告激活状态响应参数 */
        type UpdateAnnouncementActiveResponse = ApiResponse<Common.OperationResult>;
    }

    /** 讨论相关类型 */
    namespace Discuss {
        /** DiscussId 为 64 位雪花算法生成的 ID，用字符串表示 */
        type DiscussId = string;

        /** 添加讨论请求参数 */
        interface InsertDiscussParams {
            /** 讨论标题 */
            Title: string;
            /** 讨论内容 */
            Content: string;
            /** 父 ID，0 或者题目 ID */
            ParentId: string;
        }
        /** 添加讨论响应数据结构 */
        interface InsertDiscussResult {
            /** 添加成功返回的讨论 ID */
            DiscussId: DiscussId;
        }
        /** 添加讨论响应参数 */
        type InsertDiscussResponse = ApiResponse<InsertDiscussResult>;

        /** 查询讨论的详细内容响应数据结构 */
        interface SelectDiscussResult {
            /** 讨论 ID */
            _id: DiscussId;
            /** 讨论标题 */
            Title: string;
            /** 讨论内容 */
            Content: string;
            /** 浏览量 */
            Views: number;
            /** 评论数 */
            Comments: number;
            /** 创建时间 */
            CreateTime: Common.DateTimeString;
            /** 更新时间 */
            UpdateTime: Common.DateTimeString;
            /** 讨论用户信息 */
            User: User.SimpleUserInfo;
        }
        /** 查询讨论的详细内容响应参数 */
        type SelectDiscussResponse = ApiResponse<SelectDiscussResult>;

        /** 查询讨论的详细信息（编辑）响应数据结构 */
        interface SelectDiscussByEditResult {
            /** 讨论 ID */
            _id: DiscussId;
            /** 讨论标题 */
            Title: string;
            /** 讨论内容 */
            Content: string;
            /** 讨论用户 ID */
            UserId: User.UserId;
        }
        /** 查询讨论的详细信息（编辑）响应参数 */
        type SelectDiscussByEditResponse = ApiResponse<SelectDiscussByEditResult>;

        /** 更新讨论请求参数 */
        interface UpdateDiscussParams {
            /** 讨论 ID */
            DiscussId: DiscussId;
            /** 讨论标题 */
            Title: string;
            /** 讨论内容 */
            Content: string;
        }
        /** 更新讨论响应参数 */
        type UpdateDiscussResponse = ApiResponse<Common.OperationResult>;

        /** 删除讨论响应参数 */
        type DeleteDiscussResponse = ApiResponse<Common.OperationResult>;

        /** 分页查询讨论请求参数 */
        interface SelectDiscussListParams extends Common.PaginationParams {
            /** 搜索信息 */
            SearchInfo?: {
                /** 父 ID，0 或者题目 ID */
                ParentId?: DiscussId;
                /** 用户 ID */
                UserId?: User.UserId;
            };
        }
        /** 分页查询讨论列表（管理员权限）请求参数 */
        interface SelectDiscussListByAdminParams extends Common.PaginationParams {
            /** 搜索信息 */
            SearchInfo?: {
                /** 讨论标题 */
                Title?: string;
                /** 父 ID，0 或者题目 ID */
                ParentId?: Problem.ProblemId | "0";
                /** 用户 ID */
                UserId?: User.UserId;
            };
        }
        /** 分页查询讨论列表项 */
        interface SelectDiscussListItem {
            /** 讨论 ID */
            _id: DiscussId;
            /** 讨论标题 */
            Title: string;
            /** 父 ID，0 或者题目 ID */
            ParentId: Problem.ProblemId | "0";
            /** 浏览量 */
            Views: number;
            /** 评论数 */
            Comments: number;
            /** 创建时间 */
            CreateTime: Common.DateTimeString;
            /** 讨论用户信息 */
            User: User.SimpleUserInfo;
        }
        /** 分页查询讨论列表响应数据结构 */
        type SelectDiscussListResult = Common.PaginationResponse<SelectDiscussListItem>;
        /** 分页查询讨论列表响应参数 */
        type SelectDiscussListResponse = ApiResponse<SelectDiscussListResult>;
    }

    /** 题解相关类型 */
    namespace Solution {
        /** SolutionId 为 64 位雪花算法生成的 ID，用字符串表示 */
        type SolutionId = string;

        /** 添加题解请求参数 */
        interface InsertSolutionParams {
            /** 题解标题 */
            Title: string;
            /** 题解内容 */
            Content: string;
            /** 关联的题目 ID */
            ParentId: Problem.ProblemId;
            /** 公开标志 */
            Public: boolean;
        }
        /** 添加题解响应数据结构 */
        interface InsertSolutionResult {
            /** 添加成功返回的题解 ID */
            SolutionId: SolutionId;
        }
        /** 添加题解响应参数 */
        type InsertSolutionResponse = ApiResponse<InsertSolutionResult>;

        /** 查询题解响应数据结构 */
        interface SelectSolutionResult {
            /** 题解 ID */
            _id: SolutionId;
            /** 题解标题 */
            Title: string;
            /** 题解内容 */
            Content: string;
            /** 浏览量 */
            Views: number;
            /** 评论数 */
            Comments: number;
            /** 创建时间 */
            CreateTime: Common.DateTimeString;
            /** 更新时间 */
            UpdateTime: string;
            /** 题解用户信息 */
            User: User.SimpleUserInfo;
        }
        /** 查询题解响应参数 */
        type SelectSolutionResponse = ApiResponse<SelectSolutionResult>;

        /** 查询题解的详细信息（编辑）响应数据结构 */
        interface SelectSolutionByEditResult {
            /** 题解 ID */
            _id: SolutionId;
            /** 题解标题 */
            Title: string;
            /** 题解内容 */
            Content: string;
            /** 关联的题目 ID */
            ParentId: Problem.ProblemId;
            /** 公开标志 */
            Public: boolean;
            /** 题解用户 ID */
            UserId: User.UserId;
        }
        /** 查询题解的详细信息（编辑）响应参数 */
        type SelectSolutionByEditResponse = ApiResponse<SelectSolutionByEditResult>;

        /** 更新题解请求参数 */
        interface UpdateSolutionParams {
            /** 题解 ID */
            SolutionId: SolutionId;
            /** 题解标题 */
            Title: string;
            /** 题解内容 */
            Content: string;
            /** 公开标志 */
            Public: boolean;
        }
        /** 更新题解响应参数 */
        type UpdateSolutionResponse = ApiResponse<Common.OperationResult>;

        /** 删除题解响应参数 */
        type DeleteSolutionResponse = ApiResponse<Common.OperationResult>;

        /** 分页查询题解请求参数 */
        interface SelectSolutionListParams extends Common.PaginationParams {
            /** 搜索信息 */
            SearchInfo?: {
                /** 父 ID，题目 ID */
                ParentId?: Problem.ProblemId;
                /** 用户 ID */
                UserId?: User.UserId;
            };
        }
        /** 分页查询题解列表项 */
        interface SelectSolutionListItem {
            /** 题解 ID */
            _id: SolutionId;
            /** 题解标题 */
            Title: string;
            /** 浏览量 */
            Views: number;
            /** 评论数 */
            Comments: number;
            /** 创建时间 */
            CreateTime: Common.DateTimeString;
            /** 题解用户信息 */
            User: User.SimpleUserInfo;
        }
        /** 分页查询题解列表响应数据结构 */
        type SelectSolutionListResult = Common.PaginationResponse<SelectSolutionListItem>;
        /** 分页查询题解列表响应参数 */
        type SelectSolutionListResponse = ApiResponse<SelectSolutionListResult>;

        /** 分页查询题解列表（管理员权限）请求参数 */
        interface SelectSolutionListByAdminParams extends Common.PaginationParams {
            /** 搜索信息 */
            SearchInfo?: {
                /** 题解标题 */
                Title?: string;
                /** 父 ID，题目 ID */
                ParentId?: Problem.ProblemId;
                /** 用户 ID */
                UserId?: User.UserId;
                /** 公开标志 */
                Public?: boolean;
            };
        }
        /** 分页查询题解列表（管理员权限）列表项 */
        interface SelectSolutionListByAdminItem extends SelectSolutionListItem {
            /** 公开标志 */
            Public: boolean;
        }
        /** 分页查询题解列表（管理员权限）响应数据结构 */
        type SelectSolutionListByAdminResult = Common.PaginationResponse<SelectSolutionListByAdminItem>;
        /** 分页查询题解列表（管理员权限）响应参数 */
        type SelectSolutionListByAdminResponse = ApiResponse<SelectSolutionListByAdminResult>;
    }

    /** 评论相关类型 */
    namespace Comment {
        /** CommentId 为 64 位雪花算法生成的 ID，用字符串表示 */
        type CommentId = string;

        /** 添加评论请求参数 */
        interface InsertCommentParams {
            /** 父 ID（可以是公告 ID、讨论 ID、题解 ID 或是评论 ID） */
            ParentId: Announcement.AnnouncementId | Discuss.DiscussId | Solution.SolutionId | Comment.CommentId;
            /** 父类型 */
            ParentType: "Announcement" | "Discuss" | "Solution" | "Comment";
            /** 评论类型（父评论，回复） */
            CommentType: "Father" | "Reply";
            /** 评论内容 */
            Content: string;
        }
        /** 添加评论响应数据结构 */
        interface InsertCommentResult {
            /** 添加成功返回的评论 ID */
            CommentId: CommentId;
            /** 创建时间 */
            CreateTime: Common.DateTimeString;
        }
        /** 添加评论响应参数 */
        type InsertCommentResponse = ApiResponse<InsertCommentResult>;

        /** 删除评论响应参数 */
        type DeleteCommentResponse = ApiResponse<Common.OperationResult>;

        /** 分页获取评论列表请求参数 */
        // Page, PageSize, ParentId, SonNum
        interface SelectCommentListParams extends Common.PaginationParams {
            /** 父 ID（可以是公告 ID、讨论 ID 或是题解 ID） */
            ParentId: Announcement.AnnouncementId | Discuss.DiscussId | Solution.SolutionId;
            /** 评论类型 */
            CommentType: "Father" | "Reply";
            /** 子评论数量限制，仅在查询父评论时使用 */
            SonNum?: number;
        }
        /** 子评论列表项 */
        interface ChildCommentListItem {
            /** 评论 ID */
            _id: CommentId;
            /** 评论用户信息 */
            User: User.SimpleUserInfo;
            /** 评论内容 */
            Content: string;
            /** 点赞数量 */
            Likes: number;
            /** 创建时间：评论时间 */
            CreateTime: Common.DateTimeString;
        }
        /** 评论列表项 */
        interface SelectCommentListItem {
            /** 评论 ID */
            _id: CommentId;
            /** 评论用户信息 */
            User: User.SimpleUserInfo;
            /** 父级 ID */
            ParentId: Announcement.AnnouncementId | Discuss.DiscussId | Solution.SolutionId;
            /** 评论内容 */
            Content: string;
            /** 点赞数量 */
            Likes: number;
            /** 创建时间：评论时间 */
            CreateTime: Common.DateTimeString;
            /** 子评论列表，仅在查询父评论时返回 */
            Child_Comments?: ChildCommentListItem[];
            /** 子评论数量，仅在查询父评论时返回 */
            Child_Total?: number;
        }
        /** 分页获取评论列表响应数据结构 */
        type SelectCommentListResult = Common.PaginationResponse<SelectCommentListItem>;
        /** 分页获取评论列表响应参数 */
        type SelectCommentListResponse = ApiResponse<SelectCommentListResult>;

        /** 分页获取评论列表（管理员权限）请求参数 */
        interface SelectCommentListByAdminParams extends Common.PaginationParams {
            /** 搜索信息 */
            SearchInfo?: {
                /** 父类型 */
                ParentType?: "Announcement" | "Discuss" | "Solution";
                /** 评论内容 */
                Content?: string;
                /** 评论用户ID */
                UserId?: User.UserId;
            };
        }
        /** 子评论列表项（管理员权限） */
        interface ChildCommentListByAdminItem {
            /** 评论 ID */
            _id: CommentId;
            /** 评论内容 */
            Content: string;
            /** 评论用户信息 */
            User: User.SimpleUserInfo;
            /** 创建时间：评论时间 */
            CreateTime: Common.DateTimeString;
        }
        /** 评论列表项（管理员权限） */
        interface SelectCommentListByAdminItem {
            /** 评论 ID */
            _id: CommentId;
            /** 评论用户信息 */
            User: User.SimpleUserInfo;
            /** 父级 ID */
            ParentId: Announcement.AnnouncementId | Discuss.DiscussId | Solution.SolutionId;
            /** 父类型 */
            ParentType: "Announcement" | "Discuss" | "Solution";
            /** 评论内容 */
            Content: string;
            /** 点赞数量 */
            Likes: number;
            /** 创建时间：评论时间 */
            CreateTime: Common.DateTimeString;
            /** 子评论列表 */
            Child_Comments: ChildCommentListByAdminItem[];
            /** 子评论数量 */
            Child_Total: number;
        }
        /** 分页获取评论列表（管理员权限）响应数据结构 */
        type SelectCommentListByAdminResult = Common.PaginationResponse<SelectCommentListByAdminItem>;
        /** 分页获取评论列表（管理员权限）响应参数 */
        type SelectCommentListByAdminResponse = ApiResponse<SelectCommentListByAdminResult>;
    }

    /** 状态相关类型 */
    namespace Status {
        /** 状态记录 ID */
        type StatusRecordId = string;
        /** 测试信息列表项 */
        interface TestInfoItem {
            /** 运行时间 */
            RunTime: string;
            /** 运行内存 */
            RunMemory: string;
            /** 状态 */
            Status: string;
            /** 标准输入 */
            StandardInput: string;
            /** 标准输出 */
            StandardOutput: string;
            /** 用户输出 */
            PersonalOutput: string;
        }

        /** 查询一条状态记录响应数据结构 */
        interface SelectStatusRecordResult {
            /** 状态记录 ID */
            _id: StatusRecordId;
            /** 状态 */
            Status: string;
            /** 编程语言 */
            Language: string;
            /** 代码 */
            Code: string;
            /** 编译器信息 */
            CompilerInfo: string;
            /** 测试信息 */
            TestInfo: TestInfoItem[];
        }
        /** 查询一条状态记录响应参数 */
        type SelectStatusRecordResponse = ApiResponse<SelectStatusRecordResult>;

        /** 分页获取状态记录列表请求参数 */
        interface SelectStatusRecordListParams extends Common.PaginationParams {
            /** 搜索信息 */
            SearchInfo?: {
                /** 用户 ID */
                UserId?: User.UserId;
                /** 题目 ID */
                ProblemId?: Problem.ProblemId;
                /** 题目标题 */
                ProblemTitle?: string;
                /** 状态 */
                Status?: number;
                /** 编程语言 */
                Language?: string;
            };
        }
        /** 状态记录列表项 */
        interface SelectStatusRecordListItem {
            /** 状态记录 ID */
            _id: StatusRecordId;
            /** 用户 ID */
            UserId: User.UserId;
            /** 用户昵称 */
            UserNickName: string;
            /** 题目 ID */
            ProblemId: Problem.ProblemId;
            /** 题目标题 */
            ProblemTitle: string;
            /** 状态 */
            Status: number;
            /** 提交时间 */
            SubmitTime: Common.DateTimeString;
            /** 编程语言 */
            Language: string;
            /** 运行时间 */
            RunTime: string;
            /** 运行内存 */
            RunMemory: string;
            /** 代码长度 */
            Length: string;
        }

        /** 分页获取状态记录列表响应数据结构 */
        type SelectStatusRecordListResult = Common.PaginationResponse<SelectStatusRecordListItem>;
        /** 分页获取状态记录列表响应参数 */
        type SelectStatusRecordListResponse = ApiResponse<SelectStatusRecordListResult>;
    }

    /** 判题相关类型 */
    namespace Judge {
        /** 提交判题请求参数 */
        interface SubmitJudgeCodeParams {
            /** 题目 ID */
            ProblemId: Problem.ProblemId;
            /** 编程语言 */
            Language: string;
            /** 代码 */
            Code: string;
        }

        /** 提交判题响应数据结构 */
        interface SubmitJudgeCodeResult {
            /** 提交成功返回的状态记录 ID */
            StatusRecordId: Status.StatusRecordId;
            /** 编译器信息 */
            CompilerInfo: string;
            /** 判题结果 */
            Status: number;
            /** 是否第一次通过 */
            IsFirstAC: boolean;
        }
        /** 提交判题响应参数 */
        type SubmitJudgeCodeResponse = ApiResponse<SubmitJudgeCodeResult>;
    }
}
