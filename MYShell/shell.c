#include "shell.h"

void initNode(CMD_NODE *cmdNode);    // 初始化控制节点
void input(char cmd[COMMAND_MAX]);  // 输入函数
void analysis_command(CMD_NODE *cmdNode);  // 将命令进行分解
void put_into_arr(char argList[ARGLIST_NUM_MAX][COMMAND_MAX], char *cmd);   // 将命令分解并放入数组中
void get_flag(char argList[ARGLIST_NUM_MAX][COMMAND_MAX], COMMAND_TYPE *flag);   // 得到命令类型
void other_work();   // 处理一些命令特有的事情
void run(CMD_NODE *cmdNode); // 按照不同类别运行命令

void run(CMD_NODE *cmdNode) {
    pid_t pid = 0;
    
    pid = fork();   // 创建子进程
    
    if(pid < 0) {
        printf("创建子进程失败");
        exit(-1);
    }
    if(pid == 0) {  // 子进程
        if(cmdNode->type == NORMAL || cmdNode->type == BACKSTAGE) {
            execvp(cmdNode->argList[0], cmdNode->argList);
            exit(0);
        }
        if(cmdNode->type == IN_REDIRECT) {
            int fd = open(cmdNode->file, O_RDONLY|O_CREAT);
            
            dup2(fd, STDIN);   // 将输入流切换成指定的文件路径
            execvp(cmdNode->argList[0], cmdNode->argList);
            exit(0);
        }
        if(cmdNode->type == OUT_REDIRECT) {
            int fd = open(cmdNode->file, O_WRONLY|O_CREAT, O_TRUNC);    // 只写模式打开, 存在,则并且清空以前记录 不存在,则创建
            
            dup2(fd, STDOUT);   // 将输出流切换成指定的文件路径
            execvp(cmdNode->argList[0], cmdNode->argList);
            exit(0);
        }
        if(cmdNode->type == HAVE_PIPE) {
            pid_t pid2 = 0;
            int fd = 0;
            
            pid2 = fork();  // 再创建一个进程, 子子进程运行管道前命令, 子进程运行管道后命令
            
            if(pid2 < 0) {
                printf("管道命令运行失败\n");
                exit(-1);
            }
            
            if(pid2 == 0) { // 子子进程部分
            printf("进入子子进程\n");
            fd = open("/tmp/shellTemp", O_WRONLY|O_CREAT|O_TRUNC, 0644);
            dup2(fd, STDOUT);
            execvp(cmdNode->argList[0], cmdNode->argList);
            close(fd);
            exit(0);
            }

            if(waitpid(pid2, 0, 0) == -1) { // 子进程等待子子进程结束运行
                printf("error: 管道命令运行失败\n");
                exit(-1);
            }
            
            fd = open("/tmp/shellTemp", O_RDONLY);
            dup2(fd, STDIN);
            execvp(cmdNode->argNext[0], cmdNode->argNext);
            exit(0);
        }
    }
    if(cmdNode->type == BACKSTAGE) {
        printf("子进程pid为%d\n", pid);
        return ;
    }
    if(waitpid(pid, 0, 0) == -1) {
        printf("等待子进程失败\n");
    }
}

void other_work(CMD_NODE *cmdNode) {
    int index = 0;
    int type = cmdNode->type;

    while(cmdNode->arg[index][0] != '\0') {
        cmdNode->argList[index] = cmdNode->arg[index];
        index++;
    }

    if(type == BACKSTAGE) { // 带有后台运行命令
        cmdNode->argList[index - 1] = NULL; // 将后台运行&屏蔽掉
    }

    index = 0;  // 使得下面代码能够正常使用index   
    
    if(type == OUT_REDIRECT || type == IN_REDIRECT) { // 带有重定向命令
        while(strcmp(">", cmdNode->argList[index]) && strcmp("<", cmdNode->argList[index])) {
                index++;
        }

        index++;    // 跳过'>'或者'<'
        
        strcpy(cmdNode->file, cmdNode->argList[index]);
        cmdNode->argList[index - 1] = NULL;  // 使得argList只存放>或者<之前的命令
        
        return ;
    }

    if(type == HAVE_PIPE) {    // 带有管道命令
        int indexNext = 0;

        while(strcmp("|", cmdNode->argList[index++]));  // 便利到'|'这儿
        cmdNode->argList[index - 1] = NULL;  // 使得argList只存放|之前的命令
        while(cmdNode->arg[index][0] != '\0')  {    // 将管道后命令存放到argNext数组中
            cmdNode->argNext[indexNext++] = cmdNode->arg[index++];   
        }

        return ;
    }
}

void get_flag(char arg[ARGLIST_NUM_MAX][COMMAND_MAX], COMMAND_TYPE *flag) {
    int argIndex = 0;
    int count = 0;

    while(arg[argIndex][0] != '\0') {
        if(strcmp("|", arg[argIndex]) == 0) {    // 将命令的类型置为管道命令
            *flag = HAVE_PIPE;
            count++;
        }
        if(strcmp(">", arg[argIndex]) == 0) {    // 将命令的类型置为输出重定向
            *flag = OUT_REDIRECT;
            count++;
        }
        if(strcmp("<", arg[argIndex]) == 0) {    // 将命令的类型置为输入重定向
            *flag = IN_REDIRECT;
            count++;             
        }
        if(strcmp("&", arg[argIndex]) == 0) {    // 将命令的类型置为后台运行
            *flag = BACKSTAGE;
            count++;                
        }
        argIndex++;
    }
    
    if(count > 1) { // 命令类型重复定义
        printf("error: have too many args\n");
        exit(-1);
        *flag = COMMAND_ERR;
    }
}

void put_into_arr(char arg[ARGLIST_NUM_MAX][COMMAND_MAX], char *cmd) {
    int argIndex = 0;
    int index = 0;

    while(*cmd != '\0') {
        if(*cmd == ' ') {
            argIndex++;
            index = -1;
        }else {
            arg[argIndex][index] = *cmd;
        }
        cmd++;
        index++;
    }
}

void analysis_command(CMD_NODE *cmdNode) {
    /* 自定义命令 */
    //if(strcmp(cmdNode->cmd, "exit") == 0) {
    //    exit(-1);
    //}
    //if(strcmp(cmdNode->cmd, "pause") == 0) {
    //    char ch;
    //    while((ch = getchar()) == '\r');    // 在linux中的pause和windows里的不一样
    //}
    // printf("wtywty%s\n", cmdNode->cmd );
    if( strcmp(cmdNode->cmd, "quit\n") == 0 ) {
        system("exit"); 
    }
    if( strcmp(cmdNode->cmd, "clr\n") == 0 ) {
        system("clear"); // 清屏
    }
    if( strcmp(cmdNode->cmd, "help\n") == 0 ) {
        system("man");// 显示用户手册
    }
    if(  strcmp(cmdNode->cmd, "environ\n") == 0 ) {
        system("set"); // 列出所有环境变量
    }
    if(strcmp(cmdNode->cmd, "exit\n") == 0) {
        exit(-1);
    }
    system(cmdNode->cmd);
    /* 结束 */
    put_into_arr(cmdNode->arg, cmdNode->cmd); 
    get_flag(cmdNode->arg, &(cmdNode->type));
}

void input(char cmd[COMMAND_MAX]) {
    fgets(cmd, COMMAND_MAX, stdin);
    if(strlen(cmd) >= COMMAND_MAX - 1) {
        printf("error: command too long!\n");
        exit(-1);
    }
}

void initNode(CMD_NODE *cmdNode) {
    memset(cmdNode, 0, sizeof(CMD_NODE));
    cmdNode->type = NORMAL;
}
