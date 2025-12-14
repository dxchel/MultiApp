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
    Gtk::Button *backButton;
    Gtk::Button *forwardButton;
    Gtk::Button *homeButton;
    Gtk::Button *reloadButton;
    Gtk::Entry *uriEntry;
    Gtk::Button *enterButton;
    Gtk::MenuButton *menuButton;

    void entry_uri_load(std::string uri);

public:
    Browser();
    static void web_view_load_changed(WebKitWebView *web_view,
                                      WebKitLoadEvent load_event,
                                      gpointer user_data);
};

#endif