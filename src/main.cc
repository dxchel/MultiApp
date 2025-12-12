#include "../include/main_application.hpp"


int main(int argc, char** argv)
{
    MainApplicationTester mainAppTester {};
    mainAppTester.mainApplicationStructureTests();
    BrowserAppTester browserAppTester {};
    return mainApp->run(argc, argv);
}