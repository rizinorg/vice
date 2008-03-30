/*
 * vicii-resources.h - Resources for the MOS 6569 (VIC-II) emulation.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
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

#ifndef _VIC_II_RESOURCES_H
#define _VIC_II_RESOURCES_H

#include "vice.h"

/* VIC-II resources.  */
struct vic_ii_resources_s
{
    /* Flag: New or old luminances? */
    int new_luminances;

    /* Flag: Do we emulate the sprite-sprite collision register and IRQ?  */
    int sprite_sprite_collisions_enabled;

    /* Flag: Do we emulate the sprite-background collision register and
       IRQ?  */
    int sprite_background_collisions_enabled;

#ifdef USE_XF86_EXTENSIONS
    /* Flag: Do we use double size?  */
    int fullscreen_double_size_enabled;

    /* Flag: Do we copy lines in double size mode?  */
    int fullscreen_double_scan_enabled;
#endif
};
typedef struct vic_ii_resources_s vic_ii_resources_t;

extern vic_ii_resources_t vic_ii_resources;

extern int vic_ii_resources_init(void);

#endif

