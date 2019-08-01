#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

char **parse_command(char * command, char * file);

//Author: Eduard Klimenko
//Class: TCSS 422
//Assignment 1: MASH
int main(int argc, char *argv[]){
	// holds array of array of args
	char *** commandarr =  (char ***)malloc(sizeof(char **) * 3);

	//fetch cmd1
  	printf("Mash-1>");
	char command1[255];
	fgets(command1, 255, stdin);
	char * command1p = command1;

	//fetch cmd2
	printf("Mash-2>");
	char command2[255];
	fgets(command2, 255, stdin);
	char * command2p = command2;

	//fetch cmd3
	printf("Mash-3>");
	char command3[255];
	fgets(command3, 255, stdin);
	char * command3p = command3;

	//fetch filename
	printf("File>");
	char file[255];
	fgets(file, 255, stdin);
	char * filep = file;

	//setup for exec calls
	*(commandarr+0) = parse_command(command1p, filep);
	*(commandarr+1) = parse_command(command2p, filep);
	*(commandarr+2) = parse_command(command3p, filep);

	//var initialization
	int p1, p2, p3, p4, p5, p6, p7, c1 = 0, c2 = 0, c3 = 0;
	//struct used to keep time
	struct timespec start, finish, start1, finish1, start2, finish2, start3, finish3;
	
	//starts total elapsed time
	clock_gettime(CLOCK_REALTIME, &start);	

	//runs 3 commands at once using fork/exec/wait
	p1 = fork();
	//start cmd1 timer
	clock_gettime(CLOCK_REALTIME, &start1);
	if(p1 == 0){ //child
		close(STDOUT_FILENO);
		//create cmd1 output file
		open("cmd1output.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
		printf("----- CMD 1: ");
		while(*(*(commandarr+0)+c1) != NULL){
			printf("%s ", *(*(commandarr+0)+c1));
			c1++;
		}
		for(int j = 80; j > strlen(command1)+ strlen(file) + 14; j--)
			printf("-");
		printf("\n");
		//exec() call
		if(execvp(*(*(commandarr+0)+0), *(commandarr+0)) == -1){ //if fail
			printf("CMD1:[SHELL 1] STATUS CODE=-1\n");
			exit(1);
		}
	}else if(p1>0){
		p2 = fork();
		//start cmd2 timer
		clock_gettime(CLOCK_REALTIME, &start2);
		if(p2 == 0){ //child
			close(STDOUT_FILENO);
			//create cmd2 output file
			open("cmd2output.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
			printf("----- CMD 2: ");
			while(*(*(commandarr+1)+c2) != NULL){
				printf("%s ", *(*(commandarr+1)+c2));
				c2++;
			}
		for(int j = 80; j > strlen(command2)+ strlen(file) + 14; j--)
			printf("-");
		printf("\n");
		//exec() call
		if(execvp(*(*(commandarr+1)+0), *(commandarr+1)) == -1){ //if fail
			printf("CMD2:[SHELL 2] STATUS CODE=-1\n");
			//stop cmd2 timer (fail case)
			clock_gettime(CLOCK_REALTIME, &finish2);
			exit(1);
			}
		}else if(p2>0){
			p3 = fork();
			//start cmd3 timer
			clock_gettime(CLOCK_REALTIME, &start3);
			if(p3 == 0){ //child
			close(STDOUT_FILENO);
			//create cmd3 output file
			open("cmd3output.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
			printf("----- CMD 3: ");
			while(*(*(commandarr+2)+c3) != NULL){
				printf("%s ", *(*(commandarr+2)+c3));
				c3++;
			}
			for(int j = 80; j > strlen(command3)+ strlen(file) + 14; j--)
				printf("-");
			printf("\n");
			//exec() call
			if(execvp(*(*(commandarr+2)+0), *(commandarr+1)) == -1){ //if fail
				printf("CMD3:[SHELL 3] STATUS CODE=-1\n");
				//stop cmd3 timer (fail case)
				clock_gettime(CLOCK_REALTIME, &finish3);
				exit(1);
				}
			}else if(p3>0){
				//checks what cmd finished and stops its timer
				int res1 = wait(NULL);
				if (res1 == p1)
					clock_gettime(CLOCK_REALTIME, &finish1);
				else if (res1 == p2)
					clock_gettime(CLOCK_REALTIME, &finish2);
				else if (res1 == p3)
					clock_gettime(CLOCK_REALTIME, &finish3);
				printf("First process finished...\n");
			}
			//checks what cmd finished and stops its timer
			int res2 = wait(NULL);
			if (res2 == p1)
				clock_gettime(CLOCK_REALTIME, &finish1);
			else if (res2 == p2)
				clock_gettime(CLOCK_REALTIME, &finish2);
			else if (res2 == p3)
				clock_gettime(CLOCK_REALTIME, &finish3);
			printf("Second process finished...\n");		
		}
		//checks what cmd finished and stops its timer
		int res3 = wait(NULL);
		if (res3 == p1)
			clock_gettime(CLOCK_REALTIME, &finish1);
		else if (res3 == p2)
			clock_gettime(CLOCK_REALTIME, &finish2);
		else if (res3 == p3)
			clock_gettime(CLOCK_REALTIME, &finish3);
		printf("Third process finished...\n");
	}

	//stops total elapsed timer
	clock_gettime(CLOCK_REALTIME, &finish);
	long seconds = finish.tv_sec - start.tv_sec;
	long ns = finish.tv_nsec - start.tv_nsec;
	if (start.tv_nsec > finish.tv_nsec){
		--seconds;
		ns += 1000000000;
	}

	//prints cmd1 file contents
	p4 = fork();
	char *cmd1print[] = {"cat", "cmd1output.txt", 0};
	if(p4 == 0){ //child
		if(execvp(cmd1print[0], cmd1print) == -1){ //if fail
			printf("PRINTING CMD-1: STATUS CODE=-1\n");
			exit(1);
		}
	} wait(NULL);

	//prints cmd1 time
	long seconds1 = finish1.tv_sec - start1.tv_sec;
	long ns1 = finish1.tv_nsec - start1.tv_nsec;
	if (start1.tv_nsec > finish1.tv_nsec){
		--seconds1;
		ns1 += 1000000000;
	}
	printf("Result took: %dms\n",
		(int)(((double)seconds1 + (double)ns1/(double)1000000000)*(double)1000));


	//prints cmd2 file contents
	p5 = fork();
	char *cmd2print[] = {"cat","cmd2output.txt", 0};
	if(p5 == 0){ //child
		if(execvp(cmd2print[0], cmd2print) == -1){ //if fail
			printf("PRINTING CMD-2: STATUS CODE=-1\n");
			exit(1);
		}
	} wait(NULL);

	//prints cmd2 time
	long seconds2 = finish2.tv_sec - start2.tv_sec;
	long ns2 = finish2.tv_nsec - start2.tv_nsec;
	if (start2.tv_nsec > finish2.tv_nsec){
		--seconds2;
		ns2 += 1000000000;
	}
	printf("Result took: %dms\n",
		(int)(((double)seconds2 + (double)ns2/(double)1000000000)*(double)1000));

	//prints cmd3 file contents
	p6 = fork();
	char *cmd3print[] = {"cat", "cmd3output.txt", 0};
	if(p6 == 0){ //child
		if(execvp(cmd3print[0], cmd3print) == -1){ //if fail
			printf("PRINTING CMD-3: STATUS CODE=-1\n");
			exit(1);
		}
	} wait(NULL);

	//prints cmd3 time
	long seconds3 = finish3.tv_sec - start3.tv_sec;
	long ns3 = finish3.tv_nsec - start3.tv_nsec;
	if (start3.tv_nsec > finish3.tv_nsec){
		--seconds3;
		ns3 += 1000000000;
	}
	printf("Result took: %dms\n",
		(int)(((double)seconds3 + (double)ns3/(double)1000000000)*(double)1000));

	//prints process IDs and total time
	for(int j = 1; j < 80; j++)
	printf("-");
	printf("\n");
	printf("Children process IDs: %d %d %d\nTotal elsapsed time: %dms\n",
	 p1, p2, p3, (int)(((double)seconds + (double)ns/(double)1000000000)*(double)1000));

	//removing files
	p7 = fork();
	char *cmdremove[] = {"rm", "cmd1output.txt","cmd2output.txt","cmd3output.txt", 0};
	if(p7 == 0){ //child
		if(execvp(cmdremove[0], cmdremove) == -1){ //if fail
			printf("REMOVING CMDs: STATUS CODE=-1\n");
			exit(1);
		}
	} wait(NULL);
	
	//frees malloc
	free(commandarr);
	return 0;
}

//helper function to parse string and count the number of args
//supports unlimted number of args using malloc
char **parse_command(char * command, char * file){

	//initialization
	char *string, *found, *filep;
	string = strdup(command);
	filep = strdup(file);
	int argcount = 0, inword = 0, i = 0;
	char * commandp = command;

	//counts args
	do switch(*commandp) {
		case '\0':
		case ' ':
			if (inword) { inword = 0; argcount++; }
			break;
		default: inword = 1;
	} while (*commandp++);

	//allocated memory for arg array
	char ** commandptr = (char **)malloc(sizeof(char *) * (argcount+1));

	//parses words into array
	while ((found = strsep(&string, " ")) != NULL){
		*(commandptr+i) = found;
		i++;
	}

	//adds file to end of list
	*(commandptr+i) = filep;
	//removes '\n' from filename
	*(*(commandptr+(argcount-0)) + strlen(*(commandptr+(argcount-0)))-1) = '\0';
	//removes '\n' from last arg
	*(*(commandptr+(argcount-1)) + strlen(*(commandptr+(argcount-1)))-1) = '\0';
	//makes last arg null for exec call
	*(commandptr+argcount+1) = NULL;

	return commandptr;
}
