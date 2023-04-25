#include <stdio.h>
#include <string.h>

void itoh(int num){
    char hex[3];
    char set[] = "0123456789ABCDEF";
    hex[0] = set[num/16];
    num = num % 16;
    hex[1] = set[num];
    hex[2] = '\0';
    printf("%s\n",hex);
}

int main(){
    itoh(20);
    itoh(60);
    itoh(76);
    itoh(224);
    itoh(216);
    itoh(44);
    itoh(84);

}