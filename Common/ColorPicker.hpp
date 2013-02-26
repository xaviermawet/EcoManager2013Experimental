#ifndef __COLORPICKER_HPP__
#define __COLORPICKER_HPP__

#include <QColor>

class ColorPicker
{
    public:

        ColorPicker(int num);

        QColor color(int ind, int alpha=255) const;
        QColor light(int ind, int alpha=255) const;
        QColor dark (int ind, int alpha=255) const;

    protected:

        int _inc;
};

#endif /* __COLORPICKER_HPP__ */
