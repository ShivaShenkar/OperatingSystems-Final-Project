#include "randomGraph.cpp"
#include <stdlib.h>
#include <string>

int main(){

    #define ITERATIONS 30
    #define MAX_VERTICES 30

    int randV,randE;
    for(int i=0;i<ITERATIONS;i++){
        randV = (random()%MAX_VERTICES)+1;
        randE = random()%(randV*(randV-1)/2+1);
        Graph g = generateRandomGraph(randV,randE);
        findEulerianCircle(g);
    }


    return 0;
}