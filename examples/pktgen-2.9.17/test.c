#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX 256


void nameformat(char *name, char *QS)
{
        char *bungle, *x;
        char elem[255];

        *QS = 0;
        bungle = malloc(strlen(name) + 3);
        strcpy(bungle, name);
        x = strtok(bungle, ".");
        while (x != NULL) {
                if (snprintf(elem, 255, "%c%s", strlen(x), x) == 255) {
                        puts("String overflow.");
                        exit(1);
                }
                strcat(QS, elem);
                x = strtok(NULL, ".");
        }
        free(bungle);
}


main()
{
int i;
char *name = "www.example.com";
//char dst[128];
char *dst = malloc(strlen(name)+3);
nameformat(name, dst);
for(i = 0 ; i< strlen(dst); i++){
printf("dst %d\n", dst[i]);
}
printf("name length%d\n", strlen(name));
printf("dst lenght  %d\n", strlen(dst));

}
