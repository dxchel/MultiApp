#include "include/tests.hpp"


int main(int argc, char** argv)
{
    int exit_code {};
    MainApplicationTester mainApplicationTester {};
    BrowserAppTester browserAppTester {};
    exit_code += static_cast<int>(mainApplicationTester.mainApplicationStructureTests());
    exit_code += static_cast<int>(browserAppTester.browserAppStructureTests());
    return exit_code;
}