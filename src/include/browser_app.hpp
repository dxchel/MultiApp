#ifndef _BROWSER_APP_
#define _BROWSER_APP_

#define HOME_URL "https://dxchel.github.io/portfolio"

#include <gtkmm.h>
#include <webkit/webkit.h>


/**
 * @brief Browser class containing important Widgets and functions for Browser functionality.
 * 
 * Gtk::Box implementing class that contains important Gtk Widgets
 * for loading, displaying and directing web pages into a Gkt::Box.
 */
class Browser : public Gtk::Box
{
    friend class BrowserTest;

    WebKitWebView *webView {};
    Gtk::Box *header {};
    Gtk::ScrolledWindow *scroller {};
    Gtk::Button *backButton {};
    Gtk::Button *forwardButton {};
    Gtk::Button *homeButton {};
    Gtk::Button *reloadButton {};
    Gtk::Entry *uriEntry {};
    Gtk::Button *enterButton {};
    Gtk::MenuButton *menuButton {};

    Gtk::Label *statusLabel {};

    /**
     * @brief Removes initial http(s)?:// and www. substrings from uri.
     * 
     * 
     * Uses RegEx for leaving only site and options from uri.
     * 
     * @param[in] uri: Uri to get information from.
     * 
     * @return uri without http(s)?:// and www. substrings.
     */
    static std::string get_uri_root(const std::string&);

    /**
     * @brief Run checks and load web page requested into webView.
     * 
     * Runs checks on uriEntry input for deciding if user wants a webpage or search.
     * Also adds www. and https:// when needed.
     * 
     * @param[in] uri: Uri overload for debug and internal purposes.
     */
    void entry_uri_load(std::string uri="") const;

public:
    /**
     * @brief Creates Browser object with all needed Widgets and signals.
     * 
     * Creates Browser object using res/gtk/browser_app.ui file,
     * Checks for file issues, gets Widgets and connects needed signals.
     */
    Browser();

    /**
     * @brief Set browser status label
     * 
     * @param[in] label: Gtk::Label to use as status label.
     */
    void setStatusLabel(Gtk::Label *label);

    /**
     * @brief Function for webView load events.
     * 
     * Detect load event type and modify Browser widgets accordingly
     * for stopping load and disabling input.
     * 
     * @param[in] webView: webView that fired event.
     * @param[in] loadEvent: Load event detected.
     * @param[in] userData: User provided data, will use to pass browser object.
     */
    static void web_view_load_changed(WebKitWebView *webView,
                                      const WebKitLoadEvent loadEvent,
                                      gpointer userData);
};

#endif