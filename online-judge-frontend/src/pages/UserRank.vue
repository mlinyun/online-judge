<template>
    <el-card class="box-card">
        <el-carousel :interval="4000" type="card" height="300px">
            <el-carousel-item v-for="(item,index) in carouseldata.array" :key="index">
                <center>
                    <h1>{{item.rank}}</h1>
                    <el-avatar :size="100" :src="item.avatar" />
                    <h1>{{item.nickname}}</h1>
                </center>
            </el-carousel-item>
        </el-carousel>
        <el-table :data="userrankdata.array" style="width: 100%" @cell-click="(row, column, cell, event)=>rankclick(row, column, cell, event)">
            <el-table-column prop="Rank" label="排名" width="100" />
            <el-table-column prop="Avatar" label="" width="100">
                <template #default="scope">
                    <el-avatar :size="50" :src="scope.row.Avatar" />
                </template>
            </el-table-column>
            <el-table-column prop="NickName" label="NickName" width="200" />
            <el-table-column prop="PersonalProfile" label="PersonalProfile" width="700"/>
            <el-table-column prop="ACNum" label="ACNum" width="150"/>
            <el-table-column prop="SubmitNum" label="SubmitNum" width="150"/>
        </el-table>
        <div class="demo-pagination-block">
            <el-pagination
            v-model:current-page="currentPage"
            v-model:page-size="pageSize"
            :page-sizes="[15, 30, 50]"
            :small="small"
            :disabled="disabled"
            :background="background"
            layout="total, sizes, prev, pager, next, jumper"
            :total="TotalNum"
            @size-change="handleSizeChange"
            @current-change="handleCurrentChange"
            />
        </div>
    </el-card>
</template>

<script setup>
import {reactive,ref,onMounted} from 'vue'
import service from '../axios'
import { useRouter } from 'vue-router'

const router = useRouter()
let currentPage = ref(1) // 当前页数
let pageSize = ref(15) // 当前页的数量
let TotalNum = ref(0)
const small = ref(false)
const background = ref(false)
const disabled = ref(false)

const pointmessage = ref('')

const handleSizeChange = (val) => {
    console.log(`${val} items per page`)
    pageSize.value = val;
    GetUserRank()
}
const handleCurrentChange = (val) => {
    console.log(`current page: ${val}`)
    currentPage.value = val;
    GetUserRank()
}

// 用户排名信息列表
let userrankdata = reactive({'array':[]})

let carouseldata = reactive({'array':[]})

/*
    功能：获取用户的Rank排名
    传入：Json(Page,PageSize)
    传出：Json(ArrayInfo[_id,Rank,Avatar,NickName,PersonalProfile,SubmitNum,ACNum],TotalNum)
*/
function GetUserRank(){
    service.get(`/api/user/rank`,{
        params: {
            Page : currentPage.value,
            PageSize : pageSize.value
        },
    }).then(
        response => {
            if(response.data.Result == "Success"){
                userrankdata.array = response.data.ArrayInfo
                TotalNum.value = Number(response.data.TotalNum)
                if(currentPage.value == 1){
                    setCarousel(response.data.ArrayInfo)
                }
            }else{
                pointmessage.value = response.data.Reason
                ErrorMessage()
            }
        },
        error => {
            pointmessage.value = "网络似乎出现了问题！";
            ErrorMessage()
        }
    )
}

function setCarousel(Info)
{
    let size = Info.length >= 3 ? 3 : Info.length
    for(var i = 0; i < size; i++){
        let info = {
            rank: Info[i].Rank,
            nickname: Info[i].NickName,
            avatar: Info[i].Avatar
        }
        carouseldata.array.push(info)
    }
}
function rankclick(row, column, cell, event){
    router.push({name:"UserHome",query:{UserId:row._id}})
}

// 发送错误消息
const ErrorMessage = () => {
    ElMessage({
        showClose: true,
        message: pointmessage.value,
        type: 'error',
    })
}

onMounted(()=>{
    GetUserRank()
})
</script>

<style scoped>
.box-card{
    margin: 10px;
}
.demo-pagination-block{
    margin-top: 20px;
    display: flex;
    justify-content: center;
    align-items: center
}

.el-carousel__item h3 {
  color: #475669;
  opacity: 0.75;
  line-height: 200px;
  margin: 0;
  text-align: center;
}

.el-carousel__item:nth-child(2n) {
  background-color: #99a9bf;
}

.el-carousel__item:nth-child(2n + 1) {
  background-color: #d3dce6;
}
</style>