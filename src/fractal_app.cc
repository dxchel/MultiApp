#include "include/fractal_app.hpp"

#include <gtkmm.h>
#include <cairo.h>
#include <thread>
#include <atomic>

#include <iostream>


int mandelbrot(double cr, double ci, int max_iter) {
    double zr = 0, zi = 0;
    for (int i = 0; i < max_iter; ++i) {
        double zr2 = zr * zr - zi * zi + cr;
        double zi2 = 2.0 * zr * zi + ci;
        zr = zr2; zi = zi2;
        if (zr * zr + zi * zi > 4.0) return i;
    }
    return max_iter;
}

void iter_to_rgb(int iter, int max_iter, uint8_t &r, uint8_t &g, uint8_t &b) {
    if (iter == max_iter) { r = g = b = 0; return; }
    double t = static_cast<double>(iter) / max_iter;
    double r1 = 17*(1-t)*t*t*t, g1 = 7*(1-t)*(1-t)*t*t, b1 = 13*(1-t)*(1-t)*(1-t)*t;
    r = (uint8_t)std::min(255.0, r1 * 255);
    g = (uint8_t)std::min(255.0, g1 * 255);
    b = (uint8_t)std::min(255.0, b1 * 255);
}


FractalArea::FractalArea() : Gtk::DrawingArea(),
    cx_{-0.5}, cy_{0}, range_{3.5},
    drag_start_x_{}, drag_start_y_{},
    drag_start_cx_{}, drag_start_cy_{},
    max_iter_{64}
{
    set_expand(true);
    set_draw_func(sigc::mem_fun(*this, &FractalArea::on_draw));

    // Mouse/touch controls
    auto drag = Gtk::GestureDrag::create();
    drag->signal_drag_begin().connect([this](double x, double y){
        drag_start_cx_ = cx_; drag_start_cy_ = cy_;
        drag_start_x_ = x;   drag_start_y_ = y;
        std::cout << "Dragged from " << drag_start_x_ << "," << drag_start_y_ << std::endl;
    });
    drag->signal_drag_update().connect([this](double dx, double dy){
        double px_per_unit = get_width() / range_;
        cx_ = drag_start_cx_ - dx / px_per_unit;
        cy_ = drag_start_cy_ + dy / px_per_unit;
        std::cout << "Dragged by " << cx_ << "," << cy_ << std::endl;
        queue_draw();
    });
    add_controller(drag);

    auto scroll = Gtk::EventControllerScroll::create();
    scroll->set_flags(Gtk::EventControllerScroll::Flags::VERTICAL);
    scroll->signal_scroll().connect([this](double, double dy) -> bool {
        range_ *= (dy < 0) ? 0.8 : 1.25;
        queue_draw();
        return true;
    }, false);
    add_controller(scroll);
};

void FractalArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int w, int h)
{
    // Render into a raw pixel buffer (multithreaded by row bands)
    std::vector<uint8_t> pixels(w * h * 3);
    const int nthreads = (int)std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    auto render_band = [&](int y0, int y1) {
        for (int y = y0; y < y1; ++y) {
            for (int x = 0; x < w; ++x) {
                double cr = cx_ + (x - w/2.0) * range_ / w;
                double ci = cy_ - (y - h/2.0) * range_ / w;
                int iter = mandelbrot(cr, ci, max_iter_);
                uint8_t r, g, b;
                iter_to_rgb(iter, max_iter_, r, g, b);
                int idx = (y * w + x) * 3;
                pixels[idx]   = r;
                pixels[idx+1] = g;
                pixels[idx+2] = b;
            }
        }
    };

    int band = h / nthreads;
    for (int t = 0; t < nthreads; ++t) {
        int y0 = t * band;
        int y1 = (t == nthreads-1) ? h : y0 + band;
        threads.emplace_back(render_band, y0, y1);
    }
    for (auto &th : threads) th.join();

    // Blit to Cairo surface
    auto surface = Cairo::ImageSurface::create(Cairo::Surface::Format::RGB24, w, h);
    uint8_t* dst = surface->get_data();
    int stride = surface->get_stride();
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int si = (y * w + x) * 3;
            int di = y * stride + x * 4;
            dst[di+2] = pixels[si];   // R → B (Cairo ARGB32 is BGRA in memory)
            dst[di+1] = pixels[si+1]; // G
            dst[di+0] = pixels[si+2]; // B → R
            dst[di+3] = pixels[si];
        }
    }
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
    snprintf(buf, sizeof(buf), "Center: (%.6f, %.6f)", cx_, cy_);
    cr->move_to(14, 26); cr->show_text(buf);
    snprintf(buf, sizeof(buf), "Range: %.2e   Iter: %d", range_, max_iter_);
    cr->move_to(14, 46); cr->show_text(buf);
}

void FractalArea::set_max_iter(int v)
{
    max_iter_ = v;
    queue_draw();
}

int FractalArea::get_max_iter() const { return max_iter_; }

void FractalArea::reset() {
    cx_ = -0.5; cy_ = 0.0; range_ = 3.5;
    queue_draw();
}


FractalBox::FractalBox(Gtk::Label *parentLabel) : Gtk::Box(Gtk::Orientation::VERTICAL)
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
    fractalArea = Gtk::manage(new FractalArea());

    gtk_drop_down_set_model(GTK_DROP_DOWN(fractalDropDown), G_LIST_MODEL(FractalBox::fractals));

    // Add Callbacks
    if(resetButton) [[likely]]
        resetButton->signal_clicked().connect
        (
            [this](){ std::cout << "Reset button pressed!" << std::endl; fractalArea->reset(); }
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
    {
        iterScale->set_range(32, 1024);
        iterScale->set_value(64);
        iterScale->set_size_request(180, -1);
        iterScale->set_draw_value(true);
        iterScale->signal_value_changed().connect([this]{
            fractalArea->set_max_iter((int)iterScale->get_value());
        });
    }

    // Insert elements into Fractal Box
    insert_child_at_start(*header);
    insert_child_after(*fractalArea, *header);
    statusLabel = parentLabel;
}
