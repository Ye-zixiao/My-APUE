# 终端登录的过程

```c
/*

yexinhai@localhost ~/m/chap9 (master)> pstree -Aups
systemd(1)-+-ModemManager(728)-+-{ModemManager}(745)
           |                   `-{ModemManager}(749)
           |-NetworkManager(683)-+-{NetworkManager}(705)
           |                     `-{NetworkManager}(707)
           |-bumblebeed(681)
           |-cupsd(781)
           |-dbus-daemon(682,dbus)
           |-login(24607)---fish(28153)-+-tar(29312)---xz(29314)    
           |                            `-tar(29313)---xz(29315)
           |    //systemd-----(fork-exec)----->getty-----(exec)----->login-----(fork-exec)----->shell的过程
           |-login(28328)---bash(28341,hll)
           |-lvmetad(312)
           |-nginx(782)---nginx(783,http)
           |-polkitd(685,polkitd)-+-{polkitd}(704)
           |                      |-{polkitd}(706)
           |                      |-{polkitd}(712)
           |                      |-{polkitd}(713)
           |                      |-{polkitd}(714)
           |                      |-{polkitd}(715)
           |                      `-{polkitd}(723)
           |-sshd(709)---sshd(24481)---sshd(24495,yexinhai)---fish(24496)-+-pstree(29348)
           |                                                              `-{fish}(29345)
           |-systemd(28141)---(sd-pam)(28142)
           |-systemd(24484,yexinhai)---(sd-pam)(24485)
           |-systemd(28330,hll)-+-(sd-pam)(28331)
           |                    |-dbus-daemon(28347)
           |                    |-gvfsd(28349)-+-{gvfsd}(28350)
           |                    |              `-{gvfsd}(28351)
           |                    `-gvfsd-fuse(28354)-+-{gvfsd-fuse}(28367)
           |                                        |-{gvfsd-fuse}(28368)
           |                                        |-{gvfsd-fuse}(28369)
           |                                        |-{gvfsd-fuse}(28370)
           |                                        `-{gvfsd-fuse}(28371)
           |-systemd-journal(245)
           |-systemd-logind(688)
           |-systemd-timesyn(674,systemd-timesync)---{systemd-timesyn}(679)
           |-systemd-udevd(287)
           `-wpa_supplicant(763)

	其中上面所有的进程关系中，					“└─”这个符号表示fork-exec的过程
						  parent process
								├─child process			
	所以在登录终端的过程中，
	1）systemd fork-exec出 agtty进程(注意只有按下ctrl+alt+Fi按键组合的时候才会让systemd真
       的fork-exec出getty进程，除非是进入系统后的tty1)
	2）而agtty进程又exec出 login进程
	3）若登录成功，则login fork-exec出 shell进程（在fork后exec前子进程回先调用setunid等函数
       降低子进程的运行特权，更改resuid）
    4) 否则login调用exit返回让父进程systemd取得退出状态，然后systemd重新fork-exec出agtty，
       而agtty又重新exec出login，让用户继续尝试登录
							
		特别的这里的login是否是通过fork-exec出shell还是直接exec出shell不确定，书上说是exec，
	但实验通过pstree我觉得是fork-exec，影响就是shell的父进程是systemd还是login的问题，当然也
	有可能是时代变化的关系，这一点我不敢确定
*/
```
