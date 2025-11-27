<template>
    <h1>题解</h1>
    <el-table :data="serverdata.array" style="width: 100%"
        border >
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
import store from '../../store'
import {reactive,ref,onMounted} from 'vue'
import { ElMessage } from 'element-plus'
import { useRouter } from 'vue-router'
const router = useRouter();

const pointmessage = ref('')

let serverdata = reactive({'array':[]})
let TotalNum = ref(0)

let currentPage = ref(1) // 当前页数
let pageSize = ref(10) // 当前页的数量


// 查看题解
function handleCheck(row){
    router.push({
        name: "Solution",
        query: { 
            SolutionId: row._id,
        }
    });
}
// 编辑题解
function handleEdit(row){
    router.push({name:"SolutionEditor",query: { 
        EditType:"Update",
        SolutionId:row._id,
        ParentId:"0"
    }})
}
// 删除题解
function handleDelete(row){
    service.delete(`/api/solution`,{
        params:{
            UserId:row.UserId,
            SolutionId:row._id
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
    service.get(`/api/solutionlist/admin`,{
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