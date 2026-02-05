#include "bmp.h"
#include <getopt.h>
     
int main(int agrc, char * agrs[])
{
    struct option long_options[] = 
    {
        {"rgbfilter", no_argument, NULL, 'r'},
        {"input", required_argument, NULL, 'i'},
        {"output", required_argument, NULL, 'o'},
        {"component_name", required_argument, NULL, 'n'},
        {"component_value", required_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {"info", no_argument, NULL, 'I'},
        {"square", no_argument, NULL, 's'},
        {"left_up", required_argument, NULL, 'l'},
        {"side_size", required_argument, NULL, 'S'},
        {"thickness", required_argument, NULL , 't'},
        {"color", required_argument, NULL, 'c'},
        {"fill", no_argument, NULL, 'f'},
        {"fill_color", required_argument, NULL, 'F'},
        {"freq_color", no_argument, NULL, 'q'},
        {"exchange", no_argument, NULL, 'e'},
        {"right_down", required_argument, NULL, 'R'},
        {"exchange_type", required_argument, NULL, 'T'},
        {"compare", no_argument, NULL, 'C'},
        {0, 0, 0, 0}
    };

    char input_file[50];
    char output_file[50];
    char add_file[50];
    char mirror_arg[50];
    FilterParams * filter_params = malloc(sizeof(FilterParams));
    if(!filter_params){
        printf("Memory allocation error!");
        return 41;
    }
    SquareParams * square_params = malloc(sizeof(SquareParams));
    if(!square_params){
        printf("Memory allocation error!");
        return 41;
    }
    ExchangeParams * exchange_params = malloc(sizeof(ExchangeParams));
    if(!exchange_params){
        printf("Memory allocation error!");
        return 41;
    }
    BMP * bmp;
    char mode = ' ';
    RGB color = {-1, -1, -1};

    int opt;
    while((opt = getopt_long(agrc, agrs, "ri:o:n:v:l:c:t:", long_options, NULL)) != -1)
    {
        switch (opt)
        {
        case 'i':
            strcpy(input_file, optarg);
            break;
        case 'o':
            strcpy(output_file, optarg);
            break;
        case 'n':
            strcpy(filter_params->component_name, optarg);
            if(!ckeck_correct_component_name(filter_params->component_name))
                return 40;
            break;
        case 'v':
            filter_params->component_value = atoi(optarg);
            if(!check_correct_component_value(filter_params->component_value))
                return 41;
            break;
        case 'r':
            if(mode != ' ')
            {
                fprintf(stderr, ERROR_MORE_ACTIONS);
                return 41;
            }
            mode = 'f';
            break;
        case 'I':
            if(mode != ' ')
            {
                fprintf(stderr, ERROR_MORE_ACTIONS);
                return 41;
            }
            mode = 'i';
            break;
        case 's':
            if(mode != ' ')
            {
                fprintf(stderr, ERROR_MORE_ACTIONS);
                return 41;
            }
            mode = 's';
            break;
        case 'l':
            if(!to_coordinate(&square_params->left_up, optarg))
                    return 41;
            exchange_params->left_up = square_params->left_up;
            break;
        case 'S':
            if(!to_side_size(&square_params->side_size, optarg))
                return 41;
            break;
        case 't':
            if(!to_thickness(&square_params->thickness, optarg))
                return 41;
            break;
        case 'c':
            if(!to_color(&square_params->color, optarg))
                return 41;
            color = square_params->color;
            break;
        case 'f':
            square_params->fill = true;
            break;
        case 'F':
            if(!to_color(&square_params->fill_color, optarg))
                return 41;
            break;
        case 'q':
            if(mode != ' ')
            {
                fprintf(stderr, ERROR_MORE_ACTIONS);
                return 41;
            }
            mode = 'q';
            break;
        case 'e':
            if(mode != ' ')
            {
                fprintf(stderr, ERROR_MORE_ACTIONS);
                return 41;
            }
            mode = 'e';
            break;
        case 'R':
            if(!to_coordinate(&exchange_params->right_down, optarg))
                return 41;
            break;
        case 'T':
            if(!to_exchange_type(&exchange_params->mode, optarg))
                return 41;
            break;
        case 'h':
            help();
            return 0;
        default:
            break;
        }
    }

    if(strcmp(input_file, "") == 0)
    {
        strcpy(input_file, agrs[agrc - 1]);
    }

    bmp = read_bmp(input_file);
    if(!bmp) return 41;

    if(strcmp(input_file, output_file) == 0)
    {
        fprintf(stderr, ERROR_INPUT_AND_OUTPUT_SIMILAR);
        return 41;
    }
    
    switch (mode)
    {
    case 'i':
        info(bmp);
        return 0;
        break;
    case 's':
        draw_square(square_params, bmp);
        break;
    case 'f':
        filter(filter_params, bmp);
        break;
    case 'q':
        freq_color(&color, bmp);
        break;
    case 'e':
        exchange(exchange_params, bmp);
        break;
    default:
        break;
    }
    if(!save_bmp(output_file, bmp))
    {
        return 41;
    }

    free(filter_params);
    free(square_params);
    free(exchange_params);

    return 0;
}
