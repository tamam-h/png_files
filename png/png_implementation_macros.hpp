#pragma once

#define APPLY_TO_WRITABLE_PIXEL_TYPES(macro)\
macro(greyscale_1)\
macro(greyscale_2)\
macro(greyscale_4)\
macro(greyscale_8)\
macro(greyscale_16)\
macro(truecolour_8)\
macro(truecolour_16)\
macro(greyscale_with_alpha_8)\
macro(greyscale_with_alpha_16)\
macro(truecolour_with_alpha_8)\
macro(truecolour_with_alpha_16)

#define APPLY_TO_ALL_PIXEL_TYPES(macro)\
APPLY_TO_WRITABLE_PIXEL_TYPES(macro)\
macro(greyscale_float)\
macro(truecolour_float)\
macro(greyscale_with_alpha_float)\
macro(truecolour_with_alpha_float)