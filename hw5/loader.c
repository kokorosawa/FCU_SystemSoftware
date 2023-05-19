#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("argc is %d\n",argc);
    if(argc <= 3){
        printf("%s",argv[0]);
    }else{
        
        printf("Mode:%s\n",argv[1]);
        int startaddr = strtol(argv[2],NULL, 16);
        printf("%d\n",startaddr);
        for(int i = 3; i < argc; i++){
            if(argv[i] == NULL){
                printf("Not find file!");
            }else{
                FILE * fp;
                char buffer[100] = "";
                buffer[0] = '\0';

                fp = fopen (argv[i], "r");   
                while(!feof(fp)){
                    fgets (buffer, sizeof(buffer), fp);
                    printf("%s",buffer);
                }
                

            }
        }
    }
}