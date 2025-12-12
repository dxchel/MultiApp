#ifndef _BROWSER_APP_
#define _BROWSER_APP_

#include <gtkmm.h>
#include <webkit/webkit.h>

#include <iostream>
#include <regex>


enum class BrowserAppError
{
    no_error,
    browser_error,
    header_error,
    back_error,
    forward_error,
    home_error,
    reload_error,
    entry_error,
    enter_error,
    menu_error,
    scroller_error,
    webview_error
};

class Browser : public Gtk::Box
{
    WebKitWebView *webView;
    Gtk::Box *header;
    Gtk::ScrolledWindow *scroller;
    Gtk::Entry *urlEntry;
    void url_load(std::string url);

public:
    Browser();
};

#endif