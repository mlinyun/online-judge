<template>
    <h1>评论</h1>
    <el-table
        :data="TableData"
        style="width: 100%; margin-bottom: 20px"
        row-key="id"
        border
    >
        <el-table-column prop="content" label="内容" sortable />
        <el-table-column prop="createtime" label="创建时间" sortable />
        <el-table-column label="操作">
        <template #default="scope">
            <el-button size="small" type="danger"
            @click="handleDelete(scope.row)">删除</el-button>
        </template>
    </el-table-column>
    </el-table>
    <div class="demo-pagination-block">
        <el-pagination
            v-model:current-page="currentPage"
            v-model:page-size="pageSize"
            :page-sizes="[10, 15, 20]"
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

<script lang="ts" setup>
import service from '../../axios'
import {reactive,ref,onMounted} from 'vue'
import { useRouter} from 'vue-router'
import store from '../../store'

const router = useRouter()

let currentPage = ref(1) // 当前页数
let pageSize = ref(10) // 当前页的数量
let totalsize = ref(0)
const small = ref(false)
const background = ref(false)
const disabled = ref(false)

// 评论接口
interface Comment {
    id: number
    parentid: number
    parenttype: string
    content: string
    createtime: string
    hasChildren?: boolean
    children?: Comment[]
}

let TableData:Comment[] = reactive([])

function GetServerInfo(){
    service.get(`/api/commentlist/admin`,{
        params: {
            Page : currentPage.value,
            PageSize : pageSize.value
        },
    }).then(
        response => {
            console.log('请求成功了',response.data)
            SetInfo(response.data.ArrayInfo)
            totalsize.value = Number(response.data.TotalNum)
        },
        error => {
            console.log('请求失败了',error.data)
        }
    )
}

// 设置数据
function SetInfo(ServerInfo){
    // 设置数据为空
    TableData = []
    console.log('serverinfo',ServerInfo)
    for(var i in ServerInfo){
        let Info = ServerInfo[i]
        let fatherinfo:Comment = {
            id : Info["_id"],
            parentid : Info["ParentId"],
            parenttype : Info["ParentType"],
            content : Info["Content"],
            createtime : Info["CreateTime"],
            children:[]
        }

        for(var j in Info["Child_Comments"]){
            let SonInfo = Info["Child_Comments"][j]
            let soninfo:Comment = {
                id : SonInfo["_id"],
                parentid : Info["ParentId"],
                parenttype : Info["ParentType"],
                content : SonInfo["Content"],
                createtime : SonInfo["CreateTime"]
            }

            fatherinfo.children?.push(soninfo) 
        }
        TableData.push(fatherinfo)
    }
}
function handleDelete(row){
    console.log('点击删除',row)
    service.delete(`/api/comment`,{
        params:{
            ArticleId:row.parentid,
			CommentId:row.id
        }
    }).then(
        response => {
            console.log('请求成功了',response.data)
            if(response.data.Result=="Success"){
                router.go(0)
            }
        },
        error => {
            console.log('请求失败了',error.data)
        }
    )
}
const handleSizeChange = (val) => {
    console.log(`${val} items per page`)
    pageSize.value = val;
    GetServerInfo()
}
const handleCurrentChange = (val) => {
    console.log(`current page: ${val}`)
    currentPage.value = val;
    GetServerInfo()
}
onMounted(()=>{
    GetServerInfo()
})
</script>

<style scoped>
.demo-pagination-block{
    margin-top: 20px;
    display: flex;
    justify-content: center;
    align-items: center
}
</style>