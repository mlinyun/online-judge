import { fileURLToPath, URL } from 'node:url'

import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import monacoEditorPlugin from 'vite-plugin-monaco-editor'

// https://vitejs.dev/config/
export default defineConfig({
	plugins: [
		vue(),
		monacoEditorPlugin(),
	],
	resolve: {
		alias: {
		'@': fileURLToPath(new URL('./src', import.meta.url))
		}
	},
	server: {
		proxy: {
			'/api': {
				target: 'http://127.0.0.1:8081', // 后端请求地址
				changeOrigin: true,
				rewrite: (path) => path.replace(/^\/api/, ""),
			},
		},
		port: '8080', // 端口
		host: '0.0.0.0'
	},
	lintOnSave:false //关闭语法检查
})
