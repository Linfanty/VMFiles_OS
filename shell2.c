#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>


int main()
{
	char cmd[128];

	while(1)
	{
		printf("[user@localhost myshell]$ ");
		fflush(stdout);

		ssize_t _s = read(0,cmd,sizeof(cmd)-1);   //读取命令
		if(_s > 0)
		{
			cmd[_s - 1] = '\0';
		}
		else
		{
			perror("read");
			return 1;
		}

		char *_argv[32];    //用指针数组接收命令
		_argv[0] = cmd;
		int i = 1;

		char *start = cmd;
		while(*start)
		{
			if(isspace(*start))   //每个命令以空格结束
			{
				start = '\0';
				start++;
				_argv[i] = start;
				i++;
				continue;
			}
			start++;
		}

		_argv[i] = NULL;
		pid_t id = fork();   //创建子进程
		if(id < 0)
		{
			perror("fork");
		}
		else if(id == 0)
		{
			execvp(_argv[0],_argv);   //将接收到的命令传给系统
			exit(1);
		}
		else
		{
			int status = 0;
			pid_t ret = waitpid(id,&status,0);
			if(ret > 0 && WIFEXITED(status))
			{}
			else
			{
				perror("waitpid");
			}
		}
	}

	return 0;
}
