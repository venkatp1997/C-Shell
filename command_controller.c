#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include "cd.h"
#include "echo.h"
#define MAX 100
struct dir{
	char pwd[100];
	char path[100];
	int flag;
};
int g_flag,g_flagg,g_pid,g_head,g_wait,q[32769];
void signal_handler(){
	g_flag=1;
}
void signal_handler1(){
	g_flagg=1;
}
struct dir execute(char *temp1, char *check, struct dir curr, char *names[32769],int stack[],int *head,char *stack_names[32769],char *inname,char *outname,int piped,int oo,int mo,int pipes[],int append){
	char background;
	char *command;										
	char *argument;
	int i,j,t,flag=0,status,in,out;
	int check_len=strlen(check);								
	struct dir exec_out;
	exec_out=curr;
	pid_t pid;
	background=temp1[strlen(temp1)-1];
	char *copy=strdup(temp1);
	command=strtok(temp1," ");
	g_flag=0;
	g_flagg=0;
	if(strcmp(command,"cd")==0){
		char *arg;
		arg=strtok(NULL," ");
		if(arg==NULL){chdir(check);strcpy(curr.pwd,check);strcpy(curr.path,"~");}
		else{
			curr=cd(arg,curr,check_len,check);
			chdir(curr.path);
		}
		exec_out=curr;
	}
	else if(!strcmp(command,"echo")){
		char *str=strdup(temp1);
		pid_t pid=fork();
		if(pid==0){
			if(outname!=NULL){
				if(append==1)out=open(outname,O_WRONLY|O_APPEND);
				else out=open(outname, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
				dup2(out,1);	
			}
			if(piped==1){
				if(oo>0){
					dup2(pipes[2*(oo-1)],0);
					if((oo+1)<mo)dup2(pipes[(2*oo)+1],1);
					for(i=0;i<((mo-1)*2);i++){
						if(i==(2*(oo-1)))continue;
						close(pipes[i]);
					}
				}
				else{
					dup2(pipes[1],1);
					for(i=0;i<((mo-1)*2);i++){
						if(i==1)continue;
						close(pipes[i]);
					}
				}
			}
			echo(copy);
			_exit(0);
		}
		else{
			if(outname!=NULL)close(out);
			t=waitpid(pid,&status,WUNTRACED);
		}
	}
	else if(!strcmp(command,"quit")){
		exec_out.flag=1;
	}
	else if(!strcmp(command,"pwd")){
		char *str=strdup(temp1);
		pid_t pid=fork();
		if(pid==0){
			if(outname!=NULL){
				if(append==1)out=open(outname,O_WRONLY|O_APPEND);
				else out=open(outname, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
				dup2(out,1);	
			}
			if(piped==1){
				if(oo>0){
					dup2(pipes[2*(oo-1)],0);
					if((oo+1)<mo)dup2(pipes[(2*oo)+1],1);
					for(i=0;i<((mo-1)*2);i++){
						if(i==(2*(oo-1)))continue;
						close(pipes[i]);
					}
				}
				else{
					dup2(pipes[1],1);
					for(i=0;i<((mo-1)*2);i++){
						if(i==1)continue;
						close(pipes[i]);
					}
				}
			}
			printf("%s\n",curr.pwd);
			_exit(0);
		}
		else{
			if(outname!=NULL)close(out);
			t=waitpid(pid,&status,WUNTRACED);
		}
	}
	else if(!strcmp(command,"fg")){
		char *argv[100];
		argv[0]=command;
		argument=strtok(NULL," ");
		i=1;
		while(1){
			if(argument==NULL)break;
			argv[i++]=argument;
			argument=strtok(NULL," ");
		}
		argv[i]=NULL;
		int temp=0,tempp;char *nametempp;
		if(argv[1]!=NULL){
			for(j=0;j<strlen(argv[1]);j++)temp=temp*10+(argv[1][j]-'0');
			if(temp>=(*head))printf("No such process with that number exists\n");
			else{
				for(j=temp+1;j<(*head);j++){stack[j-1]=stack[j];stack_names[j-1]=NULL;stack_names[j-1]=strdup(stack_names[j]);}(*head)--;
				tempp=stack[temp];nametempp=strdup(stack_names[temp]);
				kill(tempp,SIGCONT);
				while((t=waitpid(tempp,&status,WNOHANG))==0){					
					g_wait=tempp;
					signal(SIGTSTP,signal_handler);
					if(g_flag==1){
						stack[*head]=tempp;
						stack_names[*head]=strdup(nametempp);
						names[tempp]=stack_names[*head];
						(*head)++;
						break;
					}
					if(g_flagg==1){
						puts("-1");
						kill(tempp,9);
						break;
					}
				}
			}
		}
		else printf("NULL Argument\n");
	}
	else if(!strcmp(command,"jobs")){
		for(i=1;i<(*head);i++)printf("[%d] %s [%d]\n",i,stack_names[i],stack[i]);
	}
	else if(!strcmp(command,"kjob")){
		char *argv[100];
		argv[0]=command;
		argument=strtok(NULL," ");
		i=1;
		while(1){
			if(argument==NULL)break;
			argv[i++]=argument;
			argument=strtok(NULL," ");
		}
		argv[i]=NULL;
		int temp=0,tempp,arg2=0;char *nametempp;
		for(j=0;j<strlen(argv[1]);j++)temp=temp*10+(argv[1][j]-'0');tempp=stack[temp];nametempp=strdup(stack_names[temp]);
		for(j=0;j<strlen(argv[2]);j++)arg2=arg2*10+(argv[2][j]-'0');
		if(arg2==9){kill(tempp,9);for(j=temp+1;j<(*head);j++){stack[j-1]=stack[j];stack_names[j-1]=NULL;stack_names[j-1]=strdup(stack_names[j]);}(*head)--;}
		else kill(tempp,arg2);
	}
	else if(!strcmp(command,"overkill")){
		char *argv[100];
		argv[0]=command;
		argument=strtok(NULL," ");
		i=1;
		while(1){
			if(argument==NULL)break;
			argv[i++]=argument;
			argument=strtok(NULL," ");
		}
		argv[i]=NULL;
		for(i=1;i<(*head);i++){kill(stack[i],9);stack[i]=-1;stack_names[i]=NULL;}
		*head=1;
	}
	else if(background=='&'){
		pid=fork();
		if(pid==0){
			char *argv[100];
			argv[0]=command;
			argument=strtok(NULL," ");
			i=1;
			while(1){
				if(argument==NULL)break;
				argv[i++]=argument;
				argument=strtok(NULL," ");
			}
			argv[i-1]=NULL;
			argv[i]=NULL;
			if(execvp(argv[0],argv)==-1)perror(command);
			_exit(0);
		}
		else{
			char str[MAX];
			int j=0;
			names[pid]=strdup(command);
			stack[*head]=pid;
			stack_names[*head]=strdup(command);
			printf("[%d] %d %s\n",*head,pid,stack_names[*head]); 
			(*head)++;
		}
	}
	else{
		pid_t pid=fork();
		if(pid==0){
			if(piped==1){
				if(oo>0){
					dup2(pipes[2*(oo-1)],0);
					if((oo+1)<mo)dup2(pipes[(2*oo)+1],1);
					for(i=0;i<((mo-1)*2);i++){
						if(i==(2*(oo-1)))continue;
						close(pipes[i]);
					}
				}
				else{
					dup2(pipes[1],1);
					for(i=0;i<((mo-1)*2);i++){
						if(i==1)continue;
						close(pipes[i]);
					}
				}
			}
			char *argv[100];
			argv[0]=command;
			argument=strtok(NULL," ");
			i=1;
			while(1){
				if(argument==NULL)break;
				argv[i++]=argument;
				argument=strtok(NULL," ");
			}
			if(inname!=NULL){
				in=open(inname,O_RDONLY);
				dup2(in,0);
			}
			if(outname!=NULL){
				if(append==1)out=open(outname,O_WRONLY|O_APPEND);
				else out=open(outname, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
				dup2(out,1);
			}
			if(inname!=NULL)close(in);
			if(outname!=NULL)close(out);
			argv[i]=NULL;
			if(execvp(argv[0],argv)==-1){flag=1;perror(command);}
			_exit(0);
		}
		else if(flag==0 && piped==0){
			g_pid=pid;
			while((t=waitpid(pid,&status,WNOHANG))==0){					
				g_wait=pid;
				signal(SIGTSTP,signal_handler);
				signal(SIGINT,signal_handler1);
				if(g_flag==1){
					stack[*head]=pid;
					stack_names[*head]=strdup(command);
					names[pid]=stack_names[*head];
					(*head)++;
					break;
				}
				if(g_flagg==1){
					kill(pid,9);
					break;
				}
			}
		}
	}
	return exec_out;
}
