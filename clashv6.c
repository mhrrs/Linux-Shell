#define _GNU_SOURCE
#define MAX_ARGS 32
#include "clashv6.h"

int main(int argc, char **argv){
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	char *my_args[MAX_ARGS];
	
	//AUX variables that allow me to keep everything modular by looping through functions
	char *command_array[20] = {"path", "cd", "exit"};
	int check = 0;

	if(argc < 2){
		printf("No batch detected.\n");
	}

	else if(argc > 2){
		fprintf(stderr, "Usage error.\n");
		exit(EXIT_FAILURE);
	}

	else{
		FILE *stream;
		stream = fopen(argv[1], "r");
		if(stream == NULL){
			perror("fopen");
			exit(EXIT_FAILURE);
		}

		while((nread = getline(&line, &len, stream)) != -1){
			printf("Retrieved line of length %zu:\n", nread);
			fwrite(line, nread, 1, stdout);
		}
		free(line);
		fclose(stream);
	}


	//beginning of the program's interactions with the user
	printf("Entering interactive mode.\n");
	printf("clash>");	
	while ((nread = getline(&line, &len, stdin)) != -1){
	
		//parse() tokenizes every argument into my_args and then returns the amount of arguments to argc			
		argc = parse(line, my_args);
		
		//loops through the list of built-in functions to determine whether one needs to be executed
		//3 is used as it is the size of command array
		for(int i = 0; i < 3; i++){
			if(strcmp(my_args[0],command_array[i]) == 0){
				interactive(argc, my_args);
				check = 1;	
			}
		}
		
		//in the case the arg is not a built-in function, this code will run
		if(check == 0){
			execute(argc, my_args);
		}
		
		check = 0;	
		printf("clash>");
	}

	free(line);
	exit(EXIT_SUCCESS);
}
