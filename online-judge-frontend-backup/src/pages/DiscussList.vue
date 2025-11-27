<template>
    <el-card class="box-card">
        <el-row>
            <el-col :span="2"></el-col>
            <el-col :span="20">
                <div>
                    <h1>讨论</h1>
                    <div id="addbutton">
                        <el-button @click="AddDiscussion" type="success" v-show="addbuttonshow" :disabled="addbuttondisabled">
                            写讨论
                        </el-button>
                    </div>
                </div>
                <el-card v-for="(data,index) in serverdata.array" id="discusscard">
                    <el-row>
                        <el-col :span="2">
                            <el-avatar :size="70" :src="data.User[0].Avatar"/>
                        </el-col>
                        <el-col :span="22">
                            <router-link :to="{name:'Discuss',query:{DiscussId:data._id}}">
                                <span id="discusstitle">{{ data.Title }}</span>
                            </router-link>
                            <br> 
                            <br>
                            {{ data.User[0].NickName }} &nbsp
                            {{ data.CreateTime }} &nbsp
                            <div class="iconright">
                                <el-icon><View /></el-icon> &nbsp {{ data.Views }} &nbsp
                                <el-icon><ChatRound /></el-icon> &nbsp {{ data.Comments }}
                            </div>
                        </el-col>
                    </el-row>
                </el-card>

                <div class="demo-pagination-block">
                    <el-pagination
                    v-model:current-page="currentPage"
                    v-model:page-size="pageSize"
                    :page-sizes="[10, 15, 25]"
                    :small="small"
                    :disabled="disabled"
                    :background="background"
                    layout="total, sizes, prev, pager, next, jumper"
                    :total="TotalNum"
                    @size-change="handleSizeChange"
                    @current-change="handleCurrentChange"
                    />
                </div>
            </el-col>
            <el-col :span="2"></el-col>
        </el-row>
    </el-card>
</template>


<script setup>
import service from '../axios'
import {reactive,ref,onMounted} from 'vue'

import { useRouter,useRoute,onBeforeRouteUpdate } from 'vue-router'
import store from '../store';
const router = useRouter()
const route = useRoute()

const addbuttonshow = ref(true)
const addbuttondisabled = ref(false)
// 上层id 默认为 0
const searchinfo = reactive({
    ParentId:'0',
    UserId:'0'
})

let serverdata = reactive({'array':[]})
let TotalNum = ref(0)

let currentPage = ref(1) // 当前页数
let pageSize = ref(10) // 当前页的数量

// 请求服务器获取信息
function GetServerInfo(ParentId,UserId){
    if(ParentId!=null){
        searchinfo.ParentId = ParentId
    }else{
        searchinfo.ParentId = 0
    }

    if(UserId!=null){
        searchinfo.UserId = UserId
    }else{
        searchinfo.UserId = 0
    }
    service.get(`/api/discusslist`,{
        params: {
            SearchInfo:JSON.stringify(searchinfo),
            Page : currentPage.value,
            PageSize : pageSize.value
        },
    }).then(
        response => {
            console.log('请求成功了',response.data)
            if(response.data.ArrayInfo == null) serverdata.array = []
            serverdata.array = response.data.ArrayInfo
            TotalNum.value = Number(response.data.TotalNum)
        },
        error => {
            console.log('请求失败了',error.data)
        }
    )
}

function cellclick(row, column, cell, event) {
    router.push({
        name: "Discuss",
        query: {
            DiscussId: row._id,
        }
    });
}
function AddDiscussion(){
    router.push({name:"DiscussEditor",query: { 
        EditType:"Insert",
        ParentId:searchinfo.ParentId,
        DiscussId:"0"
    }})
}
const handleSizeChange = (val) => {
    console.log(`${val} items per page`)
    pageSize.value = val;
    GetDiscussInfo(searchinfo.ParentId,searchinfo.UserId)
}
const handleCurrentChange = (val) => {
    console.log(`current page: ${val}`)
    currentPage.value = val;
    GetDiscussInfo(searchinfo.ParentId,searchinfo.UserId)
}

// 路由更改前
onBeforeRouteUpdate(to=>{
    GetServerInfo(to.query.ParentId,to.query.UserId)
    if(to.query.UserId != null) addbuttonshow.value = false
    else addbuttonshow.value = true
})
// 组件初始化
onMounted(()=>{
    GetServerInfo(route.query.ParentId,route.query.UserId)
    if(store.state.UserId == '0')addbuttondisabled.value = true

    if(route.query.UserId != null) addbuttonshow.value = false
    else addbuttonshow.value = true
})
</script>

<style scoped>
#addbutton{
    display: flex;
    justify-content: flex-end;
}
.iconright{
    float:right;
}
#discusstitle{
    font-family:Arial;
    font-size:18px;
}
#discusscard{
    margin: 10px;
}
.demo-pagination-block{
    margin-top: 20px;
    display: flex;
    justify-content: center;
    align-items: center
}

.box-card{
    margin: 10px;
}
</style>