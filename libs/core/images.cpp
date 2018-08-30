#include "pxt.h"

PXT_VTABLE_BEGIN(RefMImage, 0, 0)
PXT_VTABLE_END

RefMImage::RefMImage(ImageData *d) : PXT_VTABLE_INIT(RefMImage), img(d) {
    img->incr();
}

void RefMImage::destroy(RefMImage *t) {
    t->img->decr();
}

void RefMImage::print(RefMImage *t) {
    DMESG("RefMImage %p r=%d size=%d x %d", t, t->refcnt, img->width, img->height);
}

void RefMImage::makeWritable() {
    if (img->isReadOnly()) {
        MicroBitImage i(img);
        img = i.clone().leakData();
    }
}

/**
 * Creation, manipulation and display of LED images.
 */
//% color=#5C2D91 weight=31 icon="\uf03e"
//% advanced=true
namespace images {
/**
 * Creates an image that fits on the LED screen.
 */
//% weight=75 help=images/create-image
//% blockId=device_build_image block="create image"
//% parts="ledmatrix"
Image createImage(ImageLiteral_ leds) {
    return new RefMImage(imageBytes(leds));
}

/**
 * Creates an image with 2 frames.
 */
//% weight=74 help=images/create-big-image
//% blockId=device_build_big_image block="create big image" imageLiteral=2
//% parts="ledmatrix"
Image createBigImage(ImageLiteral_ leds) {
    return createImage(leds);
}
} // namespace images

namespace ImageMethods {
/**
 * Plots the image at a given column to the screen
 */
//% help=images/plot-image
//% parts="ledmatrix"
void plotImage(Image i, int xOffset = 0) {
    if (uBit.display.getDisplayMode() == DISPLAY_MODE_GREYSCALE) {
        i->makeWritable();
        MicroBitImage mbImg(i->img);
        int b = uBit.display.getBrightness();
        for (int row = 0; row < i->img->height; ++row) {
            for (int col = 0; col < i->img->width; ++col) {
                if (mbImg.getPixelValue(col, row) > 0) {
                    mbImg.setPixelValue(col, row, b);
                }
            }
        }
    }
    uBit.display.print(MicroBitImage(i->img), -xOffset, 0, 0, 0);
}

/**
 * Shows an frame from the image at offset ``x offset``.
 * @param xOffset column index to start displaying the image
 */
//% help=images/show-image weight=80 blockNamespace=images
//% blockId=device_show_image_offset block="show image %sprite(myImage)|at offset %offset" blockGap=8
//% parts="ledmatrix" async
void showImage(Image sprite, int xOffset, int interval = 400) {
    if (uBit.display.getDisplayMode() == DISPLAY_MODE_GREYSCALE) {
        sprite->makeWritable();
        MicroBitImage mbImg(sprite->img);
        int b = uBit.display.getBrightness();
        for (int row = 0; row < sprite->img->height; ++row) {
            for (int col = 0; col < sprite->img->width; ++col) {
                if (mbImg.getPixelValue(col, row) > 0) {
                    mbImg.setPixelValue(col, row, b);
                }
            }
        }
    }
    uBit.display.print(MicroBitImage(sprite->img), -xOffset, 0, 0, interval);
}

/**
 * Draws the ``index``-th frame of the image on the screen.
 * @param xOffset column index to start displaying the image
 */
//% help=images/plot-frame weight=80
//% parts="ledmatrix"
void plotFrame(Image i, int xOffset) {
    // TODO showImage() used in original implementation
    plotImage(i, xOffset * 5);
}

/**
 * Scrolls an image .
 * @param frameOffset x offset moved on each animation step, eg: 1, 2, 5
 * @param interval time between each animation step in milli seconds, eg: 200
 */
//% help=images/scroll-image weight=79 async blockNamespace=images
//% blockId=device_scroll_image
//% block="scroll image %sprite(myImage)|with offset %frameoffset|and interval (ms) %delay"
//% blockGap=8 parts="ledmatrix"
void scrollImage(Image id, int frameOffset, int interval) {
    MicroBitImage i(id->img);
    if (uBit.display.getDisplayMode() == DISPLAY_MODE_GREYSCALE) {
        id->makeWritable();
        int b = uBit.display.getBrightness();
        for (int row = 0; row < id->img->height; ++row) {
            for (int col = 0; col < id->img->width; ++col) {
                if (i.getPixelValue(col, row) > 0) {
                    i.setPixelValue(col, row, b);
                }
            }
        }
    }
    uBit.display.animate(i, interval, frameOffset, MICROBIT_DISPLAY_ANIMATE_DEFAULT_POS, 0);
}

/**
 * Sets all pixels off.
 */
//% help=images/clear
//% parts="ledmatrix"
void clear(Image i) {
    i->makeWritable();
    MicroBitImage(i->img).clear();
}

/**
 * Sets a specific pixel brightness at a given position
 */
//%
//% parts="ledmatrix"
void setPixelBrightness(Image i, int x, int y, int value) {
    i->makeWritable();
    MicroBitImage(i->img).setPixelValue(x, y, value);
}

/**
 * Gets the pixel brightness ([0..255]) at a given position
 */
//%
//% parts="ledmatrix"
int pixelBrightness(Image i, int x, int y) {
    int pix = MicroBitImage(i->img).getPixelValue(x, y);
    if (pix < 0)
        return 0;
    return pix;
}

/**
 * Gets the width in columns
 */
//% help=functions/width
int width(Image i) {
    return i->img->width;
}

/**
 * Gets the height in rows (always 5)
 */
//%
int height(Image i) {
    return i->img->height;
}

/**
 * Set a pixel state at position ``(x,y)``
 * @param x TODO
 * @param y TODO
 * @param value TODO
 */
//% help=images/set-pixel
//% parts="ledmatrix"
void setPixel(Image i, int x, int y, bool value) {
    setPixelBrightness(i, x, y, value ? 255 : 0);
}

/**
 * Get the pixel state at position ``(x,y)``
 * @param x TODO
 * @param y TODO
 */
//% help=images/pixel
//% parts="ledmatrix"
bool pixel(Image i, int x, int y) {
    return pixelBrightness(i, x, y) > 0;
}

/**
 * Shows a particular frame of the image strip.
 * @param frame TODO
 */
//% weight=70 help=images/show-frame
//% parts="ledmatrix"
void showFrame(Image i, int frame, int interval = 400) {
    showImage(i, frame * 5, interval);
}
} // namespace ImageMethods