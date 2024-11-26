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

//12
bool append(char* path1, char* path2) {
    FILE *f1 = fopen(path1, "a");
    FILE *f2 = fopen(path2, "r");
    if (!f1 || !f2) {
        printf("Error while reading file %s\n", path2);
        return false;
    }
    char buf[256];

    while (fgets(buf, 256, f2) != NULL) {
        fputs(buf, f1);
    }
    fclose(f1);
    fclose(f2);
    return true;
}

void makeDump(DIR* dir, char* path) {
    FILE* res = fopen("res.txt", "w+");
    fclose(res);
    struct dirent* ent;
    char* file_path;
    while ((ent = readdir(dir)) != NULL) {
        asprintf(&file_path, "%s/%s", path, ent->d_name); // asprintf работает
        if(!append("res.txt", file_path)) {
            return;
        }
    }
    printf("succes\n");
}

 
int main() {
    char input[BUFFER_SIZE];
    char history[HISTORY_SIZE][BUFFER_SIZE];
    int history_count = 0;
// По сигналу SIGHUP вывести "Configuration reloaded"
        signal (SIGHUP, handle_SIGHUP);
 
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
             
            }
sleep(1);
continue;
}


  
        
       //10. По `\l /dev/sda` определить является ли диск загрузочным
         if (strncmp(input, "\\l", 2) == 0) {
            check=true;
            char* dname = input + 3;
            disk_check(dname);
            continue;
        }
        
       //12
        if (strncmp(input, "\\proc ", 6) == 0) {
            char* path;
            asprintf(&path, "/proc/%s/map_files", input+6);

            DIR* dir = opendir(path);
            if (dir) {
                makeDump(dir, path);
            }
            else {
                printf("Process not found\n");
            }
            check = true;
            continue;
        }

        if(check==false){
        printf("there is no command: %s\n", input);
        }
       
    }
    while (!feof(stdin));
 
    //history

    save_history(history, history_count);
 
    return 0;
}
