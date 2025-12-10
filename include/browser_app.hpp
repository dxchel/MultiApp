#ifndef _BROWSER_APP_
#define _BROWSER_APP_

#include <gtkmm.h>
#include <webkit/webkit.h>
#include <iostream>

class Browser : public Gtk::Box
{
    WebKitWebView *webView{};
    Gtk::Box *header{};
    Gtk::ScrolledWindow *scroller{};

public:
    Browser();
};

#endif