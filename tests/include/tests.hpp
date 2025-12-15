#ifndef _TESTERS_
#define _TESTERS_

#include "../../src/include/main_application.hpp"
#include "../../src/include/browser_app.hpp"


class MainApplicationTester
{
    Glib::RefPtr<MainApplication> mainApp;

public:
    MainApplicationTester();
    MainApplicationError mainApplicationStructureTests() const;
};

class BrowserAppTester
{
    Browser *browser;

public:
    BrowserAppTester();
    BrowserAppError browserAppStructureTests() const;
    BrowserAppError browserAppFunctionalTests() const;
};

#endif