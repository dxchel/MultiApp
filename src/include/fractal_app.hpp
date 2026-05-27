#pragma once

#include<any>

#include <gtkmm.h>
#include <cairo.h>


/**
 * @brief Mandelbrot Fractal algorithm
 *
 * Returns number of iterations until the result diverges too much from 0.
 *
 * @param[in] cr: Real part of the point to analyze in the complex plane.
 * @param[in] ci: Imaginary part of the point to analyze in the complex plane.
 * @param[in] max_iter: Maximum iterations available for fractal resolution.
 * @param[in] vector: Vector needed for the declaration to be consistent for fractal function mapping.
 *
 * @return Number of iterations until the value diverges.
 */
int mandelbrot(double cr, double ci, int max_iter, const std::vector<std::any> = {});
/**
 * @brief Julia Fractal algorithm
 *
 * Returns number of iterations until the result diverges too much from 0.
 * It uses default constants but can be overriden.
 *
 * @param[in] cr: Real part of the point to analyze in the complex plane.
 * @param[in] ci: Imaginary part of the point to analyze in the complex plane.
 * @param[in] max_iter: Maximum iterations available for fractal resolution.
 * @param[in] vector: Vector containing the two constants needed in the Julia fractal algorithm.
 *
 * @return Number of iterations until the value diverges.
 */
int julia(double, double, int, const std::vector<std::any> = {-0.5125, 0.5213});


/**
 * @brief FractalArea class containing important functions for Fractal Drawing.
 *
 * Gtk::DrawingArea implementing class that contains important functions
 * for drawing fractals.
 */
class FractalArea : public Gtk::DrawingArea {
    friend class FractalTest;
    friend class FractalTest_FractalFunctionalTest_Test;

    double cx{}, cy{}, range{},
        drag_start_cx{}, drag_start_cy{},
        const_r{}, const_i{},
        r_lower_level{}, r_upper_level{},
        g_lower_level{}, g_upper_level{},
        b_lower_level{}, b_upper_level{};
    int max_iter{};

    Cairo::RefPtr<Cairo::ImageSurface> surface;

    std::string selection;
    std::unordered_map<std::string, std::function<int(double, double, int, std::vector<std::any>)>> fractal_map;

    /**
     * @brief Uses custom RGB intervals to draw spectrum
     *
     * @param[in] iter: Iteration to compute the RGB spectrum with.
     * @param[in] max_iter: Maximum iterations available for fractal resolution.
     * @param[in] r: reference to the R data to write on.
     * @param[in] g: reference to the G data to write on.
     * @param[in] b: reference to the B data to write on.
     */
    void iter_to_rgb(int iter, int max_iter, uint8_t &r, uint8_t &g, uint8_t &b);


public:
    /**
     * @brief Creates FractalArea object with all needed functions.
     *
     * Creates FractalArea object with needed fractal drawing functions
     *
     * @param[in] selection: Fractal algorithm selection for function mapping with Julia as default.
     */
    FractalArea(const std::string selection = "Julia");

    void on_draw(const Cairo::RefPtr<Cairo::Context>&, int, int);

    /**
     * @brief Sets the selection string.
     *
     * Sets the algorithm selection string for use with the function map.
     *
     * @param[in] new_selection: New fractal algorithm selection to use next.
     */
    void set_selection(const std::string new_selection);
    /**
     * @brief Sets the max_iter variable for fractal rsolution.
     *
     * @param[in] iter: New max_iter to use in the fractal algorithms.
     */
    void set_max_iter(int iter);
    /**
     * @brief Sets ranges for the blue channel in the iter_to_rgb function.
     *
     * @param[in] min: New lower limit on the R spectrum to use.
     * @param[in] max: New upper limit on the R spectrum to use.
     */
    void set_r(double min, double max);
    /**
     * @brief Sets ranges for the blue channel in the iter_to_rgb function.
     *
     * @param[in] min: New lower limit on the G spectrum to use.
     * @param[in] max: New upper limit on the G spectrum to use.
     */
    void set_g(double min, double max);
    /**
     * @brief Sets ranges for the blue channel in the iter_to_rgb function.
     *
     * @param[in] min: New lower limit on the B spectrum to use.
     * @param[in] max: New upper limit on the B spectrum to use.
     */
    void set_b(double min, double max);
    /**
     * @brief Sets the Julia algorithm constants.
     *
     * Sets the constants to be used in the Julia Fractal algorithm.
     *
     * @param[in] cr: New Real part of the constant to use in the fractal algorithm.
     * @param[in] ci: New Imaginary part of the constant to use in the fractal algorithm.
     */
    void set_consts(double cr, double ci);

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
class FractalBox : public Gtk::Box {
    friend class FractalTest;
    friend class FractalTest_FractalFunctionalTest_Test;

    Gtk::Box *menu{}, *consts_box{};
    Gtk::Button *reset_button{}, *save_button{};
    Gtk::DropDown *fractal_dropdown{};
    FractalArea *fractal_area{};
    Gtk::Label *status_label{};
    Gtk::Scale *iter_scale{},
        *const_r_scale{}, *const_i_scale{},
        *r_min_scale{}, *r_max_scale{},
        *g_min_scale{}, *g_max_scale{},
        *b_min_scale{}, *b_max_scale{};

    std::string selection{};

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
