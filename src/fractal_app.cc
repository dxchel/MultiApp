#include "include/fractal_app.hpp"

#include <gtkmm.h>

#include <iostream>


void FractalBox::setStatusLabel(Gtk::Label *label){
    statusLabel = label;
}


FractalBox::FractalBox() : Gtk::Box(Gtk::Orientation::VERTICAL)
{
    // Load the GtkBuilder file and instantiate its widgets, check for errors
    auto refBuilder {Gtk::Builder::create()};
    try
    {
        refBuilder->add_from_file("res/gtk/fractal_app.ui");
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

    // Get the GtkBuilder-instantiated buttons, and connect a signal handler
    resetButton = refBuilder->get_widget<Gtk::Button>("reset_button");
    saveButton = refBuilder->get_widget<Gtk::Button>("save_button");
    fractalDropDown = refBuilder->get_widget<Gtk::DropDown>("fractal_dropdown");
    iterScale = refBuilder->get_widget<Gtk::Scale>("iter_scale");

    // Add Callbacks
    if(resetButton) [[likely]]
        resetButton->signal_clicked().connect
        (
            [this](){ std::cout << "Reset button pressed!" << std::endl;}
        );
    if(saveButton) [[likely]]
        saveButton->signal_clicked().connect
        (
            [this](){ std::cout << "Save button pressed!" << std::endl;}
        );
    if(fractalDropDown) [[likely]]
        fractalDropDown->signal_realize().connect
        (
            [this](){ std::cout << "Fractal selection pressed!" << std::endl;}
        );
    if(iterScale) [[likely]]
        iterScale->signal_change_value().connect
        (
            [this](double new_iter){ std::cout << "Iteration changed to " << new_iter << "!" << std::endl;}
        );

    auto drawer {Gtk::manage(new FractalArea())};

    // Insert elements into Fractal Box
    insert_child_at_start(*header);
    insert_child_after(*drawer, *header);
}
