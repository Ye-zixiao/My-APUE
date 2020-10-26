#### 1.程序结构

```
        客户进程                                                           服务进程
                                                                            
csopen():                                                           loop():
         |                                                                  |              
         |                                                                  v   
         |                                                             serv_listen()
         |                                                               监听连接请求
         |                                                                  |
         v                                                                  v
      cli_conn() <---->发送建立连接请求                          <----->  serv_accept()
         |                                                          接受连接请求并创建新的与
         |                                                            客户进程连接的套接字
         |                                                                  |
         v                                                                  v
      writev() ----->发送打开文件请求                                     handle_request()--|
         |                                                                              cli_args()
         |                                                                                  |
         |                                                                              buf_args()
         |                                                                                  |
         |                                                                                open()
         v                                                                                  |
      recv_fd() <-----接收文件描述符或             向客户进程返回文件描<----- send_fd() <---
                         打印出错信息                 述符或出错信息             |
         |                                                                       v
         v                                                                重新执行内部的for
      write()输出文件中的内容                                                  循环
 
 *：不考虑断开连接的情况

```



#### 2.文件结构

```
客户端程序文件结构：
   MyAPUE.h------>open.h
                    ------->client.c    客户端主程序源文件
                    ------->open.c      csopen()函数实现源文件
                    
   MyAPUE.h------>opend.h
                    ------->server.c    服务端主程序源文件
                    ------->do_args.c   负责处理参数列表函数实现源文件
                    ------->do_client.c 处理客户信息结构体动态数组相关函数实现源文件
                    ------->sloop.c     使用select实现感兴趣文件描述符监控
                    ------->ploop.c     使用poll实现感兴趣文件描述符监控
                    ------->eloop.c     使用epoll实现感兴趣文件描述符监控
            
```

