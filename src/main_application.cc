#include "../include/main_application.hpp"
#include "../include/browser_app.hpp"


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
    // Main Window where everything is placed
    mainWindow = Gtk::manage(new Gtk::ApplicationWindow());
    mainWindow->set_default_size(800, 600);
    mainWindow->maximize();
    mainWindow->set_title("GUI MultiApp");
    mainWindow->set_default_icon_name("applications-engineering");

    // Main box containing Notebook and General status label
    auto mainBox = Gtk::manage(new Gtk::Box());
    mainBox->set_orientation(Gtk::Orientation::VERTICAL);
    mainWindow->set_child(*mainBox);

    // Main notebook to switch apps
    auto mainNotebook = Gtk::manage(new Gtk::Notebook());
    mainNotebook->set_vexpand(true);

    // Browser app for first Notebook App
    auto browserBox = Gtk::manage(new Browser());
    mainNotebook->append_page(*browserBox);
    // Browser tab title
    auto label_browser = Gtk::manage(new Gtk::Label());
    label_browser->set_text("Browser");
    mainNotebook->set_tab_label(*browserBox, *label_browser);
    
    // Fractal app for second Notebook App
    auto label2 = Gtk::manage(new Gtk::Label());
    label2->set_text("Temp Fractal Placeholder");
    label2->set_justify(Gtk::Justification::CENTER);
    label2->set_hexpand(true);
    mainNotebook->append_page(*label2);
    // Fractal tab title
    auto label_fractal = Gtk::manage(new Gtk::Label());
    label_fractal->set_text("Fractal");
    mainNotebook->set_tab_label(*label2, *label_fractal);
    mainNotebook->set_current_page(0);
    mainBox->insert_child_at_start(*mainNotebook);

    //General status Label
    auto footLabel = Gtk::manage(new Gtk::Label());
    footLabel->set_text("Welcome to MultiApp!");
    footLabel->set_justify(Gtk::Justification::CENTER);
    footLabel->set_hexpand(true);
    mainBox->insert_child_after(*footLabel, *mainNotebook);

    add_window(*mainWindow);
    return mainWindow;
}