#include "shell.h"

void show(CMD_NODE *cmdNode);

int main(void) {
    CMD_NODE cmdNode = {0};
    while(1) {
        printf("WangTianYang's myshell:");
        initNode(&cmdNode);
        input(cmdNode.cmd);
        if( strcmp(cmdNode.cmd, "quit\n") == 0
         ||  strcmp(cmdNode.cmd, "exit\n") == 0 ) {
            break;
        }
        analysis_command(&cmdNode);  
        other_work(&cmdNode);
        run(&cmdNode);
    }

    return 0;
}

void show(CMD_NODE *cmdNode) {
    printf("类型:%d\n", (int)cmdNode->type);
    printf("命令:%s\n", cmdNode->cmd);
    printf("argList\n");
    for(int i = 0; cmdNode->argList[i] != NULL; i++) {
        printf("[%d]:%s\n", i, cmdNode->argList[i]);
    }
    printf("argNext\n");
    for(int i = 0; cmdNode->argNext[i] != NULL; i++) {
        printf("[%d] : %s\n", i, cmdNode->argNext[i]);
    }
    printf("file:%s\n", cmdNode->file);
}