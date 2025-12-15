#ifndef _BROWSER_APP_
#define _BROWSER_APP_

#include <gtkmm.h>
#include <webkit/webkit.h>

#include <iostream>
#include <regex>


class Browser : public Gtk::Box
{
    friend class BrowserAppTester;

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

    static std::string get_uri_root(const std::string&);
    void entry_uri_load(std::string uri="") const;

public:
    Browser();
    static void web_view_load_changed(WebKitWebView *web_view,
                                      const WebKitLoadEvent load_event,
                                      gpointer user_data);
};

#endif