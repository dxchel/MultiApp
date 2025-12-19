#ifndef _TESTERS_
#define _TESTERS_

#include <gtest/gtest.h>

#include "../../src/include/main_application.hpp"
#include "../../src/include/browser_app.hpp"


/**
 * @brief Contains MainApplication object and tests.
 * 
 * Class to create an object with test functions for the MainApplication class,
 * only contains a MainApplication object and Structural tests function.
 * */
class MainApplicationTest : public testing::Test
{


protected:
    /**
     * @brief Creates MainApplication object mainApp.
     * 
     * Obtains MainApplication object using default constructor for testing.
     * */
    MainApplicationTest();

    Glib::RefPtr<MainApplication> mainApp;

    Gtk::ApplicationWindow* create_window();
};


/**
 * @brief Contains Browser object and Structural/Functional tests.
 * 
 * Class to create an object with test functions for the Browser class,
 * only contains a Browser object, Structural and Functional test functions.
 * */
class BrowserTest : public testing::Test
{
protected:
    /**
     * @brief Creates Browser object browser.
     * 
     * Obtains Browser object using default constructor for testing.
     * */
    BrowserTest();

    Browser *browser;
};

/**
 * @brief Runs structural tests on the object's mainApp.
 * 
 * Runs structural tests to check if all of mainApp
 * objects are present and in the correct order.
 */
// TEST_F(MainApplicationTest, MainApplicationStructureTests);

/**
 * @brief Runs structural tests on the object's browser.
 * 
 * Runs structural tests to check if all of browser
 * objects are present and in the correct order.
 */
// TEST_F(BrowserTest, BrowserStructureTests);

/**
 * @brief Runs functional tests on the object's browser.
 * 
 * Runs functional tests to check if all of browser methods are work as intended by
 * loading uris into the webView and checking if resulting page is correct.
 */
//  TEST_F(BrowserTest, BrowserFunctionalTests);
#endif