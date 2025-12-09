#include "../include/main_window.hpp"

namespace
{

void on_button_clicked(){
    if(mainDialog)
        mainDialog->set_visible(false);
}

void on_app_activate(){
    // Load the GtkBuilder file and instantiate its widgets:
    auto refBuilder = Gtk::Builder::create();
    try{
        refBuilder->add_from_file("res/multi.ui");
    }
    catch(const Glib::FileError& ex){
        std::cerr << "FileError: " << ex.what() << std::endl;
        return;
    }
    catch(const Glib::MarkupError& ex){
        std::cerr << "MarkupError: " << ex.what() << std::endl;
        return;
    }
    catch(const Gtk::BuilderError& ex){
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        return;
    }

    // Get the GtkBuilder-instantiated dialog:
    mainDialog = refBuilder->get_widget<Gtk::Window>("main_app_window");
    if(!mainDialog)
    {
        std::cerr << "Could not get the main window." << std::endl;
        return;
    }

    // Get the GtkBuilder-instantiated button, and connect a signal handler:
    auto homeButton = refBuilder->get_widget<Gtk::Button>("home_button");
    if(homeButton)
        homeButton->signal_clicked().connect([] () { on_button_clicked(); });

    // It's not possible to delete widgets after app->run() has returned.
    // Delete the dialog with its child widgets before app->run() returns.
    mainDialog->signal_hide().connect([] () { delete mainDialog; });

    // Get the GtkBuilder-instantiated browser_scroller, and connect WebKitWebView
    // This is a WA as GtkBuilder doesn't currently support WebKitWebView
    auto browserScroller = refBuilder->get_widget<Gtk::ScrolledWindow>("browser_scroller");
    if(browserScroller){
        WebKitWebView *web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
        webkit_web_view_load_uri(web_view, "https://www.google.com/");
        // Gtk::Widget * web_view_widget {Gtk::Widget(web_view)};
        // browserScroller->set_child(web_view);
    }

    app->add_window(*mainDialog);
    mainDialog->set_visible(true);
}

}

int main(int argc, char** argv){
    app = Gtk::Application::create("org.Xchel.MultiApp");

    // Instantiate a dialog when the application has been activated.
    // This can only be done after the application has been registered.
    // It's possible to call app->register_application() explicitly, but
    // usually it's easier to let app->run() do it for you.
    app->signal_activate().connect([] () { on_app_activate(); });

    return app->run(argc, argv);
}