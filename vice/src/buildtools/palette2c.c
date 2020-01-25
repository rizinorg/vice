/*
 * palette2c - convert a palette file to an array.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char line_buffer[512];

int get_line(FILE *file)
{
    int c = 0;
    int counter = 0;

    while (c != '\n' && !feof(file) && counter < 511) {
        c = fgetc(file);
        if (c != 0xd) {
            line_buffer[counter++] = (char)c;
        }
    }
    if (counter > 1) {
        line_buffer[counter - 1] = 0;
    }

    if (feof(file)) {
        return -1;
    }

    return counter - 1;
}

static char *vice_stralloc(const char *str)
{
    size_t size;
    char *ptr;

    if (str == NULL) {
        exit(-1);
    }

    size = strlen(str) + 1;
    ptr = malloc(size);

    memcpy(ptr, str, size);

    return ptr;
}


/** \brief  Attempt to parse a 8-bit unsigned integer from string \a s
 *
 * \param[in]   s       string to parse
 * \param[out]  endptr  object to store pointer to first non-valid char
 *
 * \return  value or -1 on error
 */
static int parse_hex_value(const char *s, char **endptr)
{
    long result;

    /* skip white space */
    if (s == NULL || *s == '\0') {
        return -1;
    }
    while (isspace((int)*s)) {
        s++;
    }

    result = strtol(s, endptr, 16);
    if (s == *endptr || result > 0xff) {
        return -1;
    }
    return (int)result;
}


static void pal2bin(char *in_filename, char *out_filename, char *array_name)
{
    FILE *infile = NULL;
    FILE *outfile = NULL;
    int line_size;
    int amount = 0;

    int red, blue, green, dither;

    infile = fopen(in_filename, "rb");
    if (infile == NULL) {
        printf("cannot open %s for reading\n", in_filename);
        return;
    }

    outfile = fopen(out_filename, "wb");
    if (outfile == NULL) {
        printf("cannot open %s for writing\n", out_filename);
        fclose(infile);
        return;
    }

    fprintf(outfile, "/* Autogenerated file, do not edit */\n\n");
    fprintf(outfile, "%s = {\n", array_name);

    line_size = get_line(infile);

    while (line_size != -1) {
        if (line_size != 0) {
            if (line_buffer[0] != ' ' && line_buffer[0] != '#') {
                char *endptr;

                red = parse_hex_value(line_buffer, &endptr);
                if (red < 0) {
                    fprintf(stderr, "palette2c: failed to convert RGB red\n");
                    exit(EXIT_FAILURE);
                }

                green = parse_hex_value(endptr, &endptr);
                if (green < 0) {
                    fprintf(stderr, "palette2c: failed to convert RGB green\n");
                    exit(EXIT_FAILURE);
                }

                blue = parse_hex_value(endptr, &endptr);
                if (blue < 0) {
                    fprintf(stderr, "palette2c: failed to convert RGB blue\n");
                    exit(EXIT_FAILURE);
                }

                dither = parse_hex_value(endptr, &endptr);
                if (dither < 0) {
                    fprintf(stderr, "palette2c: failed to convert dither value, ignoring\n");
                    dither = 0;
                }

                fprintf(outfile, "    0x%02x, 0x%02x, 0x%02x, 0x%02x,\n", red, green, blue, dither);
                amount++;
            }
        }
        line_size = get_line(infile);
    }
    fprintf(outfile, "    %d};\n", amount);

    fclose(outfile);
    fclose(infile);
}

int main(int argc, char *argv[])
{
    int i;

    if (argc < 3) {
        printf("too few arguments\n");
        exit(1);
    }

    /* argv[1] = .vpl file for reading */
    /* argv[2] = header file for writing */
    /* argv[3] = array name */

    pal2bin(argv[1], argv[2], argv[3]);

    return 0;
}
