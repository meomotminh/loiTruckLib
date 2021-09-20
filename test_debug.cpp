#include <wiring.h>
#include <hardwareserial.h>
#include "memdebug.h"

void shuffle(void **array, int n);
void showmem();
void dotest(bool shuffle);

extern "C" void __cxa_pure_virtual() { for(;;);}

int main(void){
    init();
    Serial.begin(9600);

    dotest(false);
    dotest(true);

    exit(0);
}

void dotest(bool shuffle__){
    void *ptrs[20];
    uint16_t i;

    Serial.println("Allocating");
    showmem();
    for (i = 0; i<sizeof(ptrs)/sizeof(ptrs[0]);i++){
        ptrs[i] = malloc(20);
        showmem();
    }

    if (shuffle__)
        shuffle(ptrs, size(ptrs)/sizeof(ptrs[0]));

    Serial.println("freeing");
    for (i = 0; i<sizeof(ptrs)/sizeof(ptrs[0]);i++){
        free(ptrs[i]);
        showmem();
    }
}

void showmem()
{
    char buffer[100];
    sprintf(buffer, "%04u %04u %04u : used/free/large",
            getMemoryUsed(),
            getFreeMemory(),
            getLargestAvailableMemoryBlock()
            );

    Serial.println(buffer);
}

int rand_int (int n)
{
    int limit = RAND_MAX - RAND_MAX % n;
    int rnd;

    do 
    {
        rnd = rand();    
    } while (rnd >= limit);
    return rnd % n;
}

void shuffle(void **array, int n)
{
    int i,j;
    void *tmp;

    for (i = n - 1; i > 0; i--){
        j = rand_int(i+1);
        tmp = array[j];
        array[j] = array[i];
        array[i] = tmp;
    }
}