#include "bmp.h"



BMP *read_bmp(const char *filename)
{
    FILE * file = fopen(filename, "rb");

    if(!file)
    {
        fprintf(stderr, ERROR_OPEN_FILE, filename);
        return NULL;
    }

    BMPHeader * bmpHeader = malloc(sizeof(BMPHeader));
    BMPInfoHeader * bmpInfoHeader = malloc(sizeof(BMPInfoHeader));

    fread(bmpHeader, 1,sizeof(BMPHeader),file);
    fread(bmpInfoHeader, 1,sizeof(BMPInfoHeader), file);

    if(bmpHeader->bfType != BMP_TYPE)
    {
        fprintf(stderr, ERROR_IS_NOT_BMP, filename);
        free(bmpHeader);
        free(bmpInfoHeader);
        return NULL;
    }
    else if(bmpInfoHeader->biBitCount != 24)
    {
        fprintf(stderr, ERROR_BI_BIT_COUNT);
        free(bmpHeader);
        free(bmpInfoHeader);
        return NULL;
    }

    BMP * bmp = malloc(sizeof(BMP));
    bmp->BMPHeader = bmpHeader;
    bmp->BMPInfoHeader = bmpInfoHeader;

    if(bmp->BMPInfoHeader->biSizeImage == 0)
    {
        int width = bmp->BMPInfoHeader->biWidth;
        int height = abs(bmp->BMPInfoHeader->biHeight);
        int rowSize = ((width * 3 + 3) / 4) * 4;
        bmp->BMPInfoHeader->biSizeImage = rowSize * height;
    }

    bmp->pixels = malloc(bmp->BMPInfoHeader->biSizeImage);
    fseek(file, bmp->BMPHeader->bfOffBits, SEEK_SET);
    fread(bmp->pixels,1, bmp->BMPInfoHeader->biSizeImage, file);

    fclose(file);

    return bmp;
}

bool save_bmp(const char *filename, BMP * bmp)
{
    FILE * file = fopen(filename, "wb");
    if(!file)
    {
        fprintf(stderr, ERROR_OPEN_FILE, filename);
        return false;
    }

    fwrite(bmp->BMPHeader, sizeof(BMPHeader), 1, file);
    fwrite(bmp->BMPInfoHeader, sizeof(BMPInfoHeader), 1, file);
    fseek(file, bmp->BMPHeader->bfOffBits, SEEK_SET);
    fwrite(bmp->pixels, 1, bmp->BMPInfoHeader->biSizeImage, file);

    fclose(file);

    return true;
}

int get_component_index(const char * component_name)
{
    if(!strcmp(component_name, "blue"))
    {
        return 0;
    }
    else if(!strcmp(component_name, "green"))
    {
        return 1;
    }
    return 2;
}

void filter(FilterParams * filter_params, BMP * bmp)
{
    int rowSize = ((bmp->BMPInfoHeader->biWidth * 3 + 3 ) /4) * 4;
    int component_index = get_component_index(filter_params->component_name);

    for(int y = 0; y < bmp->BMPInfoHeader->biHeight; y++)
    {
        for(int x = 0; x < bmp->BMPInfoHeader->biWidth; x++)
        {
            uint8_t * pixel = &bmp->pixels[(bmp->BMPInfoHeader->biHeight - y -1) * rowSize + x * 3];
            pixel[component_index] = filter_params->component_value;
        }
    }
}

void draw_square(SquareParams * square_params, BMP * bmp)
{
    int rowSize = ((bmp->BMPInfoHeader->biWidth * 3 + 3) / 4) * 4;

    if(square_params->left_up.x - square_params->thickness / 2 < 0)
    {
        square_params->left_up.x = 0;
    }
    else
    {
        square_params->left_up.x -= square_params->thickness / 2;
    }

    if(square_params->left_up.y - square_params->thickness / 2 < 0)
    {
        square_params->left_up.y = 0;
    }
    else
    {
        square_params->left_up.y -= square_params->thickness / 2;
    }

    for(int y = square_params->left_up.y; (y <= square_params->left_up.y + square_params->side_size + square_params->thickness / 2) && y < bmp->BMPInfoHeader->biHeight;y++)
    {
        for(int x = square_params->left_up.x; (x <= square_params->left_up.x + square_params->side_size + square_params->thickness / 2) && x < bmp->BMPInfoHeader->biWidth; x++)
        {
            uint8_t * pixel = &bmp->pixels[(bmp->BMPInfoHeader->biHeight - y -1) * rowSize + x * 3];

            if(square_params->fill == false && !(x < square_params->left_up.x + square_params->thickness) && !(x > square_params->left_up.x + square_params->side_size - square_params->thickness)
                && !(y < square_params->left_up.y + square_params->thickness) && !(y > square_params->left_up.y + square_params->side_size - square_params->thickness))
                continue;
                
            else if(square_params->fill == true && !(x < square_params->left_up.x + square_params->thickness) && !(x > square_params->left_up.x + square_params->side_size - square_params->thickness)
                && !(y < square_params->left_up.y + square_params->thickness) && !(y > square_params->left_up.y + square_params->side_size - square_params->thickness))
                {
                    pixel[0] = square_params->fill_color.blue;
                    pixel[1] = square_params->fill_color.green;
                    pixel[2] = square_params->fill_color.red;
                }
            else{
                pixel[0] = square_params->color.blue;
                pixel[1] = square_params->color.green;
                pixel[2] = square_params->color.red;
            }
        }
    }
}

void freq_color(RGB * color, BMP * bmp)
{
    int *** colors = calloc(256, sizeof(int **));
    for(int i = 0; i < 256;i++)
    {
        colors[i] = calloc(256, sizeof(int *));
        for(int j = 0;j < 256;j++)
        {
            colors[i][j] = calloc(256, sizeof(int)); 
        }
    }

    int rowSize = ((bmp->BMPInfoHeader->biWidth * 3 + 3) / 4) * 4;

    for(int y = 0; y < bmp->BMPInfoHeader->biHeight; y++)
    {
        for(int x = 0; x < bmp->BMPInfoHeader->biWidth;x++)
        {
            uint8_t  * pixel = &bmp->pixels[y * rowSize + x * 3];
            colors[pixel[0]][pixel[1]][pixel[2]]++;
        }
    }

    RGB max_color = {0, 0,0};
    int max_count = 0;

    for(int i = 0; i < 256; i++)
    {
        for(int j = 0; j < 256; j++)
        {
            for(int k = 0; k < 256; k++)
            {
                if(colors[i][j][k] > max_count)
                {
                    max_color.red = k;
                    max_color.green = j;
                    max_color.blue = i;
                    max_count = colors[i][j][k];
                }
            }
        }
    }

    for(int y = 0; y < bmp->BMPInfoHeader->biHeight; y++)
    {
        for(int x = 0; x < bmp->BMPInfoHeader->biWidth; x++)
        {
            uint8_t * pixel = &bmp->pixels[y * rowSize + x * 3];
            if(pixel[0] == max_color.blue & pixel[1] == max_color.green & pixel[2] == max_color.red)
            {
                pixel[0] = color->blue;
                pixel[1] = color->green;
                pixel[2] = color->red;
            }
        }
    }

    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            free(colors[i][j]);
        }
        free(colors[i]);
    }
    free(colors);
}

void swap_fragment(FragmentParams * first_fragment, FragmentParams * second_fragment, BMP * bmp)
{
    int rowSizeBmp = ((bmp->BMPInfoHeader->biWidth * 3 + 3) / 4) * 4;

    for(int y = first_fragment->left_up.y, j = second_fragment->left_up.y; y <= first_fragment->right_down.y && j <= second_fragment->right_down.y; y++, j++)
    {
        for(int x = first_fragment->left_up.x, i = second_fragment->left_up.x; x <= first_fragment->right_down.x && i <= second_fragment->right_down.x; x++, i++)
        {
            uint8_t * pixel_first = &bmp->pixels[rowSizeBmp * (bmp->BMPInfoHeader->biHeight - y -1) + x * 3];
            uint8_t * pixel_second = &bmp->pixels[rowSizeBmp * (bmp->BMPInfoHeader->biHeight - j - 1) + i * 3];
            uint8_t * tmp = malloc(sizeof(3));
            tmp[0] = pixel_first[0];
            tmp[1] = pixel_first[1];
            tmp[2] = pixel_first[2];

            pixel_first[0] = pixel_second[0];
            pixel_first[1] = pixel_second[1];
            pixel_first[2] = pixel_second[2];

            pixel_second[0] = tmp[0];
            pixel_second[1] = tmp[1];
            pixel_second[2] = tmp[2];

            free(tmp);
        }
    }
}

bool exchange(ExchangeParams *exchange_params, BMP *bmp)
{

    if(exchange_params->right_down.y > bmp->BMPInfoHeader->biHeight)
    {
        exchange_params->right_down.y = bmp->BMPInfoHeader->biHeight;
    }

    if(exchange_params->right_down.x > bmp->BMPInfoHeader->biWidth)
    {
        exchange_params->right_down.x = bmp->BMPInfoHeader->biWidth;
    }

    if((exchange_params->right_down.x - exchange_params->left_up.x) % 2 != 0)
        exchange_params->right_down.x--;
    if((exchange_params->right_down.y - exchange_params->left_up.y) % 2 != 0)
        exchange_params->right_down.y--;

    int width = (exchange_params->right_down.x - exchange_params->left_up.x);
    int height = (exchange_params->right_down.y - exchange_params->left_up.y);


    FragmentParams first = {{exchange_params->left_up.x, exchange_params->left_up.y}, {exchange_params->left_up.x + width / 2 - 1,  exchange_params->left_up.y + height / 2 - 1}};
    FragmentParams second = {{exchange_params->left_up.x + width / 2, exchange_params->left_up.y}, {exchange_params->right_down.x, exchange_params->left_up.y + height / 2 - 1}};
    FragmentParams third = {{exchange_params->left_up.x, exchange_params->left_up.y + height / 2}, {exchange_params->left_up.x + width / 2 - 1, exchange_params->right_down.y - 1}};
    FragmentParams fourth = {{exchange_params->left_up.x + width / 2, exchange_params->left_up.y + height / 2}, {exchange_params->right_down.x, exchange_params->right_down.y - 1}};
    

    switch (exchange_params->mode)
    {
    case 1:
        swap_fragment(&first, &second, bmp);
        swap_fragment(&third, &fourth, bmp);
        swap_fragment(&first, &fourth, bmp);
        break;
    case 2:
        swap_fragment(&first, &fourth, bmp);
        swap_fragment(&second, &third, bmp);
        break;
    case 3:
        swap_fragment(&first, &third, bmp);
        swap_fragment(&first, &second, bmp);
        swap_fragment(&second, &fourth, bmp);
        break;
        
    default:
        break;
    }

}

bool ckeck_correct_component_name(const char *component_name)
{
    if(strcmp(component_name, "red") == 0 || strcmp(component_name, "blue") == 0 || strcmp(component_name, "green") == 0)
        return true;
    fprintf(stderr, ERROR_UNKNOWN_COMPONENT_NAME, component_name);
    return false;
}

bool check_correct_component_value(int component_value)
{
    if(component_value >= 0 && component_value <= 255)
        return true;
    fprintf(stderr, ERROR_UNCORRECT_COMPONENT_VALUE, component_value);
    return false;
}

bool is_integer(const char * str)
{
    char * endptr;
    strtol(str, &endptr, 10);
    return *endptr == '\0';
}

bool to_coordinate(Point * point, char *params)
{
    char * x_str = strtok(params, ".");
    char * y_str = strtok(NULL, ".");
    if(is_integer(x_str) & is_integer(y_str) & atoi(x_str) >= 0 & atoi(y_str) >= 0)
    {
        point->x = atoi(x_str);
        point->y = atoi(y_str);
        return true;
    }
    else
    {
        fprintf(stderr, ERROR_UNCORRECT_COORDINATES, x_str, y_str);
        return false;
    }
}

bool to_side_size(int * size, const char * side_size)
{
    if(is_integer(side_size) & atoi(side_size) >= 0)
    {
        *size = atoi(side_size);
        return true;
    }
    fprintf(stderr, ERROR_UNCORRECT_SIDE_SIZE, side_size);
    return false;
}

bool to_thickness(int *thickness, const char *thickness_str)
{
    if(is_integer(thickness_str) & atoi(thickness_str) > 0)
    {
        *thickness = atoi(thickness_str);
        return true;
    }
    fprintf(stderr, ERROR_UNCORRECT_THICKNESS, thickness_str);
    return false;
}


bool to_color(RGB * color, char *color_str)
{
    char * red_str = strtok(color_str, ".");
    char * green_str = strtok(NULL, ".");
    char * blue_str = strtok(NULL, ".");
    if(is_integer(red_str) & is_integer(green_str) & is_integer(blue_str))
    {
        color->red = atoi(red_str);
        color->green = atoi(green_str);
        color->blue = atoi(blue_str);
        if(check_correct_component_value(color->red) & check_correct_component_value(color->green) & check_correct_component_value(color->blue))
        {
            return true;
        }
        return false;
    }
    fprintf(stderr, ERROR_UNCORRECT_COLOR, color_str);
    return false;
}

bool to_exchange_type(int * exchange_type, const char *exchange_type_str)
{
    if(strcmp(exchange_type_str, CLOCKWISE_EXCHANGE_TYPE) == 0)
    {
        *exchange_type = 1;
        return true;
    }
    else if(strcmp(exchange_type_str, DIAGONALS_EXCHANGE_TYPE) == 0)
    {
        *exchange_type = 2;
        return true;
    }
    else if(strcmp(exchange_type_str, COUNTERCLOCKWISE_EXCHANCE_TYPE) == 0)
    {
        *exchange_type = 3;
        return true;
    }
    fprintf(stderr, ERROR_UNCORRECT_EXCHANGE_TYPE, exchange_type_str);
    return false;
}

void info(BMP *bmp)
{
    printf("biSize: %u\n", bmp->BMPInfoHeader->biSize);
    printf("biWidth: %d\n", bmp->BMPInfoHeader->biWidth);
    printf("biHeight: %d\n", bmp->BMPInfoHeader->biHeight);
    printf("biPlanes: %u\n", bmp->BMPInfoHeader->biPlanes);
    printf("biBitCount: %u\n", bmp->BMPInfoHeader->biBitCount);
    printf("biCompression: %u\n", bmp->BMPInfoHeader->biCompression);
    printf("biSizeImage: %u\n", bmp->BMPInfoHeader->biSizeImage);
    printf("biXPelsPerMeter: %u\n", bmp->BMPInfoHeader->biXPelsPerMeter);
    printf("biYPelsPerMeter: %u\n", bmp->BMPInfoHeader->biYPelsPerMeter);
    printf("biClrUsed: %u\n", bmp->BMPInfoHeader->biClrUsed);
    printf("biClrImportant: %u\n", bmp->BMPInfoHeader->biClrImportant);
}


void compare(BMP * first, BMP * second)
{
    int rowSizeBmp = ((first->BMPInfoHeader->biWidth * 3 + 3) / 4) * 4;
    for(int y = 0; y < first->BMPInfoHeader->biHeight; y++)
    {
        for(int x = 0; x < first->BMPInfoHeader->biWidth; x++)
        {
            uint8_t * pixel_first = &first->pixels[rowSizeBmp * (first->BMPInfoHeader->biHeight - y -1) + x * 3];
            uint8_t * pixel_second = &second->pixels[rowSizeBmp * (second->BMPInfoHeader->biHeight - y - 1) + x * 3];
            if(pixel_first[0] != pixel_second[0] || pixel_first[1] != pixel_second[1] || pixel_first[2] != pixel_second[2])
            {
                printf("x, y: %d, %d\n", x, y);
            }
        }
    }
}

void help()
{
    printf("Course work for option 5.3, created by Vladimir Brovin.\n\n");

    printf("RGB Component Filter (--rgbfilter)\n");
    printf("Sets a specific RGB component to a fixed value across the entire image.\n\n");
    printf("Arguments:\n");
    printf("    --component_name: Name of the component to change. Options:\n");
    printf("        red\n");
    printf("        green\n");
    printf("        blue\n");
    printf("    --component_value: Integer value in the range [0, 255] to assign to the selected component.\n\n");
    printf("Usage:\n");
    printf("    --rgbfilter --component_name red --component_value 128\n\n");

    printf("Draw Square (--square)\n");
    printf("Draws a square on the image with specified properties.\n\n");
    printf("Arguments:\n");
    printf("    --left_up: Coordinates of the top-left corner, format: x.y (e.g., 100.50).\n");
    printf("    --side_size: Length of the square's side. Must be greater than 0.\n");
    printf("    --thickness: Border thickness in pixels. Must be greater than 0.\n");
    printf("    --color: Border color in format rrr.ggg.bbb (e.g., 255.0.0 for red).\n");
    printf("    --fill: Optional flag. If present, fills the square.\n");
    printf("    --fill_color: Fill color in format rrr.ggg.bbb. Required if --fill is used.\n\n");
    printf("Usage:\n");
    printf("    --square --left_up 50.50 --side_size 100 --thickness 5 --color 0.255.0 --fill --fill_color 255.0.0\n\n");

    printf("Exchange Image Quadrants (--exchange)\n");
    printf("Divides a rectangular area into 4 parts and swaps them.\n\n");
    printf("Arguments:\n");
    printf("    --left_up: Top-left corner of the region, format: x.y.\n");
    printf("    --right_down: Bottom-right corner of the region, format: x.y\n");
    printf("    --exchange_type: Swap strategy. Options:\n");
    printf("        clockwise\n");
    printf("        counterclockwise\n");
    printf("        diagonals\n\n");
    printf("Usage:\n");
    printf("    --exchange --left_up 10.10 --right_down 200.200 --exchange_type clockwise\n\n");

    printf("Replace Most Frequent Color (--freq_color)\n");
    printf("Finds the most frequent color in the image and replaces it with a given color.\n\n");
    printf("Arguments:\n");
    printf("    --color: New color in format rrr.ggg.bbb.\n\n");
    printf("Usage:\n");
    printf("    --freq_color --color 0.0.255\n\n");

    printf("Notes:\n");
    printf("Color components must be integers in the range [0, 255].\n");
    printf("Coordinate format is x.y where x and y are integers.\n");
    printf("Invalid arguments or missing required flags will result in execution errors.\n");
}

void draw_line(RGB color, Point p0, Point p1, BMP * bmp)
{
    int rowSize = ((bmp->BMPInfoHeader->biWidth * 3 + 3) / 4) * 4;

    int x0 = p0.x, x1 = p1.x;
    int y0 = p0.y, y1 = p1.y;

    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx + dy;

    while (1) {
        if (x0 >= 0 && x0 < bmp->BMPInfoHeader->biWidth && y0 >= 0 && y0 < bmp->BMPInfoHeader->biHeight) {
            int offset = (bmp->BMPInfoHeader->biHeight - y0 - 1) * rowSize + x0 * 3;
            uint8_t * pixel = &bmp->pixels[offset];
            pixel[0] = color.blue;
            pixel[1] = color.green;
            pixel[2] = color.red;
        }

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}
