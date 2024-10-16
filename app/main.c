#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define BUFFER_SIZE 1024
#define HISTORY_SIZE 100
#define HISTORY_FILE "command_history.txt"
 
//history
void save_history_to_file(char history[][BUFFER_SIZE], int count) {
    FILE *file = fopen(HISTORY_FILE, "a");
    if (file == NULL) {
        perror("Не удалось открыть файл для записи истории");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s\n", history[i]);
    }
    fclose(file);
}
 
int main() {
    char input[BUFFER_SIZE];
    char history[HISTORY_SIZE][BUFFER_SIZE];
    int history_count = 0;
 
    do {
        printf("USER$ ");
        fflush(stdout);

        // input
        if (fgets(input, BUFFER_SIZE, stdin) == NULL) {
            printf("\nЗавершение работы (Ctrl+D)\n");
            break; // выход из цикла при EOF
        }
 
        // \n
        input[strcspn(input, "\n")] = '\0';
 
        // exit
        if (strcmp(input, "exit") == 0 || strcmp(input, "\\q") == 0) {
            printf("Завершение работы (exit/\\q)\n");
            break;
        }

        // history
        if(history_count < HISTORY_SIZE) {
            strcpy(history[history_count], input);
            history_count++;
        }

       if(history_count >= HISTORY_SIZE) {
            history_count = 0;
            //clear hostory_file
       }

        //echo $PATH
        if(strcmp(input, "echo $PATH")==0) {
                char *path = getenv("PATH");
                if (path!=NULL){
                        printf("%s\n", path);
                }
                else {
                        printf("error \n");
                }
                continue;
        }
 
        // echo
        if (strncmp(input, "echo ", 5) == 0) {
            printf("%s\n", input + 5);
            continue;
        }
     
        printf("Вы ввели: %s\n", input);
       
    }
    while (!feof(stdin));
 
    //history
    save_history_to_file(history, history_count);
 
    return 0;
}
