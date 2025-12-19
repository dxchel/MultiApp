#include "include/tests.hpp"

#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>


MainApplicationTest::MainApplicationTest() : mainApp (MainApplication::create()) {};


Gtk::ApplicationWindow* MainApplicationTest::create_window() { return mainApp->create_window(); }

TEST_F(MainApplicationTest, MainApplicationStructureTest)
{
    ASSERT_THAT(mainApp, ::testing::NotNull());
    auto window {dynamic_cast<Gtk::Window *>(create_window())};
    ASSERT_THAT(window, ::testing::NotNull());
    auto box {dynamic_cast<Gtk::Box *>(window->get_child())};
    ASSERT_THAT(box, ::testing::NotNull());
    auto notebook {dynamic_cast<Gtk::Notebook *>(box->get_first_child())};
    ASSERT_THAT(notebook, ::testing::NotNull());
    auto foot {dynamic_cast<Gtk::Label *>(notebook->get_next_sibling())};
    EXPECT_THAT(foot, ::testing::NotNull());
    auto browser {dynamic_cast<Gtk::Box *>(notebook->get_first_child())};
    ASSERT_THAT(browser, ::testing::NotNull());
}
