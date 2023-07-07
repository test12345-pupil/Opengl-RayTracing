# Opengl RayTracing

主要参考：

https://blog.csdn.net/weixin_44176696/

aranara.obj模型链接：

https://sketchfab.com/3d-models/aranara-genshin-impact-ad022912f9144aa18ff19a9207573add

hall01.obj模型链接：

https://sketchfab.com/3d-models/sitting-room-with-baked-textures-61fdbbef86b9496d80644492a84c916f

GLSL主要参考：

https://www.wangshaoxing.com/blog/2018-11-06-shader-lession-1.html
https://geek-docs.com/vulkan/glsl/structs-and-arrays.html

代码环境配置：

添加了我们组自己写的scene.h、BVHnode.h等.h文件，在main.cpp中调用了这些函数，包括我们实现的BVH树，AABB盒等。
模型加载方面使用的是我们写的objloader.h文件，里面定义了loadOBJ函数，可以把obj文件解读为加载的模型。

代码运行：

代码通过CMake编译，之后可执行文件位于./build/src/Hand.exe，可以直接打开该文件执行程序

操作方式：
通过W、A、S、D控制摄像机镜头前后左右移动，通过空格、Ctrl控制摄像机上下移动，按住Alt移动鼠标的时候可以控制摄像机镜头的方位