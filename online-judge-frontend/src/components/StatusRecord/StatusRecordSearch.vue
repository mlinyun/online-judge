<template>
  <div class="m-4">
      <el-row :gutter="10">
          <el-col :span="6">
              <el-input v-model="searchproblemid" placeholder="题目ID" maxlength=5 />
          </el-col>
          <el-col :span="6">
              <el-input v-model="searchproblemtitle" placeholder="题目标题" />
          </el-col>
          <el-col :span="5">
              状态:&nbsp&nbsp
              <el-select v-model="searchstatue" class="m-2" placeholder="状态">
                  <el-option
                  v-for="item in statusoptions"
                  :key="item.value"
                  :label="item.label"
                  :value="item.value"
                  />
              </el-select>
          </el-col>
          <el-col :span="5">
              语言:&nbsp&nbsp
              <el-select v-model="searchlanguage" class="m-2" placeholder="语言">
                  <el-option
                  v-for="item in languageoptions"
                  :key="item.value"
                  :label="item.label"
                  :value="item.value"
                  />
              </el-select>
          </el-col>
          <el-col :span="2">
              <el-button type="primary" :icon="Search" @click="searchbutton">搜索</el-button>
          </el-col>
      </el-row>
</div>
</template>

<script setup>
import { ref ,reactive,onMounted} from 'vue'
import store from '../../store'
import { ElMessage } from 'element-plus'
import { Search} from '@element-plus/icons-vue'

const pointmessage = ref('')

const searchproblemid = ref('')
const searchproblemtitle = ref('')
const searchstatue = ref('')
const searchlanguage = ref('')

const statusoptions = [
{
  value: '',
  label: 'ALL',
},
{
  value: '0',
  label: 'Pending & Judging',
},
{
  value: '1',
  label: 'Compile Error',
},
{
  value: '2',
  label: 'Accepted',
},
{
  value: '3',
  label: 'Wrong Answer',
},
{
  value: '4',
  label: 'Runtime Error',
},
{
  value: '5',
  label: 'Time Limit Exceeded',
},
{
  value: '6',
  label: 'Memory Limit Exceeded',
},
{
  value: '7',
  label: 'System Error',
},
]
const languageoptions = [
{
  value: '',
  label: 'ALL',
},
{
  value: 'C',
  label: 'C',
},
{
  value: 'C++',
  label: 'C++',
},
{
  value: 'Go',
  label: 'Go',
},
{
  value: 'Java',
  label: 'Java',
},
{
  value: 'Python2',
  label: 'Python2',
},
{
  value: 'Python3',
  label: 'Python3',
},
{
  value: 'JavaScript',
  label: 'JavaScript',
}
]
const props = defineProps(['SearchStatusRecordList'])
function GetSearchInfo()
{
  let Info={
      ProblemId:searchproblemid.value,
      ProblemTitle:searchproblemtitle.value,
      Status:searchstatue.value,
      Language:searchlanguage.value
  }
  return Info
}

function searchbutton()
{
  props.SearchStatusRecordList()
}


defineExpose({
  GetSearchInfo
})
</script>