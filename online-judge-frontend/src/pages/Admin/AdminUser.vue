<template>
    <h1>用户</h1>
    <el-table :data="usersetdata.array" 
        border
        style="width: 100%">
        <el-table-column prop="_id" label="ID" width="180" />
        <el-table-column prop="NickName" label="昵称" width="180" />
        <el-table-column prop="PersonalProfile" label="个人简介" width="300"/>
        <el-table-column prop="School" label="学校" width="100"/>
        <el-table-column prop="Major" label="专业" width="100"/>
        <el-table-column prop="JoinTime" label="加入时间" width="200"/>
        <el-table-column label="操作">
            <template #default="scope">
                <el-button size="small" @click="handleCheck(scope.row)">查看</el-button>
                <el-button size="small" @click="handleEdit(scope.row)">编辑</el-button>
            </template>
        </el-table-column>
    </el-table>

    <div class="demo-pagination-block">
        <el-pagination
        v-model:current-page="currentPage"
        v-model:page-size="pageSize"
        :page-sizes="[20, 40, 60]"
        :small="small"
        :disabled="disabled"
        :background="background"
        layout="total, sizes, prev, pager, next, jumper"
        :total="totalsize"
        @size-change="handleSizeChange"
        @current-change="handleCurrentChange"
        />
    </div>
</template>

<script setup>
import service from '../../axios'
import {reactive,ref,onMounted} from 'vue'
import { useRouter} from 'vue-router'
import { ElMessage } from 'element-plus'
const router = useRouter()
const pointmessage = ref('')

let currentPage = ref(1) // 当前页数
let pageSize = ref(20) // 当前页的数量
let totalsize = ref(20)
const small = ref(false)
const background = ref(false)
const disabled = ref(false)

// --------------------------------------
// 查看用户
function handleCheck(row){
    router.push({
        name: "UserHome",
        query: { UserId: row._id }
    });
}
// 编辑用户
function handleEdit(row){
    console.log('编辑用户',row._id)
    router.push({
        name: "UserSetting",
        query: {UserId: row._id }
    });
}
// 删除用户
function handleDelete(row){
    console.log('点击删除',row)
    service.delete(`/api/user`,{
        params:{
            UserId:row._id
        }
    }).then(
        response => {
            console.log('请求成功了',response.data)
            if(response.data.Result=="Success"){
                pointmessage.value = "删除成功"
                SuccessMessage()
                router.go(0)
            }else{
                pointmessage.value = response.data.Reason
                ErrorMessage()
            }
        },
        error => {
            console.log('请求失败了',error.data)
        }
    )
}
// -------------------------------------------------------
const handleSizeChange = (val) => {
    console.log(`${val} items per page`)
    pageSize.value = val;
    GetUserSetInfo(currentPage.value,pageSize.value)
}
const handleCurrentChange = (val) => {
    console.log(`current page: ${val}`)
    currentPage.value = val;
    GetUserSetInfo(currentPage.value,pageSize.value)
}
// 用户信息列表
let usersetdata = reactive({'array':[]})

function GetUserSetInfo(m_page, m_pagesize){
    service.get(`/api/userlist/admin`,{
        params: {
            Page : m_page,
            PageSize : m_pagesize
        },
    }).then(
        response => {
            if(response.data.Result == "Success"){
                console.log('请求成功了',response.data)
                usersetdata.array = response.data.ArrayInfo
                totalsize.value = Number(response.data.TotalNum)
            }else{
                pointmessage.value = response.data.Reason
                ErrorMessage()
            }
            
        },
        error => {
            console.log('请求失败了',error.data)
        }
    )
    console.log(m_page,m_pagesize)
}

onMounted(()=>{
    GetUserSetInfo(currentPage.value,pageSize.value)
})
// 发送成功消息
const SuccessMessage = () => {
    ElMessage({
        showClose: true,
        message: pointmessage.value,
        type: 'success',
    })
}
// 发送警告消息
const WaringMessage = () => {
    ElMessage({
        showClose: true,
        message: pointmessage.value,
        type: 'warning',
    })
}
// 发送错误消息
const ErrorMessage = () => {
    ElMessage({
        showClose: true,
        message: pointmessage.value,
        type: 'error',
    })
}
</script>

<style scoped>
.demo-pagination-block + .demo-pagination-block {
  margin-top: 10px;
}
.demo-pagination-block .demonstration {
  margin-bottom: 16px;
}
</style>