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
      writev() ----->发送打开文件请求                                     handle_request()---|
         |                                                                              cli_args()
         |                                                                                  |
         |                                                                              buf_args()
         |                                                                                  |
         |                                                                                open()
         v                                                                                  |
      recv_fd() <-----接收文件描述符或              向客户进程返回文件描<----- send_fd() <----
                         打印出错信息                 述符或出错信息             |
         |                                                                      v
         v                                                               重新执行内部的for
      write()输出文件中的内容                                                  循环
 
 *：不考虑断开连接的情况
```
