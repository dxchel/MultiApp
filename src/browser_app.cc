#include "../include/browser_app.hpp"


Browser::Browser() : Gtk::Box(Gtk::Orientation::VERTICAL)
{
    // Load the GtkBuilder file and instantiate its widgets:
    auto refBuilder{Gtk::Builder::create()};
    try
    {
        refBuilder->add_from_file("res/browser_app.ui");
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
    header = Gtk::manage(refBuilder->get_widget<Gtk::Box>("header_bar"));

    // Get the GtkBuilder-instantiated browser_scroller, and connect WebKitWebView
    scroller = Gtk::manage(new Gtk::ScrolledWindow());
    webView = WEBKIT_WEB_VIEW(webkit_web_view_new());
    webkit_web_view_load_uri(webView, "https://www.google.com/");
    Gtk::Widget *webViewWidget{Gtk::manage(Glib::wrap(GTK_WIDGET(webView)))};
    webViewWidget->set_name("browser_webview");
    webViewWidget->set_vexpand(true);
    scroller->set_child(*webViewWidget);

    // Get the GtkBuilder-instantiated buttons, and connect a signal handler
    auto homeButton{refBuilder->get_widget<Gtk::Button>("home_button")};
    if(homeButton)
        homeButton->signal_clicked().connect([this] () { webkit_web_view_load_uri(webView, "https://www.google.com");});

    auto refreshButton{refBuilder->get_widget<Gtk::Button>("reload_button")};
    if(refreshButton)
        refreshButton->signal_clicked().connect([this] () { webkit_web_view_reload(webView);});


    insert_child_at_start(*header);
    insert_child_after(*scroller, *header);
}