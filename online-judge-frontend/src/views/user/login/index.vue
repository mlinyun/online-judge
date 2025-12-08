<script setup lang="ts">
defineOptions({ name: "UserLogin" });
import { userLogin } from "@/api/user";
import type { Api } from "@/types/api/api";
import { useUserStore } from "@/stores/modules/user";
import { useRoute, useRouter } from "vue-router";

const route = useRoute();
const router = useRouter();

/** 用户状态管理 */
const userStore = useUserStore();

/**
 * 用户登录参数
 */
const userLoginParams: Api.User.UserLoginParams = {
    Account: "LingYunAdmin",
    PassWord: "123456",
};

// /**
//  * 用户信息
//  */
// const userInfo = reactive<Api.User.UserLoginResult>({
//     _id: "",
//     NickName: "",
//     Avatar: "",
//     CommentLikes: [],
//     Solves: [],
//     Authority: 0,
//     Token: "",
// });

/**
 * 表格数据
 */
const tableData = ref<Api.User.UserLoginResult[]>([]);

/**
 * 用户登录处理函数
 */
const handleLogin = async () => {
    try {
        const response = await userLogin(userLoginParams);
        // 判断登录是否成功
        if (response.data.success && response.data.data) {
            // 获取用户信息
            const userInfoData = response.data.data;
            // 更新用户状态管理
            userStore.setUserInfo(userInfoData);
            userStore.setIsLoggedIn(true);
            userStore.setAccessToken(userInfoData.Token);
            // 更新本地用户信息
            // Object.assign(userInfo, userInfoData);
            // 更新表格数据以显示用户信息
            tableData.value = [userInfoData];

            // 静止 3s 后跳转到首页
            setTimeout(() => {
                // 获取 redirect 查询参数，如果存在则跳转到指定页面，否则跳转到首页
                const redirect = route.query.redirect as string;
                router.push(redirect || "/");
            }, 3000);
        }
    } catch (error) {
        console.error("Login failed:", error);
    }
};
</script>

<template>
    <div class="user-login">
        <h1>UserLogin</h1>
        <!-- 提供一个按钮绑定 -->
        <el-button type="primary" @click="handleLogin">Login</el-button>

        <br /><br />

        <!-- 显示用户信息 -->
        <el-table :data="tableData" :style="{ width: '100%' }">
            <el-table-column prop="_id" label="User ID" width="180" />
            <el-table-column prop="NickName" label="Nickname" width="180" />
            <el-table-column prop="Avatar" label="Avatar" width="180" />
            <el-table-column prop="Authority" label="Authority" width="120" />
            <el-table-column prop="Token" label="Token" />
        </el-table>
    </div>
</template>

<style scoped></style>
