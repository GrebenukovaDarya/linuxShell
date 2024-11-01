#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
 
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

void handle_SIGHUP(int signal) {
    if (signal == SIGHUP) {
        printf("Configuration reloaded\n");
    }
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
       }

        //echo $PATH
        if(strcmp(input, "e $PATH")==0) {
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

       //binary
/*
       if (strncmp(input, "cat ", 4) == 0) {
           binary_ex(input + 4);
           continue;
       }
*/

       // 9. По сигналу SIGHUP вывести "Configuration reloaded"
        signal (SIGHUP, handle_SIGHUP);
        
       //10. По `\l /dev/sda` получить информацию о разделах в системе
/*
        if (strncmp(input, "\l /dev/sda", 11) == 0) {
            DWORD drives= GetLogicalDrives();
            if (drives == 0) { 
                printf("error \n");
            }
            else {
                while(drives) {
                    if (drives & 1)
                        printf ("%s", (const char *)szDrive);
                    ++szDrive[1];
                    drives >>=1;
                }
            }
            continue;
        }
*/
        
       //11. По `\cron` подключить VFS в /tmp/vfs со списком задач в планировщике
        
       //12. По `\mem <procid>` получить дамп памяти процесса
     
        printf("there is no command: %s\n", input);
       
    }
    while (!feof(stdin));
 
    //history
    save_history_to_file(history, history_count);
 
    return 0;
}

/*
//binary file
void binary_ex(const char *binary_name){
   FILE *bfile = fopen(binary_name, "rb");
   if (bfile == NULL){
    perror ("error");
    return;
   }
 fseek(bfile, 0, SEEK_END);
 long bfile_size = ftell(bfile);
 rewind(bfile);
 unsigned char *buffer = (unsinged char *)malloc(bfile_size);
 if (buffer == NULL) {
  perror("error");
  fclose(bfile);
  return;
 }
size_t count = fread(buffer, 1, bfile_size, bfile);
 if (count != bfile_size) {
  perror("error");
  free(buffer);
  fclose(bfile);
  return;
 }
  printf("содержимое бинарного файла %s:\n",binary_name);
  for (long i=0; i<bfile_size ;i++){
    printf("%02X ", buffer[i]);
  }
  printf("\n");
  free(buffer);
  fclose(bfile);
}
*/
