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
    header = refBuilder->get_widget<Gtk::Box>("header_bar");
    if(!header)
    {
        std::cerr << "Could not get Header component!" << std::endl;
        throw std::logic_error("Components not found");
    }
    // A window can be added to an application with Gtk::Application::add_window()
    // or Gtk::Window::set_application(). When all added windows have been hidden
    // or removed, the application stops running (Gtk::Application::run() returns()),
    // unless Gio::Application::hold() has been called.
    header->signal_destroy().connect([this] () { delete header; });


    // Get the GtkBuilder-instantiated button, and connect a signal handler:
    auto homeButton{refBuilder->get_widget<Gtk::Button>("home_button")};
    if(homeButton)
        homeButton->signal_clicked().connect([this] () { std::cout << "Home button!"; });

    // Get the GtkBuilder-instantiated browser_scroller, and connect WebKitWebView
    scroller = Gtk::manage(new Gtk::ScrolledWindow());
    if(scroller)
    {
        webView = WEBKIT_WEB_VIEW(webkit_web_view_new());
        webkit_web_view_load_uri(webView, "https://www.google.com/");
        Gtk::Widget *webViewWidget{Gtk::manage(Glib::wrap(GTK_WIDGET(webView)))};
        webViewWidget->set_name("browser_webview");
        webViewWidget->set_vexpand(true);
        scroller->set_child(*webViewWidget);
    }

    insert_child_at_start(*header);
    insert_child_after(*scroller, *header);
}