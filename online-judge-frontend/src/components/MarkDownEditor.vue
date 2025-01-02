<template>
    <div>
        <v-md-editor 
        v-model="content"
        height="500px"
        left-toolbar="undo redo clear | h bold italic strikethrough quote | tip emoji | ul ol table hr | link image code | save " 
        ></v-md-editor>
    </div>
</template>

<script setup>
import {ref} from 'vue'

const content = ref('')

function GetContent()
{
    return content.value
}
function SetContent(value)
{
    content.value = value
}
defineExpose({
    GetContent,
    SetContent
})
</script>