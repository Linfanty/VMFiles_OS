myshell
    --王天阳

程序目标:
    单个命令:   ls
    带-l到多个参数的命令:   ls -l /temp
    带一个输出重定向的命令: ls -l > a
    带一个输入重定向的命令: wc -c < a
    带一个管道命令: ls -l | wc -c
    后台运行符可以添加到命令的最后---面:   ls &
    
    a）cd <directory>——把当前默认目录改变为<directory>。如果没有<directory>参数，则显示当前目录。如该目录不存在，会出现合适的错误信息。这个命令也可以改变PWD 环境变量。

    b）clr——清屏。

    c）dir <directory>——列出目录<directory>的内容。

    d）environ——列出所有的环境变量。

    e）echo <comment>——在屏幕上显示<comment>并换行（多个空格和制表符可能被缩减为一个空
    格）。

    f）help——显示用户手册，并且使用more 命令过滤。

    g）pause——停止shell 操作直到按下回车键。

    h）quit——退出shell。

注意:
    1. 在编译的时候使用root权限编译    
        编译命令:
            make
    2. 在运行的时候使用root权限编译    
        运行命令: 
            sudo 
            ./myshell
        或者
            su ./myshell 
    3. 没有命令报错
    4. 语法限制不严格, 可能会出现一些出乎意料的结果
    5. 在analysis_command中可以自定义

在开发过程中, 出现了一些挫折, 将其记录下来:
1.
{
    源代码:
    void input(CMD_NODE *cmdNode) {
    int index = 0;

    gets(cmdNode->cmd);
    if(strlen(cmdNode->cmd) >= COMMAND_MAX) {
        printf("error: command too long!\n");
    }

    while(cmdNode->cmd[index] != '\0') {
        if(!cmdNode->falg) {
            if(cmdNode->cmd[index] == '|') {    // 将命令的类型置为管道命令
                cmdNode->falg = HAVE_PIPE;
            }
            if(cmdNode->cmd[index] == '>') {    // 将命令的类型置为输出重定向
                cmdNode->falg = OUT_REDIRECT;
            }
            if(cmdNode->cmd[index] == '<') {    // 将命令的类型置为输入重定向
                cmdNode->falg = IN_REDIRECT;
            }
            if(cmdNode->cmd[index] == '&') {    // 将命令的类型置为后台运行
                cmdNode->flag = BACKSTAGE;
            }
        }else {
            // 如果命令正确, 则该判断只会运行一次
            printf("error: have too many args\n");
        }
    }

    在最初的设计中. 考虑将识别类型加入到input函数中
    但是最终放弃
    原因1: 违背了函数单一职责原理
    原因2: 使得程序的效率降低
}
2.
{
    原函数声明
    void input(CMD_NODE *cmdNode);  // 输入函数,并进行解析命令类别
    void analysis_command(CMD_NODE *cmdNode);  // 将命令进行分解
    void run(CMD_NODE *cmdNode); // 按照不同类别运行命令

    原因:
    为了保证数据的安全性,并且使上述三个函数代码量减少且逻辑更清晰,决定只在集合许多功能的函数上使用CMD_NODE
    ,具体操作函数使用普通类型参数
}
3.
{
    考虑是否要加入argNum(参数个数)成员
    最后决定不加入
    原因:
    可以通过数组是否为空来判断参数的个数
}
4. 
{
    原结构体
    typedef struct commandNode {
        COMMAND_TYPE type;  // 用户输入的命令类型
        char cmd[COMMAND_MAX];  // 用户输入的命令
        char argList[ARGLIST_NUM_MAX][COMMAND_MAX]; // 对用户输入的命令进行解析之后的数组
        char argNext[ARGLIST_NUM_MAX][COMMAND_MAX]; // 管道命令之后的解析数组
        char file[FILE_PA TH_MAX];   // 存放文件路径的数组
    }CMD_NODE;
    马上竣工的时候发现了致命错误...将argList 和 argNext设计成二维数组了, 其实应当用char *argList[]
    决定使用这样的改正方法 存在三个数组:argList argNext arg 其中arg是arg二维数组, 而其他两个为字符串数组
}