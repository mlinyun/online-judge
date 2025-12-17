<script setup lang="ts">
import { HeroSection, FeatureCards, AnnouncementList } from "@/components/home";

defineOptions({ name: "HomeView" });
</script>

<template>
    <div class="home-view">
        <!-- Hero Section -->
        <HeroSection />

        <!-- Feature Cards -->
        <FeatureCards />

        <!-- Announcements -->
        <AnnouncementList />
    </div>
</template>

<style scoped>
.home-view {
    width: 100%;
    max-width: var(--oj-container-xl);
    padding: var(--oj-spacing-4);
    margin: 0 auto;
}

@media (width >= 640px) {
    .home-view {
        padding: var(--oj-spacing-6);
    }
}

@media (width >= 1024px) {
    .home-view {
        padding: var(--oj-spacing-8);
    }
}
</style>
