# 终端登录的过程

```c
/*

yexinhai@localhost ~> pstree -a
systemd
  ├─ModemManager
  │   └─2*[{ModemManager}]
  ├─NetworkManager --no-daemon
  │   └─2*[{NetworkManager}]
  ├─bumblebeed --use-syslog
  ├─cupsd -l
  ├─dbus-daemon --system --address=systemd: --nofork --nopidfile --systemd-activation --syslog-only
  ├─login
  │   └─fish
  ├─lvmetad -f
  ├─nginx
  │   └─nginx
  ├─polkitd --no-debug
  │   └─7*[{polkitd}]
  ├─sshd
  │   └─sshd
  │       └─sshd
  │           └─fish
  │               ├─pstree -a
  │               └─{fish}
  ├─systemd --user
  │   └─(sd-pam)
  ├─systemd --user
  │   └─(sd-pam)
  ├─systemd-journal
  ├─systemd-logind
  ├─systemd-timesyn
  │   └─{systemd-timesyn}
  ├─systemd-udevd
  └─wpa_supplicant -u -s -O /run/wpa_supplicant

	其中上面所有的进程关系中，					“└─”这个符号表示fork-exec的过程
						  parent process
								├─child process			
	所以在登录终端的过程中，
	1）systemd fork-exec出 agtty进程；
	2）而agtty进程又exec出 login进程
	3）若登录成功，则login exec出 shell进程，(注意是exec而不是fork-exec，虽然执行pstree aux显示的是|--但是要知道
	   系统上是有默认6个tty的，所以即使登陆了一个账户使其中一个login exec出shell后，系统上仍然还是有5个login进程
	   在运行)。同时需要注意在login进程exec出shell进程之前，login会调用setuid函数将当前进程的ruid、euid、保留suid
	   全部设置为用户的uid
	4) 否则login调用exit返回让父进程systemd取得退出状态，然后systemd重新fork-exec出agtty，而agtty又重新exec
	   出login，让用户继续尝试登录
							
*/
```
