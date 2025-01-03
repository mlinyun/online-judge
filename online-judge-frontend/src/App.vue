<template>
  <div class="header">
    <el-menu id="nav"
             :default-active="$route"
             class="el-menu-demo"
             mode="horizontal"
             v-bind:router="true"
    >
      <el-menu-item index="/">
        <el-icon size="30">
          <HomeFilled/>
        </el-icon>
        <span class="navtext">首页</span>
      </el-menu-item>
      <el-menu-item index="/problemlist">
        <el-icon size="30">
          <Grid/>
        </el-icon>
        <span class="navtext">题库</span>
      </el-menu-item>
      <el-menu-item index="/statusrecord">
        <el-icon size="30">
          <Tools/>
        </el-icon>
        <span class="navtext">测评</span>
      </el-menu-item>
      <el-menu-item index="/discusslist">
        <el-icon size="30">
          <Comment/>
        </el-icon>
        <span class="navtext">讨论</span>
      </el-menu-item>
      <el-menu-item index="/userrank">
        <el-icon size="30">
          <Histogram/>
        </el-icon>
        <span class="navtext">排名</span>
      </el-menu-item>
      <div v-if="!store.state.IsLogin" class="navright">
        <el-button id="button" type="primary" @click="openlogindialog" text>登录</el-button>
        <el-button id="button" type="primary" @click="openregisterdialog" text>注册</el-button>
      </div>
      <div v-if="store.state.IsLogin" class="navright">
        <el-dropdown @command="handleCommand" id="user">
          <el-button type="primary" text>
            <el-icon>
              <User/>
            </el-icon>
            &nbsp {{ store.state.NickName }}
            <el-icon class="el-icon--right">
              <arrow-down/>
            </el-icon>
          </el-button>
          <template #dropdown>
            <el-dropdown-menu>
              <el-dropdown-item command="userhome">
                <el-icon>
                  <User/>
                </el-icon>
                个人主页
              </el-dropdown-item>
              <el-dropdown-item command="statusrecord">
                <el-icon>
                  <Edit/>
                </el-icon>
                我的提交
              </el-dropdown-item>
              <el-dropdown-item command="discuss">
                <el-icon>
                  <Edit/>
                </el-icon>
                我的讨论
              </el-dropdown-item>
              <el-dropdown-item command="solution">
                <el-icon>
                  <Edit/>
                </el-icon>
                我的题解
              </el-dropdown-item>
              <el-dropdown-item command="usersetting">
                <el-icon>
                  <Setting/>
                </el-icon>
                设置
              </el-dropdown-item>
              <el-dropdown-item command="admin" v-if="store.state.IsAdmin">
                <el-icon>
                  <Odometer/>
                </el-icon>
                管理员
              </el-dropdown-item>
              <el-dropdown-item command="logout">
                <el-icon>
                  <SwitchButton/>
                </el-icon>
                退出登录
              </el-dropdown-item>
            </el-dropdown-menu>
          </template>
        </el-dropdown>
      </div>
    </el-menu>
  </div>
  <div class="main">
    <login ref="logindialog"></login>
    <register ref="registerdialog"></register>
    <router-view></router-view>
  </div>
  <div class="footer">
    <p>
      Powered by XXX
    </p>
  </div>
</template>
<script setup>
import Register from './components/Dialog/Register.vue'
import Login from './components/Dialog/Login.vue'

import {ref} from 'vue'
import store from './store'
import {useRouter} from 'vue-router'

const router = useRouter();

const logindialog = ref()
const registerdialog = ref()

function openlogindialog() {
  logindialog.value.open()
}

function openregisterdialog() {
  registerdialog.value.open()
}

function handleCommand(command) {
  if (command == "userhome") {
    router.push({name: "UserHome", query: {UserId: store.state.UserId}})
  } else if (command == "usersetting") {
    router.push({name: "UserSetting", query: {UserId: store.state.UserId}})
  } else if (command == "admin") {
    router.push({name: "Admin"})
  } else if (command == "statusrecord") {
    router.push({
      name: "StatusRecord",
      query: {
        UserId: store.state.UserId
      }
    })
  } else if (command == "discuss") {
    // command == "discuss"
    router.push({
      name: "DiscussList",
      query: {
        UserId: store.state.UserId
      }
    })
  } else if (command == "solution") {
    router.push({
      name: "SolutionList",
      query: {
        UserId: store.state.UserId
      }
    })
  } else if (command = "logout") {
    store.commit('Logout')
    router.push({name: 'HomePage'})
  }

}
</script>
<style scoped>

.header {
  width: 1500px
}

.main {
  width: 1500px
}

.footer {
  width: 1500px;
  margin-top: 20px;
  margin-bottom: 10px;
  text-align: center;
  font-size: small;
}

.flex-grow {
  flex-grow: 1;
}

.navtext {
  font-family: Arial;
  font-size: 15px;
}

.navright {
  margin-left: auto;
  margin-right: 15px;
  align-self: center
}

#nav {
  background-color: #ffffff;
  position: relative;
  margin: 5px;
  z-index: 5;
  width: 100%;
}

.example-showcase .el-dropdown + .el-dropdown {
  margin-left: 15px;
}

.example-showcase .el-dropdown-link {
  cursor: pointer;
  color: var(--el-color-primary);
  display: flex;
  align-items: center;
}

</style>
