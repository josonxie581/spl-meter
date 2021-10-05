/*
 * gpio.cpp
 *
 *  Created on: 20 paź 2020
 *      Author: kwarc
 */

#include "gpio.hpp"

#include <cstdint>

#include <drivers/stm32l4/rcc.hpp>

using namespace drivers;

//-----------------------------------------------------------------------------
/* private */

void gpio::enable_clock(port port)
{
    switch (port)
    {
        case port::porta:
            rcc::toggle_periph_clock(RCC_PERIPH_BUS(AHB2, GPIOA), true);
            break;
        case port::portb:
            rcc::toggle_periph_clock(RCC_PERIPH_BUS(AHB2, GPIOB), true);
            break;
        case port::portc:
            rcc::toggle_periph_clock(RCC_PERIPH_BUS(AHB2, GPIOC), true);
            break;
        case port::portd:
            rcc::toggle_periph_clock(RCC_PERIPH_BUS(AHB2, GPIOD), true);
            break;
        case port::porte:
            rcc::toggle_periph_clock(RCC_PERIPH_BUS(AHB2, GPIOE), true);
            break;
        case port::portf:
            rcc::toggle_periph_clock(RCC_PERIPH_BUS(AHB2, GPIOF), true);
            break;
        case port::portg:
            rcc::toggle_periph_clock(RCC_PERIPH_BUS(AHB2, GPIOG), true);
            break;
        case port::porth:
            rcc::toggle_periph_clock(RCC_PERIPH_BUS(AHB2, GPIOH), true);
            break;
        default:
            break;
    }
}

//-----------------------------------------------------------------------------
/* public */

void gpio::configure(const io &io, mode mode, af af, pupd pupd, type type, speed speed)
{
    enable_clock(io.port);

    GPIO_TypeDef *port = reinterpret_cast<GPIO_TypeDef *> (io.port);
    uint8_t pin = static_cast<uint8_t> (io.pin);

    port->MODER &= ~(0b11 << (2 * pin));
    port->MODER |= (uint8_t (mode) << (2 * pin));

    port->OTYPER &= ~(0b1 << pin);
    port->OTYPER |= (uint8_t (type) << (pin));

    port->OSPEEDR &= ~(0b11 << (2 * pin));
    port->OSPEEDR |= (uint8_t (speed) << (2 * pin));

    port->PUPDR &= ~(0b11 << (2 * pin));
    port->PUPDR |= (uint8_t (pupd) << (2 * pin));

    port->AFR[pin < 8 ? 0 : 1] &= ~(0b1111 << (4 * (pin - (pin < 8 ? 0 : 8))));
    port->AFR[pin < 8 ? 0 : 1] |= (uint8_t (af) << (4 * (pin - (pin < 8 ? 0 : 8))));
}

bool gpio::read(const io &io)
{
    GPIO_TypeDef *port = reinterpret_cast<GPIO_TypeDef *> (io.port);
    uint8_t pin = static_cast<uint8_t> (io.pin);
    return (port->IDR & (1 << pin));
}

void gpio::write(const io &io, bool state)
{
    GPIO_TypeDef *port = reinterpret_cast<GPIO_TypeDef *> (io.port);
    uint8_t pin = static_cast<uint8_t> (io.pin);
    port->BSRR = state ? (1 << pin) : (1 << pin) << 16;
}

void gpio::toggle(const io &io)
{
    GPIO_TypeDef *port = reinterpret_cast<GPIO_TypeDef *> (io.port);
    uint8_t pin = static_cast<uint8_t> (io.pin);
    port->ODR ^= 1 << pin;
}

