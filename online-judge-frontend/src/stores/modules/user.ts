/**
 * 用户状态管理模块
 *
 * @description 管理用户相关的状态信息，包括登录状态、用户信息等
 *
 * @module stores/modules/user
 */
import { defineStore } from "pinia";
import type { Api } from "@/types/api/api";
import { UserAuthority } from "@/constants/user";
import { StorageConfig } from "@/utils/storage/storage-config";
import router from "@/router";

export const useUserStore = defineStore(
    "userStore",
    () => {
        // 用户信息状态
        const userInfo = ref<Api.User.UserLoginResult>({
            _id: "0",
            Avatar: "",
            NickName: "",
            CommentLikes: [],
            Solves: [],
            Authority: 1,
            Token: "",
        });
        // 登录状态
        const isLoggedIn = ref<boolean>(false);
        // 访问令牌
        const accessToken = ref<string>("");

        // 计算属性：获取用户信息
        const getUserInfo = computed(() => userInfo.value);
        // 计算属性：获取登录状态
        const getIsLoggedIn = computed(() => isLoggedIn.value);
        // 计算属性：获取访问令牌
        const getAccessToken = computed(() => accessToken.value);
        // 计算属性：是否是管理员
        const isAdmin = computed(() => {
            return userInfo.value.Authority === UserAuthority.ADMINISTRATOR;
        });

        /**
         * 设置用户信息
         * @param newInfo 新的用户信息
         */
        const setUserInfo = (newInfo: Api.User.UserLoginResult) => {
            userInfo.value = newInfo;
        };

        /**
         * 设置登录状态
         * @param loggedIn 新的登录状态
         */
        const setIsLoggedIn = (loggedIn: boolean) => {
            isLoggedIn.value = loggedIn;
        };

        /**
         * 设置访问令牌
         * @param token 新的访问令牌
         */
        const setAccessToken = (token: string) => {
            accessToken.value = token;
        };

        /**
         * 处理用户登出
         * @description 清除用户状态，保存当前用户 ID 以便下次登录检查，并跳转到登录页
         */
        const handleLogout = () => {
            // 保存当前用户 ID，以便下次登录时检查是否为同一用户
            const currentUserId: string = userInfo.value._id || "0";
            if (currentUserId && currentUserId != "0") {
                localStorage.setItem(StorageConfig.LAST_USER_ID_KEY, currentUserId);
            }
            // 清除状态
            userInfo.value = {
                _id: "0",
                Avatar: "",
                NickName: "",
                CommentLikes: [],
                Solves: [],
                Authority: 1,
                Token: "",
            };
            isLoggedIn.value = false;
            accessToken.value = "";
            // 跳转到登录页，携带当前路由作为 redirect 参数
            const currentRoute = router.currentRoute.value;
            const redirect = currentRoute.path !== "/auth/login" ? currentRoute.fullPath : undefined;
            router.push({ name: "auth-login", query: redirect ? { redirect } : {} });
        };

        return {
            userInfo,
            isLoggedIn,
            accessToken,
            getUserInfo,
            getIsLoggedIn,
            getAccessToken,
            isAdmin,
            setUserInfo,
            setIsLoggedIn,
            setAccessToken,
            handleLogout,
        };
    },
    {
        persist: { key: "user", storage: localStorage },
    }
);
