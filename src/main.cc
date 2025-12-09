#include "../include/main_application.hpp"


int main(int argc, char** argv){
    auto main_app = MainApplication::create();
    return main_app->run(argc, argv);
}