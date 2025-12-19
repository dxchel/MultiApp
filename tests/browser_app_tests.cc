#include "include/tests.hpp"

#include <iostream>
#include <regex>

#include <gtest/gtest.h>


BrowserTest::BrowserTest() : browser (Gtk::manage(new Browser())) {};

TEST_F(BrowserTest, BrowserStructuralTest)
{
    ASSERT_NE(browser, nullptr);
    auto header {dynamic_cast<Gtk::Box *>(browser->get_first_child())};
    ASSERT_NE(header, nullptr);
    auto back {dynamic_cast<Gtk::Button *>(header->get_first_child())};
    ASSERT_NE(back, nullptr);
    auto forward {dynamic_cast<Gtk::Button *>(back->get_next_sibling())};
    ASSERT_NE(forward, nullptr);
    auto home {dynamic_cast<Gtk::Button *>(forward->get_next_sibling())};
    ASSERT_NE(home, nullptr);
    auto reload {dynamic_cast<Gtk::Button *>(home->get_next_sibling())};
    ASSERT_NE(reload, nullptr);
    auto entry {dynamic_cast<Gtk::Entry *>(reload->get_next_sibling())};
    ASSERT_NE(entry, nullptr);
    auto enter {dynamic_cast<Gtk::Button *>(entry->get_next_sibling())};
    ASSERT_NE(enter, nullptr);
    auto menu {dynamic_cast<Gtk::MenuButton *>(enter->get_next_sibling())};
    ASSERT_NE(menu, nullptr);
}
