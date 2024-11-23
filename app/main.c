#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/mount.h>
#include <stdint-gcc.h>
#include <sys/stat.h>
 
#define BUFFER_SIZE 1024
#define HISTORY_SIZE 100
#define HISTORY_FILE "command_history.txt"
 
//history
void save_history(char history[][BUFFER_SIZE], int count) {
    FILE *file = fopen(HISTORY_FILE, "a");
    if (file == NULL) {
        perror("history file not found");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s\n", history[i]);
    }
    fclose(file);
}

//sighup
void handle_SIGHUP(int signal) {
    if (signal == SIGHUP) {
        printf("Configuration reloaded\n");
        exit(0);
    }
}

//10
void disk_check(char* dname) {
 
    while (*dname == ' ') {
        dname++;
    }
    char full_path[128];
    snprintf(full_path, sizeof(full_path), "/dev/%s", dname);

    FILE* disk = fopen(full_path, "rb");
    if (disk == NULL) {
        printf("error\n");
        return;
    }
 
    if (fseek(disk, 510, SEEK_SET) != 0) {
        printf("error\n");
        fclose(disk);
        return;
    }
    uint8_t signature[2];
    if (fread(signature, 1, 2, disk) != 2) {
        printf("error\n");
        fclose(disk);
        return;
    }
    fclose(disk);
    if (signature[0] == 0x55 && signature[1] == 0xAA) {
        printf("disk %s is bootable.\n", dname);
    } else {
        printf("disk %s is not bootable.\n", dname);
    }
}

 
int main() {
    char input[BUFFER_SIZE];
    char history[HISTORY_SIZE][BUFFER_SIZE];
    int history_count = 0;
 
    do {
        printf("USER$ ");
        fflush(stdout);
        bool check=false;

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

     /*
       if(history_count >= HISTORY_SIZE) {
            history_count = 0;
       }
     */

        //echo $PATH
        if(strcmp(input, "e $PATH")==0) {
            check=true;
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
            check=true;
            printf("%s\n", input + 5);
            continue;
        }

       //binary

        if (strncmp(input, "run ", 4) == 0){
            check=true;
            pid_t p = fork();
            if (p == 0){
              char *argv[] = { "sh", "-c", input + 4, 0 };
              execvp(argv[0], argv);
              fprintf(stderr, "Failed to exec shell on %s", input + 4);
              
              exit(1);
             
              continue;
            }
}


       // По сигналу SIGHUP вывести "Configuration reloaded"
        signal (SIGHUP, handle_SIGHUP);
        
       // По `\l /dev/sda` определить является ли диск загрузочным
         if (strncmp(input, "\\l", 2) == 0) {
            check=true;
            char* dname = input + 3;
            disk_check(dname);
            continue;
        }
        
       //11. По `\cron` подключить VFS в /tmp/vfs со списком задач в планировщике
        if(check==false){
        printf("there is no command: %s\n", input);
        }
       
    }
    while (!feof(stdin));
 
    //history

    save_history(history, history_count);
 
    return 0;
}
