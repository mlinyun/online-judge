<template>
    <el-card class="box-card">
        <div id="top">
            <el-avatar :size="50" :src="useravatar" /> &nbsp
            <h2>{{ title }}</h2>
        </div>
        
        <el-row>
            <el-col :span="2"></el-col>

            <el-col :span="20">
                <div>
                    <v-md-preview :text=content></v-md-preview>
                </div>
                <div id="twobutton">
                    <el-button type="primary" @click="UpdateArticle" v-show="buttonshow">修改</el-button>
                    <el-button type="primary" @click="DeleteArticle" v-show="buttonshow">删除</el-button>
                </div>
                <div>
                    <Comment :ParentId=$route.query.SolutionId ArticleType="Solution" ></Comment>
                </div>
            </el-col>

            <el-col :span="2"></el-col>
        </el-row>
    </el-card>
</template>

<script setup>
import Comment from '../components/Comment.vue'
import {ref,onMounted } from 'vue'
import { useRouter,useRoute } from 'vue-router'
import service from '../axios'
import store from '../store'

const router = useRouter()
const route = useRoute()

const solutionid = ref(0)

const title = ref('')
const content = ref('')
const views = ref(0)
const comments = ref(0)
const usernickname = ref('')
const useravatar = ref('')
const authorid = ref('')

const buttonshow = ref(false)

function GetServerInfo() {
    service
    .get(`/api/solution`, {
        params: {
            SolutionId: solutionid.value,
        },
    })
    .then(
        (response) => {
            console.log("请求成功了！！！",response.data)
            if(response.data.Result == "Success"){
                SetServerData(response.data)
            }
        },
        (error) => {
            console.log("请求失败了！！！");
            console.log(error.data);
        }
    );
}
function SetServerData(data)
{
    title.value = data.Title
    content.value = data.Content
    views.value = data.Views
    comments.value = data.Comments
    usernickname.value = data.User[0].NickName
    useravatar.value = data.User[0].Avatar
    authorid.value = data.User[0]._id
    if(Number(data.User[0]._id) == Number(store.state.UserId))
        buttonshow.value = true
}

function UpdateArticle(){
    router.push({name:"SolutionEditor",query: { 
        EditType:"Update",
        SolutionId:solutionid.value,
        ParentId:"0"
    }})
}

function DeleteArticle(){
    service
    .delete(`/api/solution`, {
        params:{
            UserId:authorid.value,
            SolutionId:solutionid.value,
        }
    })
    .then(
        (response) => {
            console.log("请求成功了！！！",response.data)
            router.go(-1)
        },
        (error) => {
            console.log("请求失败了！！！");
            console.log(error.data);
        }
    );
}

onMounted(()=>{
    solutionid.value = route.query.SolutionId
    GetServerInfo()
})
</script>
  
<style scoped>
#twobutton{
    display: flex;
    justify-content: flex-end;
}
#top{
    display: flex;
    justify-content: center;
    align-items: center
}

.box-card{
    margin: 10px;
}
</style>