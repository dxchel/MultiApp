#pragma once

#define HOME_URL "https://dxchel.github.io/portfolio"

#include <gtkmm.h>
#include <webkit/webkit.h>


/**
 * @brief Browser class containing important Widgets and functions for Browser functionality.
 *
 * Gtk::Box implementing class that contains important Gtk Widgets
 * for loading, displaying and directing web pages into a Gkt::Box.
 */
class Browser : public Gtk::Box {
    friend class BrowserTest;
    friend class BrowserTest_BrowserFunctionalTest_Test;

public:
    /**
     * @brief Creates Browser object with all needed Widgets and signals.
     *
     * Creates Browser object using resources/gtk/browser_app.ui file,
     * Checks for file issues, gets Widgets and connects needed signals.
     */
    Browser();

    /**
     * @brief Function for webView load events.
     *
     * Detect load event type and modify Browser widgets accordingly
     * for stopping load and disabling input.
     *
     * @param[in] web_view: webView that fired event.
     * @param[in] load_event: Load event detected.
     * @param[in] user_data: User provided data, will use to pass browser object.
     */
    static void web_view_load_changed(WebKitWebView *web_view,
                                      const WebKitLoadEvent load_event,
                                      gpointer user_data);
    /**
     * @brief Function for webView load in new tab events.
     *
     * Detect load _blank event type and modify Browser widgets accordingly
     * for stopping load and disabling input.
     *
     * @param[in] web_view: webView that fired event.
     * @param[in] action: Action detected.
     * @param[in] user_data: User provided data, will use to pass browser object.
     */
    static GtkWidget *on_create_cb(WebKitWebView *web_view,
                                   WebKitNavigationAction *action,
                                   gpointer user_data);

private:
    Gtk::Label      *status_label{};

    WebKitWebView   *web_view{};
    Gtk::Box        *header{};
    Gtk::Button     *back_button{};
    Gtk::Button     *forward_button{};
    Gtk::Button     *home_button{};
    Gtk::Button     *reload_button{};
    Gtk::Button     *enter_button{};
    Gtk::Entry      *uri_entry{};
    Gtk::MenuButton *menu_button{};

    /**
     * @brief Removes initial http(s)?:// and www. substrings from uri.
     *
     * Uses RegEx for leaving only site and options from uri.
     *
     * @param[in] uri: Uri to get information from.
     *
     * @return uri without http(s)?:// and www. substrings.
     */
    static std::string get_uri_root(const std::string& uri);

    /**
     * @brief Run checks and load web page requested into webView.
     *
     * Runs checks on uriEntry input for deciding if user wants a webpage or search.
     * Also adds www. and https:// when needed.
     *
     * @param[in] uri: Uri overload for debug and internal purposes.
     */
    void entry_uri_load(std::string uri="") const;

    /**
     * @brief Get Main Application status label.
     *
     * Gets Main Application status label after realize due to
     * the box needing to be contained.
     */
    void on_realize() override;
};
