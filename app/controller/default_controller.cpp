/*
 * default_controller.cpp
 *
 *  Created on: 5 paź 2021
 *      Author: kwarc
 */

#include <functional>

#include "default_controller.hpp"

#include <app/model/meter.hpp>

using namespace spl;

//-----------------------------------------------------------------------------
/* private */

void default_controller::model_new_data_callback(const spl::data_t &model_data)
{
    /* Translate to view data type */
    char weighting;
    switch (model_data.weighting)
    {
        case spl::weighting_t::a:
            weighting = 'A';
            break;
        case spl::weighting_t::c:
            weighting = 'C';
            break;
        case spl::weighting_t::z:
            weighting = 'Z';
            break;
        default:
            weighting = ' ';
            break;
    }

    char averaging;
    switch (model_data.averaging)
    {
        case spl::averaging_t::slow:
            averaging = 'S';
            break;
        case spl::averaging_t::fast:
            averaging = 'F';
            break;
        default:
            averaging = ' ';
            break;
    }

    const spl::view_interface::data view_data =
    {
        .spl = model_data.spl,
        .max_spl = model_data.spl_max,
        .min_spl = model_data.spl_min,
        .weighting = weighting,
        .averaging = averaging,
        .error_code = this->error_code,
    };

    for (auto view : this->views)
    {
        if (view != nullptr)
            view->update(view_data);
    }
}

void default_controller::update(const event_t &data)
{
    std::visit([this](auto &&e) { return this->event_handler(e); }, data);
}

void default_controller::event_handler(const change_averaging_evt_t &e)
{
    this->model->set_averaging(e.averaging);
}

void default_controller::event_handler(const change_weighting_evt_t &e)
{
    this->model->set_weighting(e.weighting);
}

void default_controller::event_handler(const clear_min_spl_data_evt_t &e)
{
    this->model->reset_min_spl_data();
}

void default_controller::event_handler(const clear_max_spl_data_evt_t &e)
{
    this->model->reset_max_spl_data();
}

//-----------------------------------------------------------------------------
/* public */

default_controller::default_controller(meter &model, std::vector<view_interface*> &views) : model {&model}, views {views}
{
    this->error_code = 0;

    for (auto view : this->views)
        view->attach(this);

    this->model->set_new_data_callback([this](const spl::data_t &data) { this->model_new_data_callback(data); });
    this->model->enable();
}

default_controller::~default_controller(void)
{

}

void default_controller::process(void)
{
    this->model->process();

    for (auto view : this->views)
    {
        if (view != nullptr)
            view->process();
    }
}
