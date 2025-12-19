#include "include/browser_app.hpp"


std::string Browser::get_uri_root(const std::string &uri)
{
    std::string result {std::regex_replace(uri, std::regex("(https?://|www\\.)"), "")};
    return result;
}

void Browser::entry_uri_load(std::string uri) const
{
    if(uri == "") [[unlikely]]
        // Get entry text
        uri = uriEntry->get_text();

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
    if(get_uri_root(webkit_web_view_get_uri(webView)) != get_uri_root(uri)) [[likely]]
        webkit_web_view_load_uri(webView, uri.c_str());
    else [[unlikely]]
        webkit_web_view_reload(webView);
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
    Gtk::Widget *webViewWidget {Gtk::manage(Glib::wrap(GTK_WIDGET(webView)))};
    webViewWidget->set_name("browser_webview");
    webViewWidget->set_vexpand(true);
    scroller->set_child(*webViewWidget);

    // Get the GtkBuilder-instantiated buttons, and connect a signal handler
    backButton = refBuilder->get_widget<Gtk::Button>("back_button");
    forwardButton = refBuilder->get_widget<Gtk::Button>("forward_button");
    homeButton = refBuilder->get_widget<Gtk::Button>("home_button");
    reloadButton = refBuilder->get_widget<Gtk::Button>("reload_button");
    uriEntry = refBuilder->get_widget<Gtk::Entry>("header_entry");
    enterButton = refBuilder->get_widget<Gtk::Button>("enter_button");
    menuButton = refBuilder->get_widget<Gtk::MenuButton>("header_menu");

    // Add Callbacks
    if(backButton) [[likely]]
        backButton->signal_clicked().connect
        (
            [this](){ webkit_web_view_go_back(webView);}
        );
    if(forwardButton) [[likely]]
        forwardButton->signal_clicked().connect
        (
            [this](){ webkit_web_view_go_forward(webView);}
        );
    if(homeButton) [[likely]]
        homeButton->signal_clicked().connect
        (
            [this](){ entry_uri_load(HOME_URL);}
    
        );
    if(reloadButton) [[likely]]
        reloadButton->signal_clicked().connect
        (
            [this]()
            {
                webkit_web_view_is_loading(webView) ?
                webkit_web_view_stop_loading(webView) :
                webkit_web_view_reload(webView);
            }
        );
    if(uriEntry) [[likely]]
    {
        uriEntry->signal_activate().connect([this](){ entry_uri_load();});
        if(enterButton) [[likely]]
            enterButton->signal_clicked().connect([this](){ entry_uri_load();});
    }
    // Menu button not visible as no usage needed for the moment
    if(menuButton) [[likely]] menuButton->set_visible(false);

    g_signal_connect(webView, "load-changed", G_CALLBACK(web_view_load_changed), this);
    webkit_web_view_load_uri(webView, HOME_URL);

    // Insert elements into Browser Box
    insert_child_at_start(*header);
    insert_child_after(*scroller, *header);
}

void Browser::web_view_load_changed(WebKitWebView *webView,
                                    const WebKitLoadEvent loadEvent,
                                    gpointer userData)
{
    auto uri {webkit_web_view_get_uri(webView)};
    auto browser {static_cast<Browser*>(userData)};
    switch (loadEvent) {
        case WEBKIT_LOAD_STARTED:
            browser->backButton->set_sensitive(webkit_web_view_can_go_back(webView));
            browser->forwardButton->set_sensitive(webkit_web_view_can_go_forward(webView));
            browser->reloadButton->set_icon_name("gtk-stop");
            browser->uriEntry->set_sensitive(false);
            browser->enterButton->set_sensitive(false);
            std::cout << "Load started with provisional URI: " << uri << std::endl;
            break;

        case WEBKIT_LOAD_REDIRECTED:
            std::cout << "Redirected to URI: " << uri << std::endl;
            break;

        case WEBKIT_LOAD_COMMITTED:
            std::cout << "URL changed/committed to: " << uri << std::endl;
            break;

        case WEBKIT_LOAD_FINISHED:
            browser->backButton->set_sensitive(webkit_web_view_can_go_back(webView));
            browser->forwardButton->set_sensitive(webkit_web_view_can_go_forward(webView));
            browser->reloadButton->set_icon_name("gtk-convert");
            browser->uriEntry->set_sensitive(true);
            browser->uriEntry->set_text(uri);
            browser->enterButton->set_sensitive(true);
            std::cout << "Load finished." << std::endl;
            break;
    }
}
