#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#pragma pack(push, 1)

typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BMPHeader;

typedef struct {
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BMPInfoHeader;

#pragma pack(pop)

typedef struct{
    BMPHeader * BMPHeader;
    BMPInfoHeader * BMPInfoHeader;
    uint8_t * pixels;
}BMP;

typedef struct{
    char component_name[5];
    int component_value;
}FilterParams;

typedef struct
{
    int red, green, blue;
}RGB;

typedef struct 
{
    int x, y;
}Point;

typedef struct {
    Point left_up;
    int side_size;
    int thickness;
    bool fill;
    RGB color;
    RGB fill_color;
}SquareParams;

typedef struct 
{
    Point left_up, right_down;
    int mode;
}ExchangeParams;

typedef struct 
{
    Point left_up, right_down;
}FragmentParams;

typedef struct 
{
    Point left_up, right_down;
    RGB color;
}ReplaceParams;

typedef struct
{
    int width;
    RGB color;
}FrameParams;




#define ERROR_OPEN_FILE "Сouldn't open %s.\n"
#define ERROR_IS_NOT_BMP "%s format is not BMP.\n"
#define ERROR_BI_BIT_COUNT "BiBitCount isn't 24.\n" 
#define ERROR_UNKNOWN_COMPONENT_NAME "Unknown component_name: %s\nPossible options: red, blue, green.\n"
#define ERROR_UNCORRECT_COMPONENT_VALUE "Uncorrect component value: %d\nPossible values are from 0 to 255.\n"
#define ERROR_UNCORRECT_COORDINATES "Uncorrect coordinates x: %s y: %s\nThe coordinates must be positive integers.\n"
#define ERROR_UNCORRECT_SIDE_SIZE "Uncorrect side_size: %s\nMust be an integer greater than 0.\n"
#define ERROR_UNCORRECT_THICKNESS "Uncorrect thickness: %s\nMust be an integer greater than 0.\n"
#define ERROR_UNCORRECT_COLOR "Uncorrect color: %s.\n"
#define ERROR_UNCORRECT_FILL_VALUE "Uncorrect fill value: %s\nPossible options: true, false.\n"
#define ERROR_MORE_ACTIONS "You have entered more than one command.\n"
#define ERROR_INPUT_AND_OUTPUT_SIMILAR "Input and output file are similar.\n"
#define ERROR_UNCORRECT_EXCHANGE_TYPE "Uncorrect exchange_type: %s\nPossible options: clockwise, diagonals.\n"
#define BMP_TYPE 0x4D42 //"BM"
#define CLOCKWISE_EXCHANGE_TYPE "clockwise"
#define DIAGONALS_EXCHANGE_TYPE "diagonals"
#define COUNTERCLOCKWISE_EXCHANCE_TYPE "counterclockwise"
#define HELP_FILE_NAME "help.txt"

BMP * read_bmp(const char * filename);

bool save_bmp(const char * filename, BMP * bmp);

void filter(FilterParams * filter_params, BMP * bmp);

void draw_square(SquareParams * square_params, BMP * bmp);

void freq_color(RGB * color, BMP * bmp);

bool exchange(ExchangeParams * exchange_params, BMP * bmp);

bool ckeck_correct_component_name(const char * component_name);

bool check_correct_component_value(int component_value);

bool to_coordinate(Point * point, char * params);

bool to_side_size(int * size, const char * side_size);

bool to_thickness(int * thickness, const char * thickness_str);

bool to_color(RGB * color, char * color_str);

bool to_exchange_type(int * exchange_type,const char * exchange_type_str);

void info(BMP * bmp);

void compare(BMP * first, BMP * second);

void help();
