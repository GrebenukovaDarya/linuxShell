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
void mem_dump(DIR* dir, char* path) {
    FILE* res = fopen("mem.txt", "w+");
    fclose(res);
    struct dirent* ent;
    char* file_path;
    
    while ((ent = readdir(dir)) != NULL) {
        asprintf(&file_path, "%s/%s", path, ent->d_name);
        FILE *f1 = fopen("mem.txt", "a");
        FILE *f2 = fopen(file_path, "r");
        if (!f1 || !f2) {
        printf("Error while reading file %s\n", file_path);
        return;
    }
    char buf[256];

    while (fgets(buf, 256, f2) != NULL) {
        fputs(buf, f1);
    }
    fclose(f1);
    fclose(f2);
    }
    printf("succes\n");
}

 
int main() {
    FILE *history_file = fopen("history.txt", "a");
    fclose(history_file);
    char input[BUFFER_SIZE];
    
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
        if(history_file!= NULL) {
          FILE *f = fopen("history.txt", "a");
          fprintf(f, "%s\n", input);
          fclose(f);
      }

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
        if (strncmp(input, "\\mem ", 5) == 0) {
            char* path;
            asprintf(&path, "/proc/%s/map_files", input+5);

            DIR* dir = opendir(path);
            if (dir) {
                mem_dump(dir, path);
            }
            else {
                printf("Process not found\n");
            }
            check = true;
            continue;
        }

        if(check==false){
        printf("there is no '%s' command\n", input);
        }
       
    }
    while (!feof(stdin));
 
    return 0;
}
