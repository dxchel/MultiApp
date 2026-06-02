#include "include/fractal_app.hpp"

#include <iostream>
#include <any>
#include <vector>
#include <thread>

#include <gtkmm.h>
#include <cairo.h>


int mandelbrot(double cr, double ci, int max_iter, const std::vector<std::any>) {
    double zr {}, zi {};
    for (int i{}; i < max_iter; ++i) {
        double zr2 {zr*zr - zi*zi + cr},
            zi2 {2.0*zr*zi + ci};
        zr = zr2; zi = zi2;
        if (zr*zr + zi*zi > 7.0) return i;
    }
    return max_iter;
}

int julia(double cr, double ci, int max_iter, const std::vector<std::any> consts) {
    if (consts.size() != 2) {
        std::cerr << "Julia fractal requires two constants, using defaults." << std::endl;
        return julia(cr, ci, max_iter, {-0.5125, 0.5213});
    }
    double zr {cr}, zi {ci};
    for (int i{}; i < max_iter; ++i) {
        double zr2 {zr*zr - zi*zi + std::any_cast<double>(consts[0])},
            zi2 {2.0*zr*zi + std::any_cast<double>(consts[1])};
        zr = zr2; zi = zi2;
        if (zr*zr + zi*zi > 7.0) return i;
    }
    return max_iter;
}

void FractalArea::set_selection(const std::string new_selection)
    { selection = new_selection; queue_draw(); }

void FractalArea::set_max_iter(int iter)
    { max_iter = iter; queue_draw(); }

void FractalArea::set_consts(double cr, double ci)
    { const_r = cr; const_i = ci; queue_draw(); }

void FractalArea::set_r(double min, double max)
    { r_lower_level = min/100; r_upper_level = max/100; queue_draw(); }

void FractalArea::set_g(double min, double max)
    { g_lower_level = min/100; g_upper_level = max/100; queue_draw(); }

void FractalArea::set_b(double min, double max)
    { b_lower_level = min/100; b_upper_level = max/100; queue_draw(); }

void FractalArea::reset()
    { cx = -0.5; cy = 0.0; range = 3.5; queue_draw(); }

void FractalArea::iter_to_rgb(int iter, int max_iter, uint8_t &r, uint8_t &g, uint8_t &b) {
    if (iter == max_iter) { r = g = b = 0; return; }
    double level {static_cast<double>(iter) / max_iter},
        r_mid {(r_upper_level-r_lower_level)/2},
        g_mid {(g_upper_level-g_lower_level)/2},
        b_mid {(b_upper_level-b_lower_level)/2};
    r = static_cast<uint8_t>(level > r_lower_level && level < r_upper_level ?
        255-(level-r_lower_level-r_mid)*(level-r_lower_level-r_mid)/r_mid/r_mid*255 : 0 );
    g = static_cast<uint8_t>(level > g_lower_level && level < g_upper_level ?
        255-(level-g_lower_level-g_mid)*(level-g_lower_level-g_mid)/g_mid/g_mid*255 : 0 );
    b = static_cast<uint8_t>(level > b_lower_level && level < b_upper_level ?
        255-(level-b_lower_level-b_mid)*(level-b_lower_level-b_mid)/b_mid/b_mid*255 : 0 );
}


FractalArea::FractalArea(const std::string selection) :
    Gtk::DrawingArea(),
    selection       (selection),
    cx              (-0.5),
    cy              (0.0),
    range           (3.5),
    drag_start_cx   (),
    drag_start_cy   (),
    surface         (),
    fractal_map     ({ { "Julia", julia }, { "Mandelbrot", mandelbrot } }) {
    set_expand(true);
    set_draw_func(sigc::mem_fun(*this, &FractalArea::on_draw));

    // Mouse/touch controls
    auto drag {Gtk::GestureDrag::create()};
    drag->signal_drag_begin().connect([this](double, double){
        drag_start_cx = cx; drag_start_cy = cy;
    });
    drag->signal_drag_update().connect([this](double dx, double dy){
        double px_per_unit {get_width() / range};
        cx = drag_start_cx - dx/px_per_unit;
        cy = drag_start_cy + dy/px_per_unit;
        queue_draw();
    });
    add_controller(drag);

    auto scroll {Gtk::EventControllerScroll::create()};
    scroll->set_flags(Gtk::EventControllerScroll::Flags::VERTICAL);
    scroll->signal_scroll().connect([this](double, double dy) -> bool {
        range *= (dy < 0) ? 0.8 : 1.25;
        queue_draw();
        return true;
    }, false);
    add_controller(scroll);
};

void FractalArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int w, int h) {
    const int NTHREADS {static_cast<int>(std::thread::hardware_concurrency())};
    std::vector<std::thread> threads;

    // Get Cairo Surface
    if (!surface || surface->get_width() != w || surface->get_height() != h)
        surface = Cairo::ImageSurface::create(Cairo::Surface::Format::RGB24, w, h);

    uint8_t* surface_data {surface->get_data()};

    auto render_band {[&](int y0, int y1) {
        int current_pixel {4*w*y0};
        for (int y{y0}; y < y1; ++y) {
            for (int x{0}; x < w; ++x, current_pixel += 4) {
                double cr {cx + (x - w/2.0)*range/w},
                    ci {cy - (y - h/2.0)*range/w};
                int iter {fractal_map.at(selection)(cr, ci, max_iter, {const_r, const_i})};
                iter_to_rgb(iter, max_iter, surface_data[current_pixel + 2], surface_data[current_pixel + 1], surface_data[current_pixel]);
            }
        }
    }};

    int band { h/NTHREADS };
    for (int t{}; t < NTHREADS; ++t) {
        int y0 {t*band},
            y1 {(t == NTHREADS-1) ? h : y0 + band};
        threads.emplace_back(render_band, y0, y1);
    }

    for (auto &th : threads) th.join();

    surface->flush();
    surface->mark_dirty();
    cr->set_source(surface, 0, 0);
    cr->paint();

    // HUD overlay
    cr->set_source_rgba(0, 0, 0, 0.55);
    cr->rectangle(8, 8, 260, 52);
    cr->fill();

    cr->set_source_rgb(1, 1, 1);
    cr->select_font_face("Monospace", Cairo::ToyFontFace::Slant::NORMAL,
                            Cairo::ToyFontFace::Weight::NORMAL);
    cr->set_font_size(12);
    char buf[128];
    snprintf(buf, sizeof(buf), "Center: (%.6f, %.6f)", cx, cy);
    cr->move_to(14, 26); cr->show_text(buf);
    snprintf(buf, sizeof(buf), "Range: %.2e   Iter: %d", range, max_iter);
    cr->move_to(14, 46); cr->show_text(buf);
}


FractalBox::FractalBox() : Gtk::Box(Gtk::Orientation::HORIZONTAL) {
    // Load the GtkBuilder file and instantiate its widgets, check for errors
    auto ref_builder {Gtk::Builder::create()};
    try {
        ref_builder->add_from_file("res/gtk/fractal_app.ui");
    } catch(const Glib::FileError& ex) {
        std::cerr << "FileError: " << ex.what() << std::endl;
        throw ex;
    } catch(const Glib::MarkupError& ex) {
        std::cerr << "MarkupError: " << ex.what() << std::endl;
        throw ex;
    } catch(const Gtk::BuilderError& ex) {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        throw ex;
    }

    // Get the GtkBuilder-instantiated buttons, and connect a signal handler
    menu             = Gtk::manage(ref_builder->get_widget<Gtk::Box>("menu_bar"));
    reset_button     = ref_builder->get_widget<Gtk::Button>("reset_button");
    save_button      = ref_builder->get_widget<Gtk::Button>("save_button");
    fractal_dropdown = ref_builder->get_widget<Gtk::DropDown>("fractal_dropdown");
    consts_box       = ref_builder->get_widget<Gtk::Box>("consts_box");

    fractal_area     = Gtk::manage(new FractalArea());

    // Add Callbacks
    if(reset_button) [[likely]]
        reset_button->signal_clicked().connect
        (
            [this](){ fractal_area->reset(); }
        );
    if(save_button) [[likely]] {
        save_button->signal_clicked().connect
        (
            [](){ std::cout << "Save button pressed!" << std::endl;}
        );
        save_button->set_visible(false);  //Set visible false until new way of creating a dialog is available
    }
    if(fractal_dropdown) [[likely]]
        fractal_dropdown->property_selected().signal_changed().connect
        (
            [this](){
                selection = std::dynamic_pointer_cast<Gtk::StringObject>(fractal_dropdown->get_selected_item())->get_string();
                fractal_area->set_selection(selection);
                if (selection != "Julia")
                    consts_box->hide();
                else
                    consts_box->show();

                if (status_label)
                    status_label->set_text("Welcome to the Fractal creator! You're using the " + selection + " algorithm");
            }
        );

    // Iterations scale and color scales
    iter_scale = ref_builder->get_widget<Gtk::Scale>("iter_scale");
    if(iter_scale) [[likely]] {
        iter_scale->signal_value_changed().connect([this]{
            fractal_area->set_max_iter(static_cast<int>(iter_scale->get_value()));
        });
        fractal_area->set_max_iter(static_cast<int>(iter_scale->get_value()));
    }

    r_min_scale = ref_builder->get_widget<Gtk::Scale>("r_scale_min");
    r_max_scale = ref_builder->get_widget<Gtk::Scale>("r_scale_max");
    if(r_min_scale && r_max_scale) [[likely]] {
        auto r_change {[this] {
                fractal_area->set_r(r_min_scale->get_value(), r_max_scale->get_value());
            }};
        r_min_scale->signal_value_changed().connect(r_change);
        r_max_scale->signal_value_changed().connect(r_change);
        fractal_area->set_r(r_min_scale->get_value(), r_max_scale->get_value());
    }

    g_min_scale = ref_builder->get_widget<Gtk::Scale>("g_scale_min");
    g_max_scale = ref_builder->get_widget<Gtk::Scale>("g_scale_max");
    if(g_min_scale && g_max_scale) [[likely]] {
        auto g_change {[this] {
                fractal_area->set_g(g_min_scale->get_value(), g_max_scale->get_value());
            }};
        g_min_scale->signal_value_changed().connect(g_change);
        g_max_scale->signal_value_changed().connect(g_change);
        fractal_area->set_g(g_min_scale->get_value(), g_max_scale->get_value());
    }

    b_min_scale = ref_builder->get_widget<Gtk::Scale>("b_scale_min");
    b_max_scale = ref_builder->get_widget<Gtk::Scale>("b_scale_max");
    if(b_min_scale && b_max_scale) [[likely]] {
        auto b_change {[this] {
                fractal_area->set_b(b_min_scale->get_value(), b_max_scale->get_value());
            }};
        b_min_scale->signal_value_changed().connect(b_change);
        b_max_scale->signal_value_changed().connect(b_change);
        fractal_area->set_b(b_min_scale->get_value(), b_max_scale->get_value());
    }

    const_r_scale = ref_builder->get_widget<Gtk::Scale>("const_r_scale");
    const_i_scale = ref_builder->get_widget<Gtk::Scale>("const_i_scale");
    if(const_r_scale && const_i_scale) [[likely]] {
        auto consts_change {[this] {
                fractal_area->set_consts(const_r_scale->get_value(), const_i_scale->get_value());
            }};
        const_r_scale->signal_value_changed().connect(consts_change);
        const_i_scale->signal_value_changed().connect(consts_change);
        fractal_area->set_consts(const_r_scale->get_value(), const_i_scale->get_value());
    }


    // Insert elements into Fractal Box
    insert_child_at_start(*menu);
    insert_child_after(*fractal_area, *menu);
}


void FractalBox::on_realize() {
    Gtk::Box::on_realize();
    selection = std::dynamic_pointer_cast<Gtk::StringObject>(fractal_dropdown->get_selected_item())->get_string();
    status_label = dynamic_cast<Gtk::Label *>(get_parent()->get_parent()->get_parent()->get_parent()->get_last_child());
    if (!status_label)
        std::cout << "Status label not found" << std::endl;
    if (status_label)
        status_label->set_text("Welcome to the Fractal creator! You're using the " + selection + " algorithm");
}
