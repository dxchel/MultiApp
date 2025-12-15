#ifndef _TESTERS_
#define _TESTERS_

#include "../../src/include/main_application.hpp"
#include "../../src/include/browser_app.hpp"


/**
 * @brief Contains MainApplication object and tests.
 * 
 * Class to create an object with test functions for the MainApplication class,
 * only contains a MainApplication object and Structural tests function.
 * */
class MainApplicationTester
{
    Glib::RefPtr<MainApplication> mainApp;

public:
    /**
     * @brief Creates MainApplication object mainApp.
     * 
     * Obtains MainApplication object using default constructor for testing.
     * */
    MainApplicationTester();

    /**
     * @brief Runs structural tests on the object's mainApp.
     * 
     * Runs structural tests to check if all of mainApp
     * objects are present and in the correct order.
     * 
     * @return MainApplicationError class error code.
     */
    MainApplicationError mainApplicationStructureTests() const;
};


/**
 * @brief Contains Browser object and Structural/Functional tests.
 * 
 * Class to create an object with test functions for the Browser class,
 * only contains a Browser object, Structural and Functional test functions.
 * */
class BrowserAppTester
{
    Browser *browser;

public:
    /**
     * @brief Creates Browser object browser.
     * 
     * Obtains Browser object using default constructor for testing.
     * */
    BrowserAppTester();

    /**
     * @brief Runs structural tests on the object's browser.
     * 
     * Runs structural tests to check if all of browser
     * objects are present and in the correct order.
     * 
     * @return BrowserAppError class error code.
     */
    BrowserAppError browserAppStructureTests() const;

    /**
     * @brief Runs functional tests on the object's browser.
     * 
     * Runs functional tests to check if all of browser methods are work as intended by
     * loading uris into the webView and checking if resulting page is correct.
     * 
     * @return BrowserAppError class error code.
     */
    BrowserAppError browserAppFunctionalTests() const;
};

#endif