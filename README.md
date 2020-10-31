# MY-APUE

### 一.前言

##### 1.阅读心得

断断续续每天抽出下午的时间差不多用了3个月看完了这本书，期间也顺带学习了makefile、gcc、valgrind等工具（之前的学习总是局限在Windows的Visual Studio），可以说这本书让我成长了不少，当然自己也还需要走很长的路。

回到书中的内容，我认为看1-17章的内容即可，大致可以分成如下6个部分：1）**UNIX基础与标准**；2）**文件目录与I/O**；3）**进程**；4）**线程**；5）**守护进程和高级I/O**；6）**进程间通信**，全书脉络清晰，又有很多不错的实例，我相信每一个看完这本书的人都会受益匪浅。但是在一些内容上我觉得作为后起之秀的《Linux/UNIX系统编程手册》有很多不错的地方或者说更好，至少我看的进程间通信、epoll这些部分是这么觉得的。两本书一起结合参考着看我觉得是更好的选择。

希望朋友们带着批评的眼光看这个仓库。欢迎交流😂


##### 2.实验平台

```sh
       OS: Manjaro Linux x86_64
       Host: HP ENVY Notebook
       Kernel: 5.8.11-1-MANJARO
       Uptime: 6 hours, 27 mins
       Packages: 1204 (pacman)
       Shell: zsh 5.8
       Resolution: 1920x1080
       Terminal: /dev/pts/0
       CPU: Intel i5-6200U (4) @ 2.800GHz
       GPU: Intel Skylake GT2 [HD Graphics 520]
       GPU: NVIDIA GeForce GTX 950M
       Memory: 471MiB / 15911MiB
```

##### 3.使用指南

由于前面的章节自己仍然使用了书中给出的头文件和链接库，所以需要对原书中的提供的链接库进行编译。但是到了后面章节我自己重新写了自己的头文件[MyAPUE.h](./include/MyAPUE.h)并同时重新实现其中的内容，为此若要使用后面章节的程序，还需要编译我自己写的链接库。这些程序至少在我的Manjaro系统上都是🆗的！

编译原书中的链接库、头文件并放入到目录中：

```
$> cd apue.3e
$> make
$> mv libapue.a /usr/lib
$> cd ../include
$> mv apue.h /usr/include
```

将我的自己的链接库进行编译

```
$> #假设当前目录为仓库根目录
$> cd lib
$> make
```

后面每一章节的内容使用make命令就可以一步到位编译完毕

### 二.思维导图

1. [UNIX基础与标准](./mindmap/1、UNIX基础与标准.mmap)

2. [文件目录与I/O](./mindmap/2、IO、文件与目录.mmap)

3. [进程](./mindmap/3、进程.mmap)

4. [线程](./mindmap/4、线程.mmap)

5. [守护进程与高级I/O](./mindmap/5、守护进程与高级I.O.mmap)

6. [进程间通信](./mindmap/6、进程间通信.mmap)

### 三.章节内容

1. [UNIX基础知识](./chap01)

2. [UNIX标准与实现](./chap02)

3. [文件I/O](./chap03)

4. [文件与目录](./chap04)

5. [标准I/O库](./chap05)

6. [系统数据文件与时间](./chap06)

7. [进程环境](./chap07)

8. [进程控制](./chap08)

9. [进程关系](./chap09)

10. [信号](./chap10)

11. [线程](./chap11)

12. [线程控制](./chap12)

13. [守护进程](./chap13)

14. [高级I/O](./chap14)

15. [进程间通信](./chap15)

16. [网络IPC：套接字](./chap16)

17. [高级进程间通信](./chap17)

### 四.参考资料

1. *UNIX高级环境编程*
2. *Linux/UNIX系统环境编程*
3. *跟我一起写Makefile*
