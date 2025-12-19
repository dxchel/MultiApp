#include "include/tests.hpp"

#include <iostream>
#include <regex>

#include <gtest/gtest.h>


BrowserTest::BrowserTest() : browser (Gtk::manage(new Browser())), webView {browser->webView},
    header {browser->header}, scroller {browser->scroller}, backButton {browser->backButton},
    forwardButton {browser->forwardButton}, homeButton {browser->homeButton},
    reloadButton {browser->reloadButton}, uriEntry {browser->uriEntry},
    enterButton {browser->enterButton}, menuButton {browser->menuButton} {};

std::string BrowserTest::get_uri_root(const std::string& uri) { return Browser::get_uri_root(uri); }
void BrowserTest::entry_uri_load(std::string uri) const { browser->entry_uri_load(uri); }

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
    std::vector<std::string> urisToLoad
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
    std::string previousUri {""};
    std::stack<std::string> uriStack {};
    for(auto &uri : urisToLoad)
    {
        uriEntry->set_text(uri);
        entry_uri_load();
        if(get_uri_root(uri) != get_uri_root(previousUri))
        {
            uriStack.push(uri);
            previousUri = uri;
        }
        if(uri.find(' ') < uri.size())
        {
            std::replace(uri.begin(), uri.end(), ' ', '+');
            uri = "https://www.google.com/search?q=" + uri;
        }
        ASSERT_EQ(get_uri_root(uri), get_uri_root(webkit_web_view_get_uri(webView))) <<
            "WebView not directed to " << uri << ", got " << webkit_web_view_get_uri(webView) << " instead." << std::endl;
    }
}