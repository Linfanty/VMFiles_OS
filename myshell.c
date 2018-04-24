/**********************************************************
myshell-basic shell replacement
>my shell
从键盘输入一行字符串，分割字符串并解析为cmd命令
clr 清屏
ls <directory> 列出<directory>目录下面的文件，默认为当前目录
environ 环境变量 
quit 退出
**********************************************************/

#include <myshell.h>

extern char **environ;       

/**********************************************************/

int main(int argc,char** argv)
{
	//system("title MyShell");
	char linebuf[MAX_BUFFER];
	char cmndbuf[MAX_BUFFER];
	char *args[MAX_ARGS];
	char **arg;
	char* prompt="==>";
	while(!feof(stdin))
	{
		system("pwd");
		fputs(prompt,stdout);
		fflush(stdout);
		//输入参数
		if(fgets(linebuf,MAX_BUFFER,stdin))
		{
			arg=args;
			*arg++=strtok(linebuf,SWEPARATOR);//按空格,换行符，制表符等分割字符串		
			while((*arg++=strtok(NULL,SWEPARATOR)));
			if(args[0])//第一个参数存在
			{
				//printf("args[0]:%s\n",args[0]);
				cmndbuf[0]=0;
				//printf("cmndbuf[0]:%c\n%s\n",cmndbuf[0],args[0]);
				//检查是否为系统已经存在的命令
				if(!strcmp(args[0],"clr"))
				{
					strcpy(cmndbuf,"clear");
				}
				else if(!strcmp(args[0],"dir"))
				{					
					strcpy(cmndbuf,"ls ");
					if(!args[1])
					{
						args[1]=".";//没输入参数默认为当前目录
					}
					strcat(cmndbuf,args[1]);
					//printf("%s",cmndbuf);
				}
				else if(!strcmp(args[0],"help"))
                    {
                        strcpy(cmndbuf,"more ./readme");//显示当前目录下的readme
                    }
				else if(!strcmp(args[0],"pause"))
                     {
					//printf("Press enter to continue...\n");//提示暂停，直到按下回车键
					strcpy(cmndbuf,"read -n 1 -p \"Press any key to continue...\"");
				//	char temp=malloc(sizeof(char));
				//	scanf("%c",&temp);
				//	while(strcmp(temp,'\n'))
				//	{
				//		scanf("%c",&temp);
				//	}
										
                    }

				else if(!strcmp(args[0],"cd"))
                    {
                        if(!args[1])
                                args[1]=".";//没输入参数默认为当前目录
					int result=chdir(args[1]);
					if(result<0)
					{printf("Error:directory not exist!\n");}
 				}                                        
				else if(!strcmp(args[0],"environ"))
				{
          			char** envstr=environ;
                    while(*envstr)
        		    {
                          printf("%s\n",*envstr);
        		          envstr++;
                    }
					
				}
				else if(!strcmp(args[0],"quit"))
				{
					break;//return 0,退出循环
				}
				else      //其他命令
				{	
					int i=1;
					strcpy(cmndbuf,args[0]);
					while(args[i])
					{
						strcat(cmndbuf," ");
						strcat(cmndbuf,args[i++]);
					}
				}


				//调用系统命令
				if(cmndbuf[0])
				{
					system(cmndbuf);
				}
	
			}
		}
	}	
} 