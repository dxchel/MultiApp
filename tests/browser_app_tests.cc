#include "include/tests.hpp"

#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>


BrowserTest::BrowserTest() : browser (Gtk::manage(new Browser())), web_view {browser->web_view},
    header {browser->header}, back_button {browser->back_button},
    forward_button {browser->forward_button}, home_button {browser->home_button},
    reload_button {browser->reload_button}, uri_entry {browser->uri_entry},
    enter_button {browser->enter_button}, menu_button {browser->menu_button} {};

std::string BrowserTest::get_uri_root(const std::string& uri) { return Browser::get_uri_root(uri); }
void BrowserTest::entry_uri_load(std::string uri) const { browser->entry_uri_load(uri); }

TEST_F(BrowserTest, BrowserStructuralTest)
{
    ASSERT_THAT(browser, ::testing::NotNull());
    auto header {dynamic_cast<Gtk::Box *>(browser->get_first_child())};
    ASSERT_THAT(header, ::testing::NotNull());
    auto back {dynamic_cast<Gtk::Button *>(header->get_first_child())};
    ASSERT_THAT(back, ::testing::NotNull());
    auto forward {dynamic_cast<Gtk::Button *>(back->get_next_sibling())};
    ASSERT_THAT(forward, ::testing::NotNull());
    auto home {dynamic_cast<Gtk::Button *>(forward->get_next_sibling())};
    ASSERT_THAT(home, ::testing::NotNull());
    auto reload {dynamic_cast<Gtk::Button *>(home->get_next_sibling())};
    ASSERT_THAT(reload, ::testing::NotNull());
    auto entry {dynamic_cast<Gtk::Entry *>(reload->get_next_sibling())};
    ASSERT_THAT(entry, ::testing::NotNull());
    auto enter {dynamic_cast<Gtk::Button *>(entry->get_next_sibling())};
    ASSERT_THAT(enter, ::testing::NotNull());
    auto menu {dynamic_cast<Gtk::MenuButton *>(enter->get_next_sibling())};
    EXPECT_THAT(menu, ::testing::NotNull());
}

TEST_F(BrowserTest, BrowserFunctionalTest)
{
    // TODO: Need to check how to simulate button clicks instead of calling functions manually
    // TODO: Need to check how to make webView load pages without showing anything
    /*if(browser->backButton->get_sensitive())
    {
        std::cerr << "Back button not initialized as disabled." << std::endl;
        return BrowserAppError::back_error;
    }
    if(browser->forwardButton->get_sensitive())
    {
        std::cerr << "Back button not initialized as disabled." << std::endl;
        return BrowserAppError::forward_error;
    }
    if(Browser::get_uri_root(webkit_web_view_get_uri(browser->webView)) != "github.com/dxchel/MultiApp")
    {
        std::cerr << "WebView not initialized to https://www.github.com/dxchel/MultiApp." << std::endl;
        return BrowserAppError::webview_error;
    }*/
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
        entry_uri_load();
        if(get_uri_root(uri) != get_uri_root(previous_uri))
        {
            uri_stack.push(uri);
            previous_uri = uri;
        }
        if(uri.find(' ') < uri.size())
        {
            std::replace(uri.begin(), uri.end(), ' ', '+');
            uri = "https://www.google.com/search?q=" + uri;
        }
        ASSERT_EQ(get_uri_root(uri), get_uri_root(webkit_web_view_get_uri(web_view))) <<
            "WebView not directed to " << uri << ", got " << webkit_web_view_get_uri(web_view) << " instead." << std::endl;
    }
}