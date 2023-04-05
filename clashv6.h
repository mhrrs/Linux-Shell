#ifndef CLASHV6_H
#define CLASHV6_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#define MAX_ARGS 32

char *string_builder(int argc, char *argv[]){

	//if a path command has no arguments the path is made null and exits	
	if(argc < 2){
		fprintf(stderr, "Usage error: %s \n", argv[0]);
		setenv("PATH", "\0", 1);
		printf("Updated PATH: (null) %s\n", getenv("PATH"));
		exit(-1);
	}

	//this chunk is what builds the string that is the path var
	int new_string_size = 0;
	int counter = 1;
	while(argv[counter] != NULL){
		new_string_size += strlen(argv[counter]) + 1;
		counter++;
	}

	char*new_string = malloc(sizeof(char)*new_string_size);
	new_string[0] = '\0';
	counter = 1;
	while(argv[counter] != NULL){
		strncat(new_string, argv[counter], strlen(argv[counter]) + 1);
		counter++;

		if(argv[counter] != NULL)
			strncat(new_string, ":", 2);
	}

	printf("You built this string: %s \n", new_string);

	//this part returns the string for it to be used by other functions
	return new_string;
}


int parse(char *line, char *my_args[]){
	char *token;
	int my_argc = 0;

	bzero(my_args, MAX_ARGS);

	//tokenizes user input into a char array
	token = strtok(line, " \n");
	while(token != NULL){
		my_args[my_argc] = token;
		token = strtok(NULL, " \n");
		my_argc++;
	}

	//returns the amount of arguments in the char array
	return my_argc;
}


void execute(int argc, char *argv[]){
	//return value from fork
	pid_t pid = fork();
	pid_t wait_result;
	int status;
	int placeh = 0;
	char *new_args[32];

	
	//this part of the program determines whether a command is to be outputted to a file
	for(int i = 0; i < argc; i++){
		if(strcmp(argv[i], ">") == 0 && pid == 0){
			placeh = i;
			char *str = argv[i+1];
			
			//recreate the argv so that it does not use '>' as an argument
			for (int i = 0; i < placeh; i++){
				new_args[i] = argv[i];
			}
			new_args[placeh] = NULL;

			
			//create file, open
			int file = open(str, O_CREAT | O_TRUNC | O_WRONLY, 0600);
			dup2(file, STDOUT_FILENO);
			close(file);
			
			perror("execvp");
			execvp(new_args[0], new_args);
			
			sleep(1);
			exit(0);	
		}
	
	}

	//determines whether a process is a parent or child
	if(pid == 0){
		execvp(argv[0], argv);
		sleep(1);
		exit(0);	
	}

	else if(pid < 0){
		perror("*** ERROR: Forking child process failed.\n");
		exit(-1);
	}
	
	else{
		wait_result = waitpid(pid, &status, WUNTRACED);
	}
}


void interactive(int argc, char *my_args[]){
	
	//this function is used whenever a built-in commmand is called.

	if(strcmp(my_args[0], "exit") == 0){
		exit(-1);
	}	
	
	else if(strcmp(my_args[0], "path") == 0){
		char *str = string_builder(argc, my_args);
		setenv("PATH", str, 1);
		printf("Updated PATH: %s\n", getenv("PATH"));		
	}

	else if(strcmp(my_args[0], "cd") == 0){
		char cd_buf[100];
		printf("Current working directory: %s\n", getcwd(cd_buf, 100));
		if(chdir(my_args[1]) != 0){
			perror("chdir() error");
		}
		else{
			chdir(my_args[1]);
			printf("Current working directory: %s\n", getcwd(cd_buf, 100));
		}
	}
}

#endif
