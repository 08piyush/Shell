/*piyush pancholi , 12041050*/


#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#define INPUT 0
#define OUTPUT 1

									/* these functions are predefined here to avoid implicit uneclaration error */
char **split_command(char *line, const char *c );
void tokenize_command(char** param,int *nr,char *buf,const char *c);
void Redirect_command(char** buf,int nr);


///////////////////////////////////////////////////////////////////////
									/* this function to avoid white space characters from the input command */
void removeWhiteSpace(char* buf){
	if(buf[strlen(buf)-1]==' ' || buf[strlen(buf)-1]=='\n')
	buf[strlen(buf)-1]='\0';
	if(buf[0]==' ' || buf[0]=='\n') memmove(buf, buf+1, strlen(buf));
}


//////////////////////////////////////////////////////////////////////	
									/* use of fork() and exec() to create child process from the parent and executing them */
int process_execute(char **args)
{
    // Forking a child
    pid_t pid = fork();  
    if (pid == -1) {
        printf("\nFailed forking child..");
        return -1;
    } else if (pid == 0) {
        if (execvp(args[0], args) < 0) {
            printf("\nCould not execute command.. \n");
            return -1;
        }
        exit(0);
    } else {
        								// waiting for child to terminate
        wait(NULL); 
        return 1;
    }
}

///////////////////////////////////////////////////////////////////////
									/* function to perform redirection '>' in the commands , only OUTPUT redirection is implemented as asked in assignment */
void Redirect_command(char** buf,int nr){
	int pc,fd;
	char *argv[100];
	removeWhiteSpace(buf[1]);
	tokenize_command(argv,&pc,buf[0]," ");
	if(fork()==0){
{		fd=open(buf[1],O_WRONLY); 	}
		if(fd<0){
			perror("cannot open file\n");
			return; }
{		dup2(fd,1);		}
		execvp(argv[0],argv);
		perror("invalid input ");
		exit(1); 						//exit when exec is not successful 
	}
	wait(NULL);
}

///////////////////////////////////////////////////////////////////////
									/* function to perform piping '|' in the input commands , it supports upto maximum 10 commands */
									/* user can increase the piping limit in the function descripter array [10][2]*/
void do_pipe(char** line,int nr){
	if(nr>10) return;

	int fd[10][2],i,pc;
	char *argv[100];

	for(i=0;i<nr;i++){
	
		tokenize_command(argv,&pc,line[i]," ");
		if(i!=nr-1){
			if(pipe(fd[i])<0){
				perror("pipe creating was not successfull\n");
				return;
			}
		}
		if(fork()==0){//child1
			if(i!=nr-1){
				dup2(fd[i][1],1);
				close(fd[i][0]);
				close(fd[i][1]);
			}

			if(i!=0){
				dup2(fd[i-1][0],0);
				close(fd[i-1][1]);
				close(fd[i-1][0]);
			}
			execvp(argv[0],argv);
			perror("invalid input ");
			exit(1);					//exit when exec is not successful 
		}
		//parent
		if(i!=0){//second process
			close(fd[i-1][0]);
			close(fd[i-1][1]);
		}
		wait(NULL);
	}}
	
	
///////////////////////////////////////////////////////////////////////	
									/*this is main executing body of function which executes simple multiargument commands */
int execute_command(char **args)
{
	int NoOfOwnCmds = 3, i, switchOwnArg = 0;
	char* ListOfOwnCmds[NoOfOwnCmds];
	char* username;
	
	ListOfOwnCmds[0] = "exit";					// these four commands are predefined we use these commands directly without fork and exec 
	ListOfOwnCmds[1] = "cd";					//
	ListOfOwnCmds[2] = "help";					//
	//ListOfOwnCmds[3] = "hello";					

	if (args[0] == NULL) {
									// An empty command was entered.
	return 1;
	}

	for (i = 0; i < NoOfOwnCmds; i++) {
	if (strcmp(args[0], ListOfOwnCmds[i]) == 0) {
	switchOwnArg = i + 1;
			break;
	}
	}
	if(switchOwnArg==1){
		printf("\nyou are exited from the shell \n");
		exit(0);
	}
	else if(switchOwnArg==2){
		chdir(args[1]);					// as given in assignment chdir command is directly used here for cd (changing directory )
		return 1;
	}
	else if(switchOwnArg==3){
		puts("this is help section \n commands available : \n cd\n ls\n exit");
		return 1;
	}
	return process_execute(args);
	}
	
//////////////////////////////////////////////////////////////////////	
									/*the input command is parsed in this section using getchar() function */
char *take_command(void)
{
  int bufsize = 100000;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  while (1) {
    									// Read a character using getchar() 
    c = getchar();

    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;   
}}


//////////////////////////////////////////////////////////////////////
									/* here we are parsing commands into tokens each argument of command is taken in token array */
void tokenize_command(char** param,int *nr,char *buf,const char *c){
	char *token;
	token=strtok(buf,c);
	int pc=-1;
	while(token){
		param[++pc]=malloc(sizeof(token)+1);
		strcpy(param[pc],token);
		removeWhiteSpace(param[pc]);
		token=strtok(NULL,c);
	}
	param[++pc]=NULL;
	*nr=pc;
}

//////////////////////////////////////////////////////////////////////
									/* here we are using character c as delimiter for segmenting string into various small snippets of commands */
char **command_split(char *line, const char *c )
{
  int bufsize = 64, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;
  
									//strtok for splitting a string by some delimiter " " , such as space , pipe 
	token = strtok(line,c);
	while (token != NULL) {
		tokens[position] = token;
		position++;

	if (position >= bufsize) {
		bufsize += 64;
		tokens_backup = tokens;
		tokens = realloc(tokens, bufsize * sizeof(char*));
	if (!tokens) {
		free(tokens_backup);
		fprintf(stderr, " allocation error\n");
		exit(EXIT_FAILURE);
	}}
	token = strtok(NULL,c);
	}
	tokens[position] = NULL;
	return tokens;
	}


////////////////////////////////////////////////////////////////////////////
//main program 
void active_loop(void)							/*this is an infinite loop which runs throughout the program and waits for the user to enter command everytime */
{
  char *buffer[100];
  char *line;
  char **args;
  char **newarr;
  int status;
  int nr=0;

do {
    printf("piyush @ 12041050 ~~rat``> ");  					/* here the shell is created */
    line = take_command();						/*to read the input command */
    
    if(strchr(line,'|')){						/*it searches presence of pipe in input */
    	tokenize_command(buffer,&nr,line,"|");				/*delimiter '|' to separate into individual commands  */
	do_pipe(buffer,nr);
	}
	
//program to run redirection in shell	
	else if(strchr(line,'>')){					//redirect output to file , searches the presence of > this symbol 
		tokenize_command(buffer,&nr,line,">");
		if(nr==2)
		Redirect_command(buffer,nr);				// program to perform output redirection 
		else printf("wrong format for redirection , use cmd1 > <filename> format");
	}
	
	
//program to run && in shell 
	else if(strpbrk(line,"&&")){					/*searches multichar && in the input */
		int i =0;						/*delimiter '&&' to separate into individual commands  */
		args=command_split(line,"&&");
	
		newarr=command_split(args[0]," ");
		status=execute_command(newarr);
	 while (status != -1) {                                      //it checks if the output of previous commands is -1 ,if its -1 then it stops otherwise it process next command 			   
		if (i<=sizeof(args)-1){     				// checks the size of array having commands separated by && 
			newarr=command_split(args[i+1]," ");		// splits that command into individual commands 
			status=execute_command(newarr);		// executes each command using the same command execute function 
			i=i+1;
			free(newarr);		
			}
		else{break;}
	}	}
//program to perform rest of direct commands 
    else {
    args=command_split(line," ");					// delimits by space 
    status = execute_command(args);					// using command execute function to execute command
    free(line);
    free(args); }
  } while (status);
}


//////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) 					// this is main function which performs first , it has a loop in it which keeps the shell active 
{
  active_loop();
  return 0;
}

