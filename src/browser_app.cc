#include "include/browser_app.hpp"

#include <iostream>
#include <regex>

#include <gtkmm.h>
#include <webkit/webkit.h>


std::string Browser::get_uri_root(const std::string &uri)
{
    std::string result {std::regex_replace(uri, std::regex("(https?://|www\\.)"), "")};
    return result;
}

void Browser::entry_uri_load(std::string uri) const
{
    if(uri == "") [[unlikely]]
        // Get entry text
        uri = uri_entry->get_text();

    // Is current URI a web page.
    if(uri.find(' ') > uri.size() &&
        std::regex_search(uri, std::regex("^(http(s)?://)?(www\\.)?[A-Za-z0-9.]+\\.[A-Za-z0-9/+-_?=#]+$"))) [[likely]]
    {
        // Add missing parts of the URL
        if(uri.find("http") > uri.size()) [[likely]]
        {
            if(uri.find("www.") > uri.size() && get_uri_root(uri) != get_uri_root(HOME_URL)) [[likely]]
                uri = "www." + uri;
            uri = "https://" + uri;
        }else if(uri.find("www.") > uri.size() && get_uri_root(uri) != get_uri_root(HOME_URL)) [[unlikely]]
            std::regex_replace(uri, std::regex("https?://"), "https://www.");
    }
    else [[unlikely]]
    {
        // Add as a google search
        std::replace(uri.begin(), uri.end(), ' ', '+');
        uri = "https://www.google.com/search?q=" + uri;
    }

    // Reload if the requested URI is the same as current
    if(get_uri_root(webkit_web_view_get_uri(web_view)) != get_uri_root(uri)) [[likely]]
        webkit_web_view_load_uri(web_view, uri.c_str());
    else [[unlikely]]
        webkit_web_view_reload(web_view);
}

Browser::Browser() : Gtk::Box(Gtk::Orientation::VERTICAL)
{
    // Load the GtkBuilder file and instantiate its widgets, check for errors
    auto ref_builder {Gtk::Builder::create()};
    try
    {
        ref_builder->add_from_file("res/gtk/browser_app.ui");
    }
    catch(const Glib::FileError& ex)
    {
        std::cerr << "FileError: " << ex.what() << std::endl;
        throw ex;
    }
    catch(const Glib::MarkupError& ex)
    {
        std::cerr << "MarkupError: " << ex.what() << std::endl;
        throw ex;
    }
    catch(const Gtk::BuilderError& ex)
    {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        throw ex;
    }

    // Get the GtkBuilder-instantiated nav and header:
    header = Gtk::manage(ref_builder->get_widget<Gtk::Box>("header_bar"));

    // Get the GtkBuilder-instantiated browser_scroller, and connect WebKitWebView with home site loaded
    web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
    auto web_view_widget {Glib::wrap(GTK_WIDGET(web_view))};
    g_object_ref_sink(web_view_widget->gobj());
    web_view_widget->set_name("browser_webview");
    web_view_widget->set_vexpand(true);

    // Get the GtkBuilder-instantiated buttons, and connect a signal handler
    back_button = ref_builder->get_widget<Gtk::Button>("back_button");
    forward_button = ref_builder->get_widget<Gtk::Button>("forward_button");
    home_button = ref_builder->get_widget<Gtk::Button>("home_button");
    reload_button = ref_builder->get_widget<Gtk::Button>("reload_button");
    uri_entry = ref_builder->get_widget<Gtk::Entry>("header_entry");
    enter_button = ref_builder->get_widget<Gtk::Button>("enter_button");
    menu_button = ref_builder->get_widget<Gtk::MenuButton>("header_menu");

    // Add Callbacks
    if(back_button) [[likely]]
        back_button->signal_clicked().connect
        (
            [this](){ webkit_web_view_go_back(web_view);}
        );
    if(forward_button) [[likely]]
        forward_button->signal_clicked().connect
        (
            [this](){ webkit_web_view_go_forward(web_view);}
        );
    if(home_button) [[likely]]
        home_button->signal_clicked().connect
        (
            [this](){ entry_uri_load(HOME_URL);}
        );
    if(reload_button) [[likely]]
        reload_button->signal_clicked().connect
        (
            [this]()
            {
                webkit_web_view_is_loading(web_view) ?
                webkit_web_view_stop_loading(web_view) :
                webkit_web_view_reload(web_view);
            }
        );
    if(uri_entry) [[likely]]
    {
        uri_entry->signal_activate().connect([this](){ entry_uri_load();});
        if(enter_button) [[likely]]
            enter_button->signal_clicked().connect([this](){ entry_uri_load();});
    }
    // Menu button not visible as no usage needed for the moment
    if(menu_button) [[likely]] menu_button->set_visible(false);

    g_signal_connect(web_view, "load-changed", G_CALLBACK(web_view_load_changed), this);
    webkit_web_view_load_uri(web_view, HOME_URL);

    // Insert elements into Browser Box
    insert_child_at_start(*header);
    append(*web_view_widget);
}

void Browser::on_realize() {
    Gtk::Box::on_realize();
    status_label = dynamic_cast<Gtk::Label *>(get_parent()->get_parent()->get_parent()->get_parent()->get_last_child());
    if (!status_label) std::cout << "Status label not found" << std::endl;
    if (status_label)
        status_label->set_text("Welcome to the Browser!");
}


void Browser::web_view_load_changed(WebKitWebView *webView,
                                    const WebKitLoadEvent loadEvent,
                                    gpointer userData)
{
    auto uri {webkit_web_view_get_uri(webView)};
    auto browser {static_cast<Browser*>(userData)};
    switch (loadEvent) {
        case WEBKIT_LOAD_STARTED:
            browser->back_button->set_sensitive(webkit_web_view_can_go_back(webView));
            browser->forward_button->set_sensitive(webkit_web_view_can_go_forward(webView));
            browser->reload_button->set_icon_name("gtk-stop");
            browser->uri_entry->set_sensitive(false);
            browser->enter_button->set_sensitive(false);
            std::cout << "Load started with provisional URI: " << uri << std::endl;
            break;

        case WEBKIT_LOAD_REDIRECTED:
            std::cout << "Redirected to URI: " << uri << std::endl;
            break;

        case WEBKIT_LOAD_COMMITTED:
            std::cout << "URL changed/committed to: " << uri << std::endl;
            break;

        case WEBKIT_LOAD_FINISHED:
            browser->back_button->set_sensitive(webkit_web_view_can_go_back(webView));
            browser->forward_button->set_sensitive(webkit_web_view_can_go_forward(webView));
            browser->reload_button->set_icon_name("gtk-convert");
            browser->uri_entry->set_sensitive(true);
            browser->uri_entry->set_text(uri);
            browser->enter_button->set_sensitive(true);
            if ( browser->status_label )
                browser->status_label->set_text(static_cast<std::string>("Welcome to the browser! You're in page: ") + webkit_web_view_get_title(webView));
            std::cout << "Load finished." << std::endl;
            break;
    }
}

