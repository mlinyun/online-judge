<template>
    <el-card class="box-card">
        <h1>{{ NickName }}</h1>
        <el-row :gutter="20">
            <el-col :span="4">
                <el-avatar :size="170" :src="Avatar" />
            </el-col>
            <el-col :span="14">
                个性签名：{{ PersonalProfile }}
                        <el-divider />
                        学校：{{ School }}
                        <el-divider />
                        专业：{{ Major }}
                        <el-divider />
                        解题数量：{{ ACNum }}
                        <el-divider />
                        提交次数：{{ SubmitNum }}
                        <el-divider />
                        加入时间：{{ JoinTime }}
                        <el-divider />
            </el-col>
            <el-col :span="6">
                <UserPieChart ref="userpiechart"></UserPieChart>
            </el-col>
        </el-row>
    </el-card>
</template>
  
<script setup>
import service  from '../axios'
import UserPieChart from '../components/Chart/UserPieChart.vue'
import { ref,reactive,onMounted } from 'vue'
import { useRouter } from 'vue-router'

const userpiechart = ref()
const router = useRouter()
const userid = router.currentRoute.value.query.UserId
const pointmessage = ref('')

const NickName = ref('')
const Avatar = ref('')
const PersonalProfile = ref('')
const School = ref('')
const Major = ref('')
const JoinTime = ref('')
const ACNum = ref(0)
const Solves = reactive({array:[]})
const SubmitNum = ref()

function GetInfo(){
    console.log('调用函数')
    service
        .get(`/api/user/home`, {
            params: {
                UserId: userid,
            },
        })
        .then(
            (response) => {
                if(response.data.Result == "Success"){
                    setinfo(response.data)
                }else{
                    pointmessage.value = response.data.Reason
                    ErrorMessage()
                }
            },
            (error) => {
                pointmessage.value = "网络似乎出现了问题！";
                ErrorMessage()
            }
        );
}
function setinfo(info)
{
    NickName.value = info.NickName
    Avatar.value = info.Avatar
    PersonalProfile.value = info.PersonalProfile
    School.value = info.School
    Major.value = info.Major
    JoinTime.value = info.JoinTime
    ACNum.value = info.ACNum
    Solves.array = info.Solves
    SubmitNum.value = info.SubmitNum

    let tmpinfo = {ACNum:info.ACNum,SubmitNum:info.SubmitNum}
    userpiechart.value.SetDataInfo(tmpinfo)
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
    // 调用函数
    GetInfo()
})
</script>

<style scoped>
.box-card{
    margin: 10px;
}
.h1{
    font-size:50px;
    margin-left: 20px;
}
</style>