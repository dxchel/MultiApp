#include "../include/tests.hpp"

#include <random>

#include <gtest/gtest.h>
#include <gmock/gmock.h>


FractalTest::FractalTest() :
    fractal          (Gtk::manage(new Fractal())),
    menu             (fractal->menu),
    consts_box       (fractal->consts_box),
    reset_button     (fractal->reset_button),
    save_button      (fractal->save_button),
    fractal_dropdown (fractal->fractal_dropdown),
    fractal_area     (fractal->fractal_area),
    status_label     (fractal->status_label),
    iter_scale       (fractal->iter_scale),
    const_r_scale    (fractal->const_r_scale),
    const_i_scale    (fractal->const_i_scale),
    r_min_scale      (fractal->r_min_scale),
    r_max_scale      (fractal->r_max_scale),
    g_min_scale      (fractal->g_min_scale),
    g_max_scale      (fractal->g_max_scale),
    b_min_scale      (fractal->b_min_scale),
    b_max_scale      (fractal->b_max_scale),
    selection        (fractal->selection) {};

TEST_F(FractalTest, FractalStructuralTest) {
    ASSERT_THAT(fractal, ::testing::NotNull());
    auto menu_t {dynamic_cast<Gtk::Box *>(fractal->get_first_child())};
    ASSERT_THAT(menu_t, ::testing::NotNull());
    ASSERT_THAT(menu_t, ::testing::Eq(menu));
    auto fractal_area_t {dynamic_cast<FractalArea *>(menu_t->get_next_sibling())};
    ASSERT_THAT(fractal_area_t, ::testing::NotNull());
    ASSERT_THAT(fractal_area_t, ::testing::Eq(fractal_area));
    auto level {dynamic_cast<Gtk::Box *>(menu->get_first_child())};
    auto reset_t {dynamic_cast<Gtk::Button *>(level->get_first_child())};
    ASSERT_THAT(reset_t, ::testing::NotNull());
    ASSERT_THAT(reset_t, ::testing::Eq(reset_button));
    auto save_t {dynamic_cast<Gtk::Button *>(reset_button->get_next_sibling())};
    ASSERT_THAT(save_t, ::testing::NotNull());
    ASSERT_THAT(save_t, ::testing::Eq(save_button));
    auto dropdown_t {dynamic_cast<Gtk::DropDown *>(save_button->get_next_sibling())};
    ASSERT_THAT(dropdown_t, ::testing::NotNull());
    ASSERT_THAT(dropdown_t, ::testing::Eq(fractal_dropdown));
    level = dynamic_cast<Gtk::Box *>(level->get_next_sibling());
    auto iter_t {dynamic_cast<Gtk::Scale *>(level->get_first_child()->get_next_sibling())};
    ASSERT_THAT(iter_t, ::testing::NotNull());
    ASSERT_THAT(iter_t, ::testing::Eq(iter_scale));
    level = dynamic_cast<Gtk::Box *>(level->get_next_sibling());
    auto r_min_t {dynamic_cast<Gtk::Scale *>(level->get_first_child()->get_next_sibling()->get_first_child())};
    ASSERT_THAT(r_min_t, ::testing::NotNull());
    ASSERT_THAT(r_min_t, ::testing::Eq(r_min_scale));
    auto r_max_t {dynamic_cast<Gtk::Scale *>(r_min_scale->get_next_sibling())};
    ASSERT_THAT(r_max_t, ::testing::NotNull());
    ASSERT_THAT(r_max_t, ::testing::Eq(r_max_scale));
    level = dynamic_cast<Gtk::Box *>(level->get_next_sibling());
    auto g_min_t {dynamic_cast<Gtk::Scale *>(level->get_first_child()->get_next_sibling()->get_first_child())};
    ASSERT_THAT(g_min_t, ::testing::NotNull());
    ASSERT_THAT(g_min_t, ::testing::Eq(g_min_scale));
    auto g_max_t {dynamic_cast<Gtk::Scale *>(g_min_scale->get_next_sibling())};
    ASSERT_THAT(g_max_t, ::testing::NotNull());
    ASSERT_THAT(g_max_t, ::testing::Eq(g_max_scale));
    level = dynamic_cast<Gtk::Box *>(level->get_next_sibling());
    auto b_min_t {dynamic_cast<Gtk::Scale *>(level->get_first_child()->get_next_sibling()->get_first_child())};
    ASSERT_THAT(b_min_t, ::testing::NotNull());
    ASSERT_THAT(b_min_t, ::testing::Eq(b_min_scale));
    auto b_max_t {dynamic_cast<Gtk::Scale *>(b_min_scale->get_next_sibling())};
    ASSERT_THAT(b_max_t, ::testing::NotNull());
    ASSERT_THAT(b_max_t, ::testing::Eq(b_max_scale));
    level = dynamic_cast<Gtk::Box *>(level->get_next_sibling());
    auto const_r_t {dynamic_cast<Gtk::Scale *>(level->get_first_child()->get_next_sibling()->get_first_child())};
    ASSERT_THAT(const_r_t, ::testing::NotNull());
    ASSERT_THAT(const_r_t, ::testing::Eq(const_r_scale));
    auto const_i_t {dynamic_cast<Gtk::Scale *>(const_r_scale->get_next_sibling())};
    ASSERT_THAT(const_i_t, ::testing::NotNull());
    ASSERT_THAT(const_i_t, ::testing::Eq(const_i_scale));

    ASSERT_THAT(level->get_visible(), ::testing::Eq(true));
    fractal_dropdown->set_selected(1);
    ASSERT_THAT(level->get_visible(), ::testing::Eq(false));
}

TEST_F(FractalTest, FractalFunctionalTest) {
    ASSERT_THAT(fractal_area->fractal_map.at("Mandelbrot")(0, 0, 1024, {}), ::testing::Eq(1024));
    ASSERT_THAT(fractal_area->fractal_map.at("Mandelbrot")(2.5, 2.5, 1024, {}), ::testing::Eq(0));
    ASSERT_THAT(fractal_area->fractal_map.at("Mandelbrot")(-0.75, 0.2, 1024, {}), ::testing::Eq(16));

    ASSERT_THAT(fractal_area->fractal_map.at("Julia")(-0.3713, 0.23418, 1024, {-0.5125, 0.5213}), ::testing::Eq(419));
    ASSERT_THAT(fractal_area->fractal_map.at("Julia")(2.5, 2.5, 1024, {-0.5125, 0.5213}), ::testing::Eq(0));
    ASSERT_THAT(fractal_area->fractal_map.at("Julia")(0, 0, 1024, {-0.5125, 0.5213}), ::testing::Eq(317));
    ASSERT_THAT(fractal_area->fractal_map.at("Julia")(-0.5, 0, 1024, {-0.5125, 0.5213}), ::testing::Eq(30));

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist_i(0, 1024);
    int iter_t {dist_i(gen)};
    fractal_area->set_max_iter(iter_t);
    ASSERT_THAT(fractal_area->max_iter, ::testing::Eq(iter_t));
    std::uniform_real_distribution<double> dist_rgb(0, 100);
    double r_min_t {dist_rgb(gen)};
    double r_max_t {dist_rgb(gen)};
    fractal_area->set_r(r_min_t, r_max_t);
    ASSERT_THAT(fractal_area->r_lower_level, ::testing::Eq(r_min_t/100));
    ASSERT_THAT(fractal_area->r_upper_level, ::testing::Eq(r_max_t/100));
    double g_min_t {dist_rgb(gen)};
    double g_max_t {dist_rgb(gen)};
    fractal_area->set_g(g_min_t, g_max_t);
    ASSERT_THAT(fractal_area->g_lower_level, ::testing::Eq(g_min_t/100));
    ASSERT_THAT(fractal_area->g_upper_level, ::testing::Eq(g_max_t/100));
    double b_min_t {dist_rgb(gen)};
    double b_max_t {dist_rgb(gen)};
    fractal_area->set_b(b_min_t, b_max_t);
    ASSERT_THAT(fractal_area->b_lower_level, ::testing::Eq(b_min_t/100));
    ASSERT_THAT(fractal_area->b_upper_level, ::testing::Eq(b_max_t/100));
    std::uniform_real_distribution<double> dist_c(-1, 1);
    double c_r_t {dist_c(gen)};
    double c_i_t {dist_c(gen)};
    fractal_area->set_consts(c_r_t, c_i_t);
    ASSERT_THAT(fractal_area->const_r, ::testing::Eq(c_r_t));
    ASSERT_THAT(fractal_area->const_i, ::testing::Eq(c_i_t));
    std::uniform_int_distribution<int> dist_string(0, 1);
    std::vector<std::string> options {"Mandelbrot", "Julia"};
    for (int i{}; i < 7; ++i) {
        int selection_t{dist_string(gen)};
        fractal_area->set_selection(options[selection_t]);
        ASSERT_THAT(fractal_area->selection, ::testing::Eq(options[selection_t]));
    }
    fractal_area->cx += 3;
    fractal_area->cy += 2;
    fractal_area->range += 1;
    fractal_area->reset();
    ASSERT_THAT(fractal_area->cx, ::testing::Eq(-0.5));
    ASSERT_THAT(fractal_area->cy, ::testing::Eq(0));
    ASSERT_THAT(fractal_area->range, ::testing::Eq(3.5));
    uint8_t r{1}, g{2}, b{3};
    fractal_area->iter_to_rgb(1024, 1024, r, g, b);
    ASSERT_THAT(r, ::testing::Eq(0));
    ASSERT_THAT(g, ::testing::Eq(0));
    ASSERT_THAT(b, ::testing::Eq(0));
    fractal_area->iter_to_rgb(0, 1024, r, g, b);
    ASSERT_THAT(r, ::testing::Eq(0));
    ASSERT_THAT(g, ::testing::Eq(0));
    ASSERT_THAT(b, ::testing::Eq(0));
}
