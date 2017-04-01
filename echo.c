#include <stdio.h>
#include <string.h>
void echo(char *str){
	int quotes=0;
	int text=0;
	char ans[100][100];
	int k=0,j=0,i;
	for(i=5;i<strlen(str);i++){
		if(str[i]==' '){
			if(quotes==1)ans[j][k++]=str[i];
			else if(text==1){ans[j][k]='\0';j++;k=0;text=0;}
			else text=0;
		}
		else if(str[i]=='\"'){
			if(quotes==1){quotes=0;ans[j++][k]='\0';k=0;}
			else quotes=1;
		}
		else{text=1;ans[j][k++]=str[i];}
	}
	for(i=0;i<=j;i++)printf("%s ",ans[i]);puts("");
}
