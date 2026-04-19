#ifndef _FRACTAL_APP_
#define _FRACTAL_APP_

#include <gtkmm.h>


/**
 * @brief FractalBox class containing important Widgets and functions for Fractal functionality.
 *
 * Gtk::Box implementing class that contains important Gtk Widgets
 * for loading, displaying and saving fractals.
 */
class FractalBox : public Gtk::Box
{
    friend class FractalTest;

    Gtk::Box *header {};
    Gtk::Button *resetButton {};
    Gtk::Button *saveButton {};
    Gtk::DropDown *fractalDropDown {};
    Gtk::Scale *iterScale {};

    Gtk::Label *statusLabel {};

public:
    /**
     * @brief Creates FractalBox object with all needed Widgets and signals.
     *
     * Creates FractalBox object using res/gtk/fractal_app.ui file,
     * Checks for file issues, gets Widgets and connects needed signals.
     */
    FractalBox();

    /**
     * @brief Set fractal status label
     *
     * @param[in] label: Gtk::Label to use as status label.
     */
    void setStatusLabel(Gtk::Label *label);
};

#endif