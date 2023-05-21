#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct  {
    char prog_name[10];
    char sym_name[10];
    int address;
    int length;
}PROG_DATA;

PROG_DATA PROG_INIT(PROG_DATA data){
    data.prog_name[0] = '\0';
    data.sym_name[0] = '\0';
    data.address = 0;
    data.length = 0;
    return data;
}

int main(int argc, char *argv[])
{
    if(argc <= 2){
        printf("%s ",argv[0]);
        printf("format error!\n");
        printf("Syntax: load <address> <file 1> <file 2> …\n");
    }else if(strcmp(argv[1],"load") == 0){
        FILE * fpr;

        fpr = fopen ("load_map.txt", "w");

        printf("Mode:%s\n",argv[1]);
        int startaddr = strtol(argv[2],NULL, 16);
        printf("startaddr : %x\n",startaddr);
        printf("Control     Symbol\n");
        printf("section     name        Address    Length\n");
        printf("-----------------------------------------\n");


        fprintf(fpr,"Control     Symbol\n");
        fprintf(fpr,"section     name        Address    Length\n");
        fprintf(fpr,"-----------------------------------------\n");
        for(int i = 3; i < argc; i++){
            if(argv[i] == NULL){
                printf("Not find file!");
                break;
            }else{
                FILE * fp;
                char buffer[100] = "";
                buffer[0] = '\0';
                PROG_DATA data;
                PROG_DATA start;
                PROG_DATA end;
                data = PROG_INIT(data);
                start = PROG_INIT(start);
                end = PROG_INIT(end);
                fp = fopen (argv[i], "r");   

                while(!feof(fp)){
                    fgets (buffer, sizeof(buffer), fp);
                    if(buffer[0] == 'H'){
                        int i = 1;
                        while(buffer[i] != ' '){
                            data.prog_name[i - 1] = buffer[i];
                            i++;
                        }
                        i += 7;
                        data.prog_name[i] = '\0';
                        // printf("%s\n",data.prog_name);

                        char temp[20];
                        int count = 0;
                        while (count != 6)
                        {   
                            temp[count] = buffer[i + count];
                            // printf("%c",buffer[i + count]);
                            count++;
                        }
                        data.length = strtol(temp,NULL,16);
                        data.address = startaddr;
                        startaddr += data.length;
                        // printf("%x\n",data.length);

                    }else if(buffer[0] =='D'){
                        int i = 1;
                        while(buffer[i] != ' '){
                            start.sym_name[i - 1] = buffer[i];
                            i++;
                        }
                        i++;

                        char temp[10];
                        int count = 0;
                        while(count != 6){
                            temp[count] = buffer[i + count];
                            count++;
                        }
                        start.address = strtol(temp,NULL,16) + data.address;
                        // printf("%X\n",start.address);

                        i = count + i;
                        count = 0;
                        while(buffer[i + count] != ' '){
                            end.sym_name[count] = buffer[i + count];
                            count++;
                        }
                        // printf("%s\n",end.sym_name);

                        i = count + i;
                        count = 0;
                        i += 2;
                        while (count != 6)
                        {
                            temp[count] = buffer[i + count];
                            count++;
                        }
                        end.address = strtol(temp,NULL,16) + data.address;
                        // printf("%X\n",end.address);
                    }
                }
                printf("%-24s",data.prog_name);
                printf("%-12X",data.address);
                printf("%04X\n",data.length);
                printf("            %-12s",start.sym_name);
                printf("%04X\n",start.address);
                printf("            %-12s",end.sym_name);
                printf("%04X\n",end.address);
                printf("\n");

                fprintf(fpr,"%-24s",data.prog_name);
                fprintf(fpr,"%-12X",data.address);
                fprintf(fpr,"%04X\n",data.length);
                fprintf(fpr,"            %-12s",start.sym_name);
                fprintf(fpr,"%04X\n",start.address);
                fprintf(fpr,"            %-12s",end.sym_name);
                fprintf(fpr,"%04X\n",end.address);
                fprintf(fpr,"\n");
                // printf("%")
            }
        }
        printf("-----------------------------------------\n");
        fprintf(fpr,"-----------------------------------------\n");

        printf("load map writed in load_map.txt!\n");
    }else{
        printf("format error!\n");
        printf("Syntax: load <address> <file 1> <file 2> …\n");
    }
}