使用串口命令操作SD卡
1.初始化：选择板载flash，还是spi sd模块
  init flash/sd  example:[init flash]
2.打印所有目录：
  ls
3.打印特定目录：
  ls dir
4.移除特定目录，并返回是否移除成功
  rm /dir
5.移除指定文件  
  rm filepath
6.创建文件，并返回是否创建成功
  touch filepath
7.创建目录，并返回是否创建成功
  mkdir dirpath
8.移除目录下的所有文件
  rm -f dir
9.访问文件内容
10.写文件
11.获取文件信息

python文件测试自动化，通过串口交互

