#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include "command_controller.h"
const int MAX=100;
int g_flag=0;
struct dir{
	char pwd[100];
	char path[100];
	int flag;
};
struct stack{
	int start;
	int pid[100010];
};
void signal_handler2(){
	g_flag=1;
}
pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int shell_is_interactive;
int main(){
	char *list[MAX];
	char username[MAX];
	char hostname[MAX];
	char *names[32769];
	char *stack_names[32769];
	int back[32769];
	char temp[MAX],temp1[MAX],temp2[MAX],temp3[MAX],check[MAX],*list_pipe[MAX],piped=0;
	int i,j,k,no,co,to,mo,oo,t,in_quotes=0,status,pipes[MAX],stack[MAX],append,tempp;
	int head=1;
	struct dir curr;
	curr.flag=0;
	struct dir exec_out;
	strcpy(curr.path,"~");
	getcwd(check,MAX);
	strcpy(curr.pwd,check);
	strcat(curr.pwd,"/");
	getlogin_r(username,MAX);
	gethostname(hostname,MAX);
	exec_out=curr;
	signal (SIGINT, SIG_IGN);
	for(i=0;i<100;i++)stack[i]=-1;
	while(1){
		pid_t t;
		while((t=waitpid(-1,&status,WNOHANG))>0){
			printf("%s with pid %d has exited normally\n",names[t],t);
			for(i=0;i<head;i++){
				if(stack[i]==t){
					tempp=i;
					break;
				}
			}
			for(j=tempp+1;j<head;j++){stack[j-1]=stack[j];stack_names[j-1]=NULL;stack_names[j-1]=strdup(stack_names[j]);}head--;
		}
		printf("<%s@%s:%s>",username,hostname,curr.path);
		if(fgets(temp,MAX,stdin)=='\0')return 0;
		no=0;co=0;
		list[no]=strtok(temp,";");
		while(1){
			if(list[no]==NULL)break;
			no++;
			list[no]=strtok(NULL,";");
		}
		list[no-1][strlen(list[no-1])-1]='\0';
		piped=0;
		while(co<no)
		{
			mo=0;oo=0;piped=0;
			strcpy(temp2,list[co]);
			list_pipe[mo]=strtok(temp2,"|");
			while(1){
				if(list_pipe[mo]==NULL)break;
				mo++;
				list_pipe[mo]=strtok(NULL,"|");
			}
			if(mo>1){
				for(k=0;k<(mo*2);k+=2)pipe(pipes+k);
				piped=1;
			}
			while(oo<mo){
				j=0;
				int flag=0;
				if(oo!=(mo-1)){if(list_pipe[oo][strlen(list_pipe[oo])-1]==' ')list_pipe[oo][strlen(list_pipe[oo])-1]='\0';}
				strcpy(temp3,list_pipe[oo]);
				for(i=0;i<strlen(temp3);i++){
					if(flag==0 && temp3[i]== ' ')continue;
					else{flag=1;temp1[j++]=temp3[i];}
				}
				temp1[j]='\0';
				append=0;
				to=1;
				int flagg=0;
				for(i=0;i<strlen(temp1);i++){if(temp1[i]=='<' || temp1[i]=='>'){to++;if(flagg==0)flagg=i;}}
				char **red_list=(char**)malloc(to*sizeof(char*));
				char red_symbol[to];
				to=0;
				j=0;
				red_list[0]=(char*)malloc(MAX);
				for(i=0;i<strlen(temp1);i++){
					if(temp1[i]!='<' && temp1[i]!='>' && temp1[i]!=' ')red_list[to][j++]=temp1[i];
					if(temp1[i]==' ' && i<flagg)red_list[to][j++]=temp1[i];
					if (temp1[i]== '<' || temp1[i]=='>'){
						red_symbol[to]=temp1[i];red_list[to][j]='\0';to++;red_list[to]=(char*)malloc(MAX);j=0;
					}
				}
				red_list[to][j]='\0';
				red_symbol[to]='\0';
				if(red_symbol[0]=='\0'){
					if(strlen(temp1)>0)
						curr=execute(temp1,check,curr,names,stack,&head,stack_names,'\0','\0',piped,oo,mo,pipes,append);
				}
				else{
					i=0;
					if(red_symbol[i]=='<' && (i+1)>=strlen(red_symbol))
						curr=execute(red_list[0],check,curr,names,stack,&head,stack_names,red_list[1],'\0',piped,oo,mo,pipes,append);
					else if (red_symbol[i]=='<' && (i+1)<strlen(red_symbol) && red_symbol[i+1]=='>')
						curr=execute(red_list[0],check,curr,names,stack,&head,stack_names,red_list[1],red_list[2],piped,oo,mo,pipes,append);
					else if(red_symbol[i]=='>' && (i+1)>=strlen(red_symbol))
						curr=execute(red_list[0],check,curr,names,stack,&head,stack_names,'\0',red_list[1],piped,oo,mo,pipes,append);
					else if(red_symbol[i]=='>' && (i+1)<strlen(red_symbol) && red_symbol[i+1]=='>'){
						append=1;
						curr=execute(red_list[0],check,curr,names,stack,&head,stack_names,red_list[1],red_list[2],piped,oo,mo,pipes,append);
					}
				}
				if(curr.flag==1)return 0;
				if(piped==1 && oo==(mo-1)){
					for(i=0;i<(oo*2);i++)close(pipes[i]);
					pid_t pid;
					while(waitpid(pid,&status,0)>-1);
				}
			oo++;
			}
			co++;
		}
	}
	return 0;
}
