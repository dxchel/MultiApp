#include "include/tests.hpp"

#include <iostream>

#include <gtest/gtest.h>


MainApplicationTest::MainApplicationTest() : mainApp (MainApplication::create()) {};


Gtk::ApplicationWindow* MainApplicationTest::create_window()
{
    return mainApp->create_window();
}

TEST_F(MainApplicationTest, MainApplicationStructureTest)
{
    ASSERT_NE(mainApp, nullptr);
    auto window {dynamic_cast<Gtk::Window *>(create_window())};
    ASSERT_NE(window, nullptr);
    auto box {dynamic_cast<Gtk::Box *>(window->get_child())};
    ASSERT_NE(box, nullptr);
    auto notebook {dynamic_cast<Gtk::Notebook *>(box->get_first_child())};
    ASSERT_NE(notebook, nullptr);
    auto foot {dynamic_cast<Gtk::Label *>(notebook->get_next_sibling())};
    EXPECT_NE(foot, nullptr);
    auto browser {dynamic_cast<Gtk::Box *>(notebook->get_first_child())};
    ASSERT_NE(browser, nullptr);
}
