<template>
	<el-card class="box-card">
		<el-row class="tac">
			<el-col :span="3">
			<el-menu
				:default-active="$route"
				class="el-menu-vertical-demo"
				v-bind:router="true"
			>
				<el-menu-item index="/admin/adminproblem">
					<el-icon><icon-menu /></el-icon>
					题目管理
				</el-menu-item>
				<el-menu-item index="/admin/adminuser">
					<el-icon><icon-menu /></el-icon>
					用户管理
				</el-menu-item>
				<el-menu-item index="/admin/admindiscuss">
					<el-icon><icon-menu /></el-icon>
					讨论管理
				</el-menu-item>
				<el-menu-item index="/admin/adminannouncement">
					<el-icon><icon-menu /></el-icon>
					公告管理
				</el-menu-item>
				<el-menu-item index="/admin/adminsolution">
					<el-icon><icon-menu /></el-icon>
					题解管理
				</el-menu-item>
				<el-menu-item index="/admin/admincomment">
					<el-icon><icon-menu /></el-icon>
					评论管理
				</el-menu-item>
			</el-menu>
			</el-col>
			<el-col :span="21">
				<router-view></router-view>
			</el-col>
		</el-row>
	</el-card>
</template>

<script lang="ts" setup>

</script>

<style scoped>
.box-card{
    margin: 10px;
}
</style>
  