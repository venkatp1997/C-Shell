#include <stdio.h>
#include <string.h>
struct dir{
	char pwd[100];
	char path[100];
};
struct dir cd(char *arg,struct dir curr,int check_len,char *check){
	struct dir copy=curr;
	int flag=0;
	char *argument=strtok(arg,"/");
	int i,j;
	while(1){
		if(argument==NULL)break;
		if(!strcmp(argument,"~")){
			strcpy(curr.pwd,check);
			strcpy(curr.path,curr.pwd);
		}
		else strcat(curr.pwd,argument);
		if(chdir(curr.pwd)==-1)perror("error");
		getcwd(curr.pwd,100);
		strcat(curr.pwd,"/");
		argument=strtok(NULL,"/");
	}
	strcpy(curr.path,curr.pwd);
	if (strlen(curr.path)<check_len)flag=1;
	if (strlen(curr.path)>=check_len){
		for(i=0;i<check_len;i++){if(check[i]!=curr.path[i]){flag=1;break;}}
	}
	if(!flag){
		j=1;
		char *temp1=strdup(curr.path);
		memset(curr.path,'\0',sizeof(curr.path));
		strcat(curr.path,"~");
		for(i=check_len;i<strlen(temp1);i++)curr.path[j++]=temp1[i];	
		curr.path[strlen(curr.path)-1]='\0';
	}
	return curr;
}
