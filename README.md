# sift
本程序在Linux下编写
## 程序依赖
- 最好在Linux系统下编译运行,实测在mac系统下会报 'segmatation fault: 11' 
- 通过cmake编译程序生成可执行文件sift,cmake安装方法见下面的说明
## 程序运行说明
1. 输入1.bmp与2.bmp, 输出point1.bmp 与 point2.bmp, 中间过程输出level-1到4的尺度空间图
2. 如果输入为jpg格式的图,请使用jpg2bmp.py 转为bmp格式的图
3. cmake编译后,运行 './sift' 命令

## 编译方法:
1. 安装cmake
2. 在终端下输入 cmake .
3. 在终端下输入 make
