#include "include/tests.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>


MainApplicationTest::MainApplicationTest() : main_app (MainApplication::create()) {};


TEST_F(MainApplicationTest, MainApplicationStructureTest)
{
    ASSERT_THAT(main_app, ::testing::NotNull());
    auto window {dynamic_cast<Gtk::Window *>(main_app->create_window())};
    ASSERT_THAT(window, ::testing::NotNull());
    auto box {dynamic_cast<Gtk::Box *>(window->get_child())};
    ASSERT_THAT(box, ::testing::NotNull());
    auto notebook {dynamic_cast<Gtk::Notebook *>(box->get_first_child())};
    ASSERT_THAT(notebook, ::testing::NotNull());
    auto foot {dynamic_cast<Gtk::Label *>(notebook->get_next_sibling())};
    EXPECT_THAT(foot, ::testing::NotNull());
    auto selected {dynamic_cast<Gtk::Box *>(notebook->get_first_child())};
    ASSERT_THAT(selected, ::testing::NotNull());
}
