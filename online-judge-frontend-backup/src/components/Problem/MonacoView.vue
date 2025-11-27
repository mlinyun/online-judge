<template>
    <div class="wrapper">
        <div id="codeEdit"></div>
    </div>
</template>

<script setup>
import { ref,onMounted,toRaw } from 'vue'
import * as monaco from 'monaco-editor'
const editor = ref(null)
// 初始化代码编辑器
function initEditor(){    
    editor.value = monaco.editor.create(document.getElementById('codeEdit'), {
        value: '',                  // 代码
        language: 'cpp',   // 语言
        theme: 'vs',   // 主题
        selectOnLineNumbers: true,  //显示行号
        foldingStrategy: 'indentation', // 代码可分小段折叠
        tabSize: 4,                 // tab 缩进长度
        roundedSelection: false,
        readOnly: true,            // 只读
        cursorStyle: 'line',        //光标样式
        automaticLayout: true,      //自动布局
        glyphMargin: true,          //字形边缘
        useTabStops: false,
        fontSize: 15,   //字体大小
        autoIndent: true,           //自动布局
    });
}
function SetData(Info)
{
    let language = "cpp"
    if(Info.Language == "C++"){
        language = "cpp"
    }else if(Info.Language == "C"){
        language = "c"
    }else if(Info.Language == "Go"){
        language = "go"
    }else if(Info.Language == "Java"){
        language = "java"
    }else if(Info.language == "Python2"|| Info.language == "Python3"){
        language = "python"
    }else if(Info.language == "JavaScript"){
        language = "javascript"
    }
    let code = Info.Code
    const curmodel = toRaw(editor.value).getModel()
    const model = monaco.editor.createModel(code,language)
    toRaw(editor.value).setModel(model)
    curmodel.dispose()
}
onMounted(()=>{
    initEditor()
})
defineExpose({
    SetData
})
</script>
<style scoped>
#codeEdit{
    width:700px;
    height: 300px;
}
.wrapper{
    width: 700px;
    height: 300px;
}
</style>