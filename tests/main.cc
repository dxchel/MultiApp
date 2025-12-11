#include "include/tests.hpp"


int main(int argc, char** argv)
{
    int exit_code {};
    exit_code += static_cast<int>(mainApplicationTests());
    exit_code += static_cast<int>(browserAppTests());
    return exit_code;
}