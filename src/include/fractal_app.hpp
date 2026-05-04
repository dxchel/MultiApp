#ifndef _FRACTAL_APP_
#define _FRACTAL_APP_

#include<any>

#include <gtkmm.h>
#include <cairo.h>


/**
 * @brief Mandelbrot Fractal algorithm
 *
 * Returns number of iterations until the result diverges too much from 0.
 */
int mandelbrot(double, double, int, const std::vector<std::any> = {});
/**
 * @brief Julia Fractal algorithm
 *
 * Returns number of iterations until the result diverges too much from 0.
 * It uses default constants but can be overriden.
 */
int julia(double, double, int, const std::vector<std::any> = {-0.5125, 0.5213});


/**
 * @brief FractalArea class containing important functions for Fractal Drawing.
 *
 * Gtk::DrawingArea implementing class that contains important functions
 * for drawing fractals.
 */
class FractalArea : public Gtk::DrawingArea
{
    friend class FractalTest;

    double cx, cy, range,
        drag_start_cx, drag_start_cy,
        const_r, const_i,
        r_lower_limit, r_upper_level,
        g_lower_level, g_upeer_level,
        b_lower_level, b_upper_level;
    int max_iter;

    Cairo::RefPtr<Cairo::ImageSurface> surface;

    std::string selection;
    std::unordered_map<std::string, std::function<int(double, double, int, std::vector<std::any>)>> fractal_map;

    /**
     * @brief Uses custom RGB intervals to draw spectrum
     */
    void iter_to_rgb(int, int, uint8_t &, uint8_t &, uint8_t &);


public:
    /**
     * @brief Creates FractalArea object with all needed functions.
     *
     * Creates FractalArea object with needed fractal drawing functions
     */
    FractalArea(const std::string = "Julia");

    void on_draw(const Cairo::RefPtr<Cairo::Context>&, int, int);

    /**
     * @brief Sets the selection string.
     *
     * Sets the algorithm selection string for use with the function map.
     */
    void set_selection(const std::string);
    /**
     * @brief Sets the max_iter variable for fractal rsolution.
     */
    void set_max_iter(int);
    /**
     * @brief Sets ranges for the blue channel in the iter_to_rgb function.
     */
    void set_r(double, double);
    /**
     * @brief Sets ranges for the blue channel in the iter_to_rgb function.
     */
    void set_g(double, double);
    /**
     * @brief Sets ranges for the blue channel in the iter_to_rgb function.
     */
    void set_b(double, double);
    /**
     * @brief Sets the Julia algorithm constants.
     *
     * Sets the constants to be used in the Julia Fractal algorithm.
     */
    void set_consts(double, double);

    /**
     * @brief Resets fractal to original position.
     *
     * Resets Drawing area to it's original positions.
     */
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

    Gtk::Box *menu, *consts_box;
    Gtk::Button *reset_button,
        *save_button;
    Gtk::DropDown *fractal_dropdown;
    FractalArea *fractal_area;
    Gtk::Label *status_label;
    Gtk::Scale *iter_scale,
        *const_r_scale, *const_i_scale,
        *r_min_scale, *r_max_scale,
        *g_min_scale, *g_max_scale,
        *b_min_scale, *b_max_scale;

    std::string selection;

    /**
     * @brief Get Main Application status label.
     *
     * Gets Main Application status label after realize due to
     * the box needing to be contained.
     */
    void on_realize() override;

public:
    /**
     * @brief Creates FractalBox object with all needed Widgets and signals.
     *
     * Creates FractalBox object using res/gtk/fractal_app.ui file,
     * Checks for file issues, gets Widgets and connects needed signals.
     */
    FractalBox();
};

#endif