#include "../include/main_application.hpp"


int main(int argc, char** argv){
    auto mainApp = MainApplication::create();
    return mainApp->run(argc, argv);
}