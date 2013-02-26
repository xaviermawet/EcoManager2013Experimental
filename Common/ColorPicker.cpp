#include "ColorPicker.hpp"

ColorPicker::ColorPicker(int num)
{
    if (num == 0)
        num = 1;
    this->_inc = 360 / (num % 360);
}

QColor ColorPicker::color(int index, int alpha) const
{
    return QColor::fromHsv((this->_inc * index) % 360, 230, 230, alpha);
}

QColor ColorPicker::light(int index, int alpha) const
{
    return QColor::fromHsv((this->_inc * index) % 360, 255, 255, alpha);
}

QColor ColorPicker::dark(int index, int alpha) const
{
    return QColor::fromHsv((this->_inc * index) % 360, 200, 200, alpha);
}
