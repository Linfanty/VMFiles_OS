#ifndef _SHELL_H_

#define _SHELL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

#define COMMAND_TYPE int  // 命令类别

/* 命令类别码 */
#define COMMAND_ERR     0
#define NORMAL          1
#define BACKSTAGE       2
#define HAVE_PIPE       3
#define IN_REDIRECT     4
#define OUT_REDIRECT    5

/* 输入命令及文件名字数限制 */
#define ARGLIST_NUM_MAX 32  // 最多有32个命令
#define COMMAND_MAX 256     // 一个命令最多有256个字符
#define FILE_PATH_MAX 256   // 文件路径最多有256个字符

/* 标准输入输出流 */
// 此处的 0 和 1 是在Linux文件描述符中规定的
#define STDIN   0   // 标准输入流
#define STDOUT  1   // 标准输出流

/* 创建命令控制节点 */
typedef struct commandNode {
    COMMAND_TYPE type;  // 用户输入的命令类型
    char cmd[COMMAND_MAX];  // 用户输入的命令
    char arg[ARGLIST_NUM_MAX][COMMAND_MAX]; // 存放所有分解命令的数组
    char *argList[ARGLIST_NUM_MAX + 1]; // 对用户输入的命令进行解析之后的字符串数组 , +1 是为了execvp操作,具体百度
    char *argNext[ARGLIST_NUM_MAX]; // 管道之后命令的字符串数组
    char file[FILE_PATH_MAX];   // 存放文件路径的数组
}CMD_NODE;

void initNode(CMD_NODE *cmdNode);    // 初始化控制节点
void input(char cmd[COMMAND_MAX]);  // 输入函数
void analysis_command(CMD_NODE *cmdNode);  // 将命令进行分解
void put_into_arr(char argList[ARGLIST_NUM_MAX][COMMAND_MAX], char *cmd);   // 将命令分解并放入数组中
void get_flag(char argList[ARGLIST_NUM_MAX][COMMAND_MAX], COMMAND_TYPE *flag);   // 得到命令类型
void other_work();   // 处理一些命令特有的事情
void run(CMD_NODE *cmdNode); // 按照不同类别运行命令

#endif  // !_SHELL_H_
