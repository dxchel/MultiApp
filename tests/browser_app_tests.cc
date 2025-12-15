#include "include/tests.hpp"

#include <iostream>
#include <regex>


BrowserAppTester::BrowserAppTester() : browser (Gtk::manage(new Browser())) {};

BrowserAppError BrowserAppTester::browserAppStructureTests() const
{
    if(!browser)
    {
        std::cerr << "Browser Box Error." << std::endl;
        return BrowserAppError::browser_error;
    }
    auto header {dynamic_cast<Gtk::Box *>(browser->get_first_child())};
    if(!header)
    {
        std::cerr << "Header Bar Error."  << std::endl;
        return BrowserAppError::header_error;
    }
    auto back {dynamic_cast<Gtk::Button *>(header->get_first_child())};
    if(!back)
    {
        std::cerr << "Back Button Error in header." << std::endl;
        return BrowserAppError::back_error;
    }
    auto forward {dynamic_cast<Gtk::Button *>(back->get_next_sibling())};
    if(!forward)
    {
        std::cerr << "Forward Button Error in header." << std::endl;
        return BrowserAppError::forward_error;
    }
    auto home {dynamic_cast<Gtk::Button *>(forward->get_next_sibling())};
    if(!home)
    {
        std::cerr << "Home Button Error in header." << std::endl;
        return BrowserAppError::home_error;
    }
    auto reload {dynamic_cast<Gtk::Button *>(home->get_next_sibling())};
    if(!reload)
    {
        std::cerr << "Reload Button Error in header." << std::endl;
        return BrowserAppError::reload_error;
    }
    auto entry {dynamic_cast<Gtk::Entry *>(reload->get_next_sibling())};
    if(!entry)
    {
        std::cerr << "Entry Button Error in header." << std::endl;
        return BrowserAppError::entry_error;
    }
    auto enter {dynamic_cast<Gtk::Button *>(entry->get_next_sibling())};
    if(!enter)
    {
        std::cerr << "Enter Button Error in header." << std::endl;
        return BrowserAppError::enter_error;
    }
    auto menu {dynamic_cast<Gtk::MenuButton *>(enter->get_next_sibling())};
    if(!menu)
    {
        std::cerr << "Menu Button Error in header." << std::endl;
        return BrowserAppError::menu_error;
    }
    return BrowserAppError::no_error;
}

BrowserAppError BrowserAppTester::browserAppFunctionalTests() const
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
        browser->uriEntry->set_text(uri);
        browser->entry_uri_load();
        if(Browser::get_uri_root(uri) != Browser::get_uri_root(previousUri))
        {
            uriStack.push(uri);
            previousUri = uri;
        }
        webkit_web_view_go_back(browser->webView);
        if(uri.find(' ') < uri.size())
        {
            std::replace(uri.begin(), uri.end(), ' ', '+');
            uri = "https://www.google.com/search?q=" + uri;
        }
        if(Browser::get_uri_root(uri) != Browser::get_uri_root(webkit_web_view_get_uri(browser->webView)))
        {
            std::cerr << "WebView not directed to " << uri << ", got " << webkit_web_view_get_uri(browser->webView) << " instead." << std::endl;
            return BrowserAppError::webview_error;
        }
    }
    return BrowserAppError::no_error;
}