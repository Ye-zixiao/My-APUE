#### 1.程序结构

```

    父进程  fds[0] <---------------->  使用UNIX域套接字  <--------x-------->    子进程
                   <--------x------->  建立的全双工管道  <----------------->  fds[1]/stdin
      csopen():                                                               
              writev()  --->发送请求                               ------>  stdin接收请求
                                                                                |
                                                                                v
                                                                            handle_request()--->buf_args()
                                                                                 |          <---cli_args()
                                                                                 v
                                                                               open()
                                                                                 |
                                                                                 v
             recv_fd() <---接收文件描述符/                          <-----    send_fd()
                           输出出错信息

```

#### 2.认识strtok()

说实话，我是你第一次使用这个函数😂！

##### 2.1函数原型

相关头文件：string.h

**char *strtok(char *str, const char *delim);**

- 其中str指向你想进行分割的字符串
- delim指向分割字符数组


##### 2.2函数用法

例如我们想将字符串"hello\tworld show\nme+your-code"字符串进行分割提取出其中的子字符串，那么我们可以使用分割字符数组" \t\n+-"传入到delim参数中，而上面我们我们想进行分割的字符串需要传入到str参数。
    不过我们需要注意的是当我们第一次想从上述的字符串中分割出子字符串时是需要传入到str之中，然后strtok()函数会从中分割返回指向第一个字符串hello的指针。但是当我们想在上述的字符串的基础上再进行分割的时候就需要向这个函数中str参数传入NULL，而delim参数依然需要传入分割字符数组的指针。此后我们就可以使用循环调用strtok()获得剩余的子字符串的起始地址，进行打印或者保存，知道搜寻到字符串的尾端返回NULL。

代码示例：

```c
/**
 * 我们的目的仍然是从上的字符串中"hello\tworld show\nme+your-code"
 * 分割出子字符串单词
 */
#include <stdio.h>
#include <string.h>

int main(void){
    char str[64] = "hello\tworld show\nme+your-code";
    char delim[6] = " \t\n-+";
    char *substr = NULL;

    substr = strtok(str, delim);
    if(substr == NULL)
        return 0;
    printf("%s\n", substr);
    while((substr = strtok(NULL, delim)) != NULL){
        printf("%s\n",substr);
    }

    return 0;
}
```

结果：

```
$ ./strtok      
hello
world
show
me
your
code
```

