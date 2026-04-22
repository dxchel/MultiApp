#ifndef _FRACTAL_APP_
#define _FRACTAL_APP_

#include <gtkmm.h>
#include <cairo.h>
#include <thread>
#include <atomic>

#include <iostream>


int mandelbrot(double, double, int);

void iter_to_rgb(int, int, uint8_t &, uint8_t &, uint8_t &);


/**
 * @brief FractalArea class containing important functions for Fractal Drawing.
 *
 * Gtk::DrawingArea implementing class that contains important functions
 * for drawing fractals.
 */
class FractalArea : public Gtk::DrawingArea
{
    friend class FractalTest;

    double cx, cy, range, drag_start_cx, drag_start_cy;
    int max_iter;

public:
    /**
     * @brief Creates FractalArea object with all needed functions.
     *
     * Creates FractalArea object with needed fractal drawing functions
     */
    FractalArea();

    void on_draw(const Cairo::RefPtr<Cairo::Context>&, int, int);
    void set_max_iter(int);
    int  get_max_iter() const;

    void reset();
};

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
    FractalArea *fractalArea {};

    Gtk::Label *statusLabel {};

public:
    /**
     * @brief Creates FractalBox object with all needed Widgets and signals.
     *
     * Creates FractalBox object using res/gtk/fractal_app.ui file,
     * Checks for file issues, gets Widgets and connects needed signals.
     */
    FractalBox(Gtk::Label* parentLabel = nullptr);
};

#endif