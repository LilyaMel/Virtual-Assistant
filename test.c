#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    char *pass;
    pass = getpass("Enter password : ");
    printf("this is my password : %s\n", pass);
}