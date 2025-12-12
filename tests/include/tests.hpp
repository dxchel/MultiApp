#ifndef _TESTERS_
#define _TESTERS_

#include "../../include/main_application.hpp"
#include "../../include/browser_app.hpp"


class MainApplicationTester
{
    Glib::RefPtr<MainApplication> mainApp;

public:
    MainApplicationTester();
    MainApplicationError mainApplicationStructureTests();
};

class BrowserAppTester
{
    Browser *browser;

public:
    BrowserAppTester();
    BrowserAppError browserAppStructureTests();
    BrowserAppError browserAppFunctionalTests();
};

#endif