#include "include/main_application.hpp"
#include "include/browser_app.hpp"

#include <iostream>


MainApplication::MainApplication() :
    Gtk::Application("org.xchel.multiapp", Gio::Application::Flags::HANDLES_OPEN),
    mainWindow {} {}

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
    auto mainBox {Gtk::manage(new Gtk::Box())};
    mainBox->set_orientation(Gtk::Orientation::VERTICAL);
    mainWindow->set_child(*mainBox);

    // Main notebook to switch apps
    auto mainNotebook {Gtk::manage(new Gtk::Notebook())};
    mainNotebook->set_vexpand(true);

    // Browser app for first Notebook App
    auto browserBox {Gtk::manage(new Browser())};
    mainNotebook->append_page(*browserBox);
    // Browser tab title
    auto labelBrowser {Gtk::manage(new Gtk::Label())};
    labelBrowser->set_text("Browser");
    mainNotebook->set_tab_label(*browserBox, *labelBrowser);
    
    // Fractal app for second Notebook App
    auto fractal_placeholder {Gtk::manage(new Gtk::Label())};
    fractal_placeholder->set_text("Temp Fractal Placeholder");
    fractal_placeholder->set_justify(Gtk::Justification::CENTER);
    fractal_placeholder->set_hexpand(true);
    mainNotebook->append_page(*fractal_placeholder);
    // Fractal tab title
    auto labelFractal {Gtk::manage(new Gtk::Label())};
    labelFractal->set_text("Fractal");
    mainNotebook->set_tab_label(*fractal_placeholder, *labelFractal);
    mainNotebook->set_current_page(0);
    mainBox->insert_child_at_start(*mainNotebook);

    //General status Label
    auto labelFoot {Gtk::manage(new Gtk::Label())};
    labelFoot->set_text("Welcome to MultiApp!");
    labelFoot->set_justify(Gtk::Justification::CENTER);
    labelFoot->set_hexpand(true);
    mainBox->insert_child_after(*labelFoot, *mainNotebook);

    add_window(*mainWindow);
    return mainWindow;
}