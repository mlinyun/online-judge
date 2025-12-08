<script setup lang="ts">
defineOptions({ name: "HomeView" });
import { useUserStore } from "@/stores/modules/user";

const userStore = useUserStore();

const accessTokenState = userStore.accessToken;
const accessTokenGetter = userStore.getAccessToken;
const accessTokenComputed = computed(() => userStore.getAccessToken);
</script>

<template>
    <div class="home-view">
        <h1>欢迎来到凌云在线编程评测系统</h1>
        <p class="placeholder-text">AccessToken (state): {{ accessTokenState }}</p>
        <p class="placeholder-text">AccessToken (getter): {{ accessTokenGetter }}</p>
        <p class="placeholder-text">AccessToken (computed): {{ accessTokenComputed }}</p>
    </div>
</template>

<style scoped>
.placeholder-text {
    color: var(--oj-text-muted);
}
</style>
