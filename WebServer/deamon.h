#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>

void create_deamon() {
    int fd0;
    pid_t retCode = fork(); //创建子进程，每个进程都会返回一个数字，父进程返回子进程的pid，子进程返回0
    if(retCode < 0) {
        perror("fork");
        exit(1);
    }
    if(retCode > 0) {
        exit(0);    //父进程退出
    }
    if(retCode = setsid() == -1) {        //创建新会话，该子进程会成为新的会话和进程组的组长
        perror("setsid");
        exit(1);
    }
    retCode = fork(); //再fork一次，新的子进程不再是会话组长
    if(retCode < 0) {
        perror("fork");
        exit(1);
    }
    if(retCode > 0) {
        exit(0);
    }
    chdir("/"); //改变进程的工作目录
    umask(0);    //设置进程的文件操作权限，使进程拥有最大权限
    for(int fd = 0; fd < getdtablesize(); fd++) {   
        close(fd);  //将父进程打开的文件描述符关闭
    }
    open("/dev/null", O_RDWR);
}