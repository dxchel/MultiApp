#include "../include/browser_app.hpp"


void Browser::url_load()
{
    std::string url {urlEntry->get_text()};
    // if(std::regex_search(url, "^[A-Za-z0-9]+\\.[A-Za-z0-9]+$"))
    if(url.find("www.") > url.size())
        url = "www." + url;
    if(url.find("https://") > url.size())
        url = "https://" + url;
    webkit_web_view_load_uri(webView, url.c_str());
    std::cout << webkit_web_view_get_uri(webView) << std::endl;
}

Browser::Browser() : Gtk::Box(Gtk::Orientation::VERTICAL), webView {}, header {}, scroller {}
{
    // Load the GtkBuilder file and instantiate its widgets, check for errors
    auto refBuilder {Gtk::Builder::create()};
    try
    {
        refBuilder->add_from_file("res/gtk/browser_app.ui");
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

    // Get the GtkBuilder-instantiated browser_scroller, and connect WebKitWebView with home site loaded
    scroller = Gtk::manage(new Gtk::ScrolledWindow());
    webView = WEBKIT_WEB_VIEW(webkit_web_view_new());
    webkit_web_view_load_uri(webView, "https://www.google.com/");
    Gtk::Widget *webViewWidget {Gtk::manage(Glib::wrap(GTK_WIDGET(webView)))};
    webViewWidget->set_name("browser_webview");
    webViewWidget->set_vexpand(true);
    scroller->set_child(*webViewWidget);

    // Get the GtkBuilder-instantiated buttons, and connect a signal handler
    auto backButton {refBuilder->get_widget<Gtk::Button>("back_button")};
    if(backButton)
        backButton->signal_clicked().connect([this](){ webkit_web_view_go_back(webView);});
    auto forwardButton {refBuilder->get_widget<Gtk::Button>("forward_button")};
    if(forwardButton)
        forwardButton->signal_clicked().connect([this](){ webkit_web_view_go_forward(webView);});
    auto homeButton {refBuilder->get_widget<Gtk::Button>("home_button")};
    if(homeButton)
        homeButton->signal_clicked().connect([this](){ webkit_web_view_load_uri(webView, "https://www.google.com");});
    auto refreshButton {refBuilder->get_widget<Gtk::Button>("reload_button")};
    if(refreshButton)
        refreshButton->signal_clicked().connect([this](){ webkit_web_view_reload(webView);});
    urlEntry = refBuilder->get_widget<Gtk::Entry>("header_entry");
    auto enterButton {refBuilder->get_widget<Gtk::Button>("enter_button")};
    if(urlEntry){
        urlEntry->signal_activate().connect([this](){ url_load();});
        if(enterButton)
            enterButton->signal_clicked().connect([this](){ url_load();});
    }

    // Insert elements into Browser Box
    insert_child_at_start(*header);
    insert_child_after(*scroller, *header);
    set_name("main_browser");
}