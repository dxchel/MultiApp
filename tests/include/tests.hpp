#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../src/include/main_application.hpp"
#include "../../src/include/browser_app.hpp"
#include "../../src/include/fractal_app.hpp"
#include "../../src/include/chat_app.hpp"


/**
 * @brief Contains MainApplication object and tests.
 *
 * Class to create an object with test functions for the MainApplication class,
 * only contains a MainApplication object and Structural tests function.
 * */
class MainApplicationTest : public testing::Test {

protected:
    /**
     * @brief Creates MainApplication object mainApp.
     *
     * Obtains MainApplication object using default constructor for testing.
     * */
    MainApplicationTest();

    Glib::RefPtr<MainApplication> main_app;
};


/**
 * @brief Contains Browser object and Structural/Functional tests.
 *
 * Class to create an object with test functions for the Browser class,
 * only contains Browser class objects, Structural and Functional test functions.
 * */
class BrowserTest : public testing::Test {
protected:
    /**
     * @brief Creates Browser object browser.
     *
     * Obtains Browser object using default constructor for testing.
     * */
    BrowserTest();

    Browser         *browser{};
    WebKitWebView   *web_view{};
    Gtk::Box        *header{};
    Gtk::Button     *back_button{};
    Gtk::Button     *forward_button{};
    Gtk::Button     *home_button{};
    Gtk::Button     *reload_button{};
    Gtk::Button     *enter_button{};
    Gtk::Entry      *uri_entry{};
    Gtk::MenuButton *menu_button{};
};


/**
 * @brief Contains Fractal object and Structural/Functional tests.
 *
 * Class to create an object with test functions for the Fractal class,
 * only contains Fractal class objects, Structural and Functional test functions.
 * */
class FractalTest : public testing::Test {
protected:
    /**
     * @brief Creates Fractal object fractal.
     *
     * Obtains Fractal object using default constructor for testing.
     * */
    FractalTest();

    FractalBox    *fractal{};
    Gtk::Box      *menu{};
    Gtk::Box      *consts_box{};
    Gtk::Button   *reset_button{};
    Gtk::Button   *save_button{};
    Gtk::DropDown *fractal_dropdown{};
    FractalArea   *fractal_area{};
    Gtk::Label    *status_label{};
    Gtk::Scale    *iter_scale{};
    Gtk::Scale    *const_r_scale{};
    Gtk::Scale    *const_i_scale{};
    Gtk::Scale    *r_min_scale{};
    Gtk::Scale    *r_max_scale{};
    Gtk::Scale    *g_min_scale{};
    Gtk::Scale    *g_max_scale{};
    Gtk::Scale    *b_min_scale{};
    Gtk::Scale    *b_max_scale{};

    std::string selection{};
};


/**
 * @brief Contains Chat object and Structural/Functional tests.
 *
 * Class to create an object with test functions for the Chat class,
 * only contains Chat class objects, Structural and Functional test functions.
 * */
class ChatTest : public testing::Test {
protected:
    /**
     * @brief Creates Chat object chat.
     *
     * Obtains chat object using default constructor for testing.
     * */
    ChatTest();

    Chat *chat{};

    Gtk::Button         *home_button{};
    Gtk::Button         *connect_button{};
    Gtk::Button         *message_button{};
    Gtk::Entry          *ip_entry{};
    Gtk::Entry          *port_entry{};
    Gtk::Entry          *message_entry{};
    Gtk::Box            *chat_box{};
    Gtk::Box            *footer_box{};
    Gtk::ScrolledWindow *chat_scrolled{};
    Gtk::Label          *status_label{};
};
