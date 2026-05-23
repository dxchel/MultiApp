#include "include/tests.hpp"

#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>


BrowserTest::BrowserTest() : browser (Gtk::manage(new Browser())),
    web_view {browser->web_view}, header {browser->header}, back_button {browser->back_button},
    forward_button {browser->forward_button}, home_button {browser->home_button},
    reload_button {browser->reload_button}, enter_button {browser->enter_button},
    uri_entry {browser->uri_entry}, menu_button {browser->menu_button} {};

TEST_F(BrowserTest, BrowserStructuralTest)
{
    ASSERT_THAT(browser, ::testing::NotNull());
    auto header_t {dynamic_cast<Gtk::Box *>(browser->get_first_child())};
    ASSERT_THAT(header_t, ::testing::NotNull());
    ASSERT_THAT(header_t, ::testing::Eq(header));
    auto back_t {dynamic_cast<Gtk::Button *>(header->get_first_child())};
    ASSERT_THAT(back_t, ::testing::NotNull());
    ASSERT_THAT(back_t, ::testing::Eq(back_button));
    auto forward_t {dynamic_cast<Gtk::Button *>(back_t->get_next_sibling())};
    ASSERT_THAT(forward_t, ::testing::NotNull());
    ASSERT_THAT(forward_t, ::testing::Eq(forward_button));
    auto home_t {dynamic_cast<Gtk::Button *>(forward_t->get_next_sibling())};
    ASSERT_THAT(home_t, ::testing::NotNull());
    ASSERT_THAT(home_t, ::testing::Eq(home_button));
    auto reload_t {dynamic_cast<Gtk::Button *>(home_t->get_next_sibling())};
    ASSERT_THAT(reload_t, ::testing::NotNull());
    ASSERT_THAT(reload_t, ::testing::Eq(reload_button));
    auto entry_t {dynamic_cast<Gtk::Entry *>(reload_t->get_next_sibling())};
    ASSERT_THAT(entry_t, ::testing::NotNull());
    ASSERT_THAT(entry_t, ::testing::Eq(uri_entry));
    auto enter_t {dynamic_cast<Gtk::Button *>(entry_t->get_next_sibling())};
    ASSERT_THAT(enter_t, ::testing::NotNull());
    ASSERT_THAT(enter_t, ::testing::Eq(enter_button));
    auto menu_t {dynamic_cast<Gtk::MenuButton *>(enter_t->get_next_sibling())};
    EXPECT_THAT(menu_t, ::testing::Eq(menu_button));
    auto web_view_t {header_t->get_next_sibling()};
    EXPECT_THAT(web_view_t, ::testing::NotNull());
}

TEST_F(BrowserTest, BrowserFunctionalTest)
{
    ASSERT_THAT(Browser::get_uri_root(webkit_web_view_get_uri(browser->web_view)), ::testing::Eq("dxchel.github.io/portfolio"));

    std::vector<std::string> uris_to_load
    {
        "google.com/",
        "https://github.com/",
        "www.github.com/",
        "github.com/",
        "roadmap.sh/",
        "youtube.com/",
        "www.github.com/dxchel/MultiApp/",
        "Hello World",
    };
    std::string previous_uri {""};
    std::stack<std::string> uri_stack {};
    for(auto &uri : uris_to_load)
    {
        uri_entry->set_text(uri);
        browser->entry_uri_load();
        if(Browser::get_uri_root(uri) != Browser::get_uri_root(previous_uri))
        {
            uri_stack.push(uri);
            previous_uri = uri;
        }
        if(uri.find(' ') < uri.size())
        {
            std::replace(uri.begin(), uri.end(), ' ', '+');
            uri = "https://www.google.com/search?q=" + uri;
        }
        ASSERT_EQ(Browser::get_uri_root(uri), Browser::get_uri_root(webkit_web_view_get_uri(web_view))) <<
            "WebView not directed to " << uri << ", got " << webkit_web_view_get_uri(web_view) << " instead." << std::endl;
    }
}