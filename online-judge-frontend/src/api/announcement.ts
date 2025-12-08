/**
 * 公告模块 API 接口
 *
 * @description 包含公告的增删改查等接口
 *
 * ## 主要功能
 * - 添加公告（管理员权限）
 * - 查询公告详细信息
 * - 更新公告（管理员权限）
 * - 删除公告（管理员权限）
 * - 分页获取公告列表
 *
 * @module api/announcement
 */

import { get, post, del } from "@/utils/http";
import type { AxiosResponse } from "axios";
import type { Api } from "@/types/api/api";

/**
 * 添加公告（管理员权限）
 * @name insertAnnouncement
 * @tags 公告模块
 * @description 添加新的公告
 * @request POST `/admin/announcement/insert`
 * @param data 公告信息，详见 {@link Api.Announcement.InsertAnnouncementParams}
 * @returns 新增结果，详见 {@link Api.Announcement.InsertAnnouncementResponse}
 */
export const insertAnnouncement = (
    data: Api.Announcement.InsertAnnouncementParams
): Promise<AxiosResponse<Api.Announcement.InsertAnnouncementResponse>> => {
    return post<Api.Announcement.InsertAnnouncementResult>("/admin/announcement/insert", data, {
        showSuccessMessage: true,
        successMessage: "公告添加成功！",
    });
};

/**
 * 查询公告详细信息
 * @name getAnnouncement
 * @tags 公告模块
 * @description 查询公告详细信息，并将其浏览量加 1
 * @request GET `/announcement/info`
 * @param announcementId 公告 ID，详见 {@link Api.Announcement.AnnouncementId}
 * @returns 公告详细信息，详见 {@link Api.Announcement.SelectAnnouncementResponse}
 */
export const getAnnouncement = (
    announcementId: Api.Announcement.AnnouncementId
): Promise<AxiosResponse<Api.Announcement.SelectAnnouncementResponse>> => {
    return get<Api.Announcement.SelectAnnouncementResult>("/announcement/info", {
        AnnouncementId: announcementId,
    });
};

/**
 * 查询公告详细信息（编辑，管理员权限）
 * @name selectAnnouncementByEdit
 * @tags 公告模块
 * @description 查询公告的详细信息，主要是编辑时的查询
 * @request GET `/admin/announcement/info`
 * @param announcementId 公告 ID，详见 {@link Api.Announcement.AnnouncementId}
 * @returns 公告详细信息，详见 {@link Api.Announcement.SelectAnnouncementByEditResponse}
 */
export const selectAnnouncementByEdit = (
    announcementId: Api.Announcement.AnnouncementId
): Promise<AxiosResponse<Api.Announcement.SelectAnnouncementByEditResponse>> => {
    return get<Api.Announcement.SelectAnnouncementByEditResult>("/admin/announcement/info", {
        AnnouncementId: announcementId,
    });
};

/**
 * 更新公告（管理员权限）
 * @name updateAnnouncement
 * @tags 公告模块
 * @description 更新公告信息
 * @request POST `/admin/announcement/update`
 * @param data 公告更新信息，详见 {@link Api.Announcement.UpdateAnnouncementParams}
 * @returns 更新结果，详见 {@link Api.Announcement.UpdateAnnouncementResponse}
 */
export const updateAnnouncement = (
    data: Api.Announcement.UpdateAnnouncementParams
): Promise<AxiosResponse<Api.Announcement.UpdateAnnouncementResponse>> => {
    return post<Api.Common.OperationResult>("/admin/announcement/update", data, {
        showSuccessMessage: true,
        successMessage: "公告更新成功！",
    });
};

/**
 * 删除公告（管理员权限）
 * @name deleteAnnouncement
 * @tags 公告模块
 * @description 删除指定公告
 * @request DELETE `/admin/announcement/delete`
 * @param announcementId 公告 ID，详见 {@link Api.Announcement.AnnouncementId}
 * @returns 删除结果，详见 {@link Api.Announcement.DeleteAnnouncementResponse}
 */
export const deleteAnnouncement = (
    announcementId: Api.Announcement.AnnouncementId
): Promise<AxiosResponse<Api.Announcement.DeleteAnnouncementResponse>> => {
    return del<Api.Common.OperationResult>(
        "/admin/announcement/delete",
        {
            AnnouncementId: announcementId,
        },
        {
            showSuccessMessage: true,
            successMessage: "公告删除成功！",
        }
    );
};

/**
 * 分页获取公告列表
 * @name selectAnnouncementList
 * @tags 公告模块
 * @description 分页获取公告列表
 * @request GET `/announcement/list`
 * @param params 分页查询参数，详见 {@link Api.Common.PaginationParams}
 * @returns 公告列表，详见 {@link Api.Announcement.SelectAnnouncementListResponse}
 */
export const selectAnnouncementList = (
    params: Api.Common.PaginationParams
): Promise<AxiosResponse<Api.Announcement.SelectAnnouncementListResponse>> => {
    return get<Api.Announcement.SelectAnnouncementListResult>("/announcement/list", {
        ...params,
    });
};

/**
 * 分页获取公告列表（管理员权限）
 * @name selectAnnouncementListByAdmin
 * @tags 公告模块
 * @description 管理员分页获取公告列表
 * @request GET `/admin/announcement/list`
 * @param params 分页查询参数，详见 {@link Api.Common.PaginationParams}
 * @returns 公告列表，详见 {@link Api.Announcement.SelectAnnouncementListResponse}
 */
export const selectAnnouncementListByAdmin = (
    params: Api.Common.PaginationParams
): Promise<AxiosResponse<Api.Announcement.SelectAnnouncementListResponse>> => {
    return get<Api.Announcement.SelectAnnouncementListResult>("/admin/announcement/list", {
        ...params,
    });
};
