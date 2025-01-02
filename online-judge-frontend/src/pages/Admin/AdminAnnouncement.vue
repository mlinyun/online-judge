<template>
    <h1>公告</h1>
    <el-button type="primary" @click="AddAnnouncement">添加公告</el-button>
    <el-table :data="serverdata.array" style="width: 100%" border>
        <el-table-column prop="_id" label="ID" width="100" />
        <el-table-column prop="Title" label="标题" width="400" />
        <el-table-column prop="Comments" label="评论数" width="100"/>
        <el-table-column prop="Views" label="浏览量" width="100"/>
        <el-table-column prop="CreateTime" label="创建时间" width="200"/>
        <el-table-column label="操作">
            <template #default="scope">
                <el-button size="small" @click="handleCheck(scope.row)">查看</el-button>
                <el-button size="small" @click="handleEdit(scope.row)">编辑</el-button>
                <el-button size="small" type="danger"
                @click="handleDelete(scope.row)">删除</el-button>
            </template>
        </el-table-column>
    </el-table>

    <div class="demo-pagination-block">
        <el-pagination
        v-model:current-page="currentPage"
        v-model:page-size="pageSize"
        :page-sizes="[10, 20, 30]"
        :small="small"
        :disabled="disabled"
        :background="background"
        layout="total, sizes, prev, pager, next, jumper"
        :total="TotalNum"
        @size-change="handleSizeChange"
        @current-change="handleCurrentChange"
        />
    </div>
</template>


<script setup>
import service from '../../axios'
import {reactive,ref,onMounted} from 'vue'
import { ElMessage } from 'element-plus'
import { useRouter } from 'vue-router'
const router = useRouter();

const pointmessage = ref('')

let serverdata = reactive({'array':[]})
let TotalNum = ref(0)

let currentPage = ref(1) // 当前页数
let pageSize = ref(10) // 当前页的数量

// 增加公告
function AddAnnouncement(){
    router.push({name:"AnnouncementEditor",query: { 
        EditType:"Insert",
        AnnouncementId:"0"
    }})
}

// 查看公告
function handleCheck(row){
    router.push({
        name: "Announcement",
        query: { 
            AnnouncementId: row._id, 
        }
    });
}
// 编辑公告
function handleEdit(row){
    router.push({name:"AnnouncementEditor",query: { 
        EditType:"Update",
        AnnouncementId:row._id,
    }})
}
// 删除公告
function handleDelete(row){
    service.delete(`/api/announcement`,{
        params:{
            AnnouncementId:row._id
        }
    }).then(
        response => {
            console.log('请求成功了',response.data)
            if(response.data.Result=="Success"){
                pointmessage.value = "删除成功"
                SuccessMessage()
                router.go(0)
            }
        },
        error => {
            console.log('请求失败了',error.data)
        }
    )
}

function GetServerInfo(){
    service.get(`/api/announcementlist/admin`,{
        params: {
            Page : currentPage.value,
            PageSize : pageSize.value
        },
    }).then(
        response => {
            console.log('请求成功了',response.data)
            serverdata.array = response.data.ArrayInfo
            TotalNum.value = Number(response.data.TotalNum)
        },
        error => {
            console.log('请求失败了',error.data)
        }
    )
}

const handleSizeChange = (val) => {
    console.log(`${val} items per page`)
    pageSize.value = val;
    GetDiscussInfo()
}
const handleCurrentChange = (val) => {
    console.log(`current page: ${val}`)
    currentPage.value = val;
    GetDiscussInfo()
}

onMounted(()=>{
    GetServerInfo()
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