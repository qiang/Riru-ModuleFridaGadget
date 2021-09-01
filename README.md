#### 说明

firda gadget 模式支持如下四种模式：

   - Listen
   - Connect
   - Script
   - ScriptDirectory
   
我没有全部测试，根据使用目的不同，我现在只需要最后一种，主要用于大规模手机部署hook功能，为了把 libgadget.so 注入到进程，所以选择了 magisk + riru 的模式，通过自定义riru模块在riru的回调里面加载 libgadget.so 

[Riru-ModuleTemplate](https://github.com/RikkaApps/Riru-ModuleTemplate)


#### 目的 & 功能

- frida 持久化
- frida 代码能够hook同一个应用的不同进程
- 应用白名单（避免和其他hook框架冲突）
- 为了用于生产环境而不是调试环境

#### 适配Android版本

Android 9，Android 10 


#### 安装

- 通过 twrp 刷入 magisk v22.1
- 通过 magisk 刷入 riru ，目前测试过 v23.9 ~ v25.4.4 
- 通过 magisk 刷入 riru-FridaGadgetRiruMoudle-v14.2.12.8.zip

#### 编译 

gradle assembleRelease

#### 配置


##### 1、白名单

主要控制某个进程是不是要加载 `libgadget.so` ，防止和其他hook框架冲突

/data/local/tmp/_white_list.config

```
com.github.testapp1,com.github.testapp2
```
##### 2、gadget scriptdirectory 配置

https://frida.re/docs/gadget/#scriptdirectory

/data/local/tmp/frida_scripts

```
twitter.js
twitter.config
```

twitter.config  配置文件的目的是为了指定是否应该为某个 app 加载 twitter.js hook 脚本