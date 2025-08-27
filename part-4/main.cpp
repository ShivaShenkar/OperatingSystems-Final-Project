#include "randomGraph.cpp"
#include <stdlib.h>
#include <string>

/**
 * make sure ./main in part-3 is compiled!! * 
**/
int main(){

    #define ITERATIONS 30
    #define MAX_VERTICES 30

    int randV,randE;
    for(int i=0;i<ITERATIONS;i++){
        randV = (random()%MAX_VERTICES)+1;
        randE = random()%(randV*(randV-1)/2+1);
        generateRandomGraph(randV,randE);
    }


    return 0;
}