#include "../include/main_application.hpp"


MainApplication::MainApplication() :
    Gtk::Application("org.xchel.multiapp", Gio::Application::Flags::HANDLES_OPEN) {}

Glib::RefPtr<MainApplication> MainApplication::create()
{
    return Glib::make_refptr_for_instance<MainApplication>(new MainApplication());
}

void MainApplication::on_activate()
{
    // The application has been started, so let's show a window.
    create_window()->present();
}

Gtk::Window* MainApplication::create_window()
{
    // Load the GtkBuilder file and instantiate its widgets:
    auto refBuilder{Gtk::Builder::create()};
    try
    {
        refBuilder->add_from_file("res/multi.ui");
    }
    catch(const Glib::FileError& ex)
    {
        std::cerr << "FileError: " << ex.what() << std::endl;
        return nullptr;
    }
    catch(const Glib::MarkupError& ex)
    {
        std::cerr << "MarkupError: " << ex.what() << std::endl;
        return nullptr;
    }
    catch(const Gtk::BuilderError& ex)
    {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        return nullptr;
    }

    // Get the GtkBuilder-instantiated dialog:
    mainWindow = refBuilder->get_widget<Gtk::Window>("main_app_window");
    if(!mainWindow)
    {
        std::cerr << "Could not get the main window." << std::endl;
        return nullptr;
    }
    // A window can be added to an application with Gtk::Application::add_window()
    // or Gtk::Window::set_application(). When all added windows have been hidden
    // or removed, the application stops running (Gtk::Application::run() returns()),
    // unless Gio::Application::hold() has been called.
    mainWindow->signal_hide().connect([this] () { delete mainWindow; });


    // Get the GtkBuilder-instantiated button, and connect a signal handler:
    auto homeButton{refBuilder->get_widget<Gtk::Button>("home_button")};
    if(homeButton)
        homeButton->signal_clicked().connect([this] () { delete mainWindow; });

    // Get the GtkBuilder-instantiated browser_scroller, and connect WebKitWebView
    auto browserScroller{refBuilder->get_widget<Gtk::ScrolledWindow>("browser_scroller")};
    if(browserScroller)
    {
        WebKitWebView *webView{WEBKIT_WEB_VIEW(webkit_web_view_new())};
        webkit_web_view_load_uri(webView, "https://www.google.com/");
        Gtk::Widget *webViewWidget{Glib::wrap(GTK_WIDGET(webView))};
        webViewWidget->set_name("browser_webview");
        webViewWidget->set_vexpand(true);
        browserScroller->set_child(*webViewWidget);
    }

    add_window(*mainWindow);
    return mainWindow;
}