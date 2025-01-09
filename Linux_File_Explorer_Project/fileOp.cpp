#include "header.cpp"
#include<spawn.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/wait.h>
extern char **environ;
void openFile(string filename){
	pid_t newPid;
	char* argv[]={"nano",new char[filename.size()+1],NULL};
	strcpy(argv[1],filename.c_str());// copy the fileName to 1st index argument of argv
	int status=posix_spawn(&newPid,"/bin/nano",NULL,NULL,argv,environ);
	if(status==0){
		if(waitpid(newPid,&status,0)!=-1);
			//cout<<"child exited with status "<<status<<endl;				
	}
	else{
		cout<<"unable to open file"<<strerror(status)<<"\n";
	}
}