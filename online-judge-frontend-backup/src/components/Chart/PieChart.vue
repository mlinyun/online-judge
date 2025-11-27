<template>
    <center>
        <h2>{{ title }}</h2>
    </center>
    <div id ="main" style="height:300px;width:300px"></div>
</template>

<script setup>
import { onMounted,reactive,ref } from 'vue'
import * as echarts from 'echarts'
let myChart
const title = ref('')
const options = reactive({
    tooltip: {
    },
    legend: {
      orient: 'horizontal',
      x: 'center',
      y: 'bottom',
      data: [ 'CE', 'AC', 'WA', 'RE', 'TLE', 'MLE', 'SE' ],
    },
    series: [
        {
            type: 'pie',
            stillShowZeroSum: false,
            data: [
                {
                    value: 0,
                    name: 'CE'
                },
                {
                    value: 0,
                    name: 'AC'
                },
                {
                    value: 0,
                    name: 'WA'
                },
                {
                    value: 0,
                    name: 'RE'
                },
                {
                    value: 0,
                    name: 'TLE'
                },
                {
                    value: 0,
                    name: 'MLE'
                },
                {
                    value: 0,
                    name: 'SE'
                }
            ],
            radius: '45%'
        }
    ]
})

function SetDataInfo(info)
{  
    options.series[0].data[0].value = info.CENum
    options.series[0].data[1].value = info.ACNum
    options.series[0].data[2].value = info.WANum
    options.series[0].data[3].value = info.RENum
    options.series[0].data[4].value = info.TLENum
    options.series[0].data[5].value = info.MLENum
    options.series[0].data[6].value = info.SENum
    title.value = info.Title
    myChart.setOption(options)
}


function EchartsInit()
{
    // 基于准备好的dom，初始化echarts实例
    myChart = echarts.init(document.getElementById('main'));
    // 绘制图表
    myChart.setOption(options)
}
defineExpose({
    SetDataInfo
})
onMounted(()=>{
    EchartsInit()
})
</script>