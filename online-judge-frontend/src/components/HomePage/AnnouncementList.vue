<template>
    <el-card v-for="(data,index) in serverdata.array" id="announcementcard">
        <el-row>
            <el-col :span="4">
                <el-icon size="40"><ChatDotSquare /></el-icon>
            </el-col>
            <el-col :span="20">
                <el-row>
                    <el-col :span="20">
                        <router-link :to="{name:'Announcement',query:{AnnouncementId:data._id}}">
                            <span>{{ data.Title }}</span>
                        </router-link>
                    </el-col>
                    <el-col :span="4">
                        <el-icon><View /></el-icon> &nbsp {{ data.Views }}
                    </el-col>
                </el-row>
                <el-row>
                    <el-col :span="20">
                        {{ data.CreateTime }}
                    </el-col>
                    <el-col :span="4">
                        <el-icon><ChatRound /></el-icon> &nbsp {{ data.Comments }}
                    </el-col>
                </el-row>
            </el-col>
        </el-row>
    </el-card>
    <div class="demo-pagination-block">
        <el-pagination
        v-model:current-page="currentPage"
        v-model:page-size="pageSize"
        :page-sizes="[5, 10, 15]"
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

import { useRouter } from 'vue-router'
const router = useRouter();

let serverdata = reactive({'array':[]})
let TotalNum = ref(0)

let currentPage = ref(1) // 当前页数
let pageSize = ref(5) // 当前页的数量

function GetServerInfo(){
    service.get(`/api/announcementlist`,{
        params: {
            Page : currentPage.value,
            PageSize : pageSize.value
        },
    }).then(
        response => {
            console.log('请求Announcement成功',response.data)
            if(response.data.Result=="Success"){
                serverdata.array = response.data.ArrayInfo
                TotalNum.value = Number(response.data.TotalNum)
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
    // 获取信息
    GetServerInfo()
})

</script>

<style scoped>
#announcementcard{
    margin: 10px;
}

.demo-pagination-block{
    margin-top: 10px;
    display: flex;
    justify-content: center;
    align-items: center
}
</style>