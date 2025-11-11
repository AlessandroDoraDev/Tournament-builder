#include <iostream>
#include "TournamentBuilderGui.h"


int main(){
#ifdef MIO_DEBUG
    std::cout<<"Siamo in debug mode\n";
#elif defined(MIO_RELEASE)
    std::cout<<"Siamo in release mode\n";
#else
    std::cout<<"La definizione non ha funzionato\n";
#endif
    mainGui();
}