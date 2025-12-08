<script setup lang="ts">
import { useRoute } from "vue-router";
defineOptions({ name: "DiscussionDetail" });

const route = useRoute();
const discussionId = route.params.id as string;
</script>

<template>
    <div class="discussion-detail">
        <h1>DiscussionDetail</h1>
        <p>讨论 ID: {{ discussionId }}</p>
    </div>
</template>

<style scoped></style>
