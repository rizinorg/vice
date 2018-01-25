/** \file   src/arch/gtk3/c64ui.c
 * \brief   Native GTK3 C64 UI
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *  Bas Wassink <b.wassink@ziggo.nl>
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

#include "vice.h"

#include <stdio.h>

#include "c64model.h"
#include "machinemodelwidget.h"
#include "not_implemented.h"
#include "sampler.h"
#include "ui.h"
#include "uimachinewindow.h"
#include "settings_sampler.h"
#include "vicii.h"
#include "videomodelwidget.h"
#include "widgethelpers.h"

#include "clockportdevicewidget.h"
#include "clockport.h"

#include "georam.h"
#include "georamwidget.h"
#include "reu.h"
#include "reuwidget.h"
#include "cartridge.h"
#include "ramcartwidget.h"
#include "dqbbwidget.h"
#include "expertwidget.h"
#include "isepicwidget.h"
#include "gmod2widget.h"
#include "mmcrwidget.h"
#include "mmc64widget.h"
#include "retroreplaywidget.h"
#include "easyflashwidget.h"
#include "rrnetmk3widget.h"
#include "uicart.h"
#include "carthelpers.h"

#include "c64ui.h"


/** \brief  List of C64 models
 *
 * Used in the machine-model widget
 */
static const char *c64_model_list[] = {
    "C64 PAL", "C64C PAL", "C64 old PAL",
    "C64 NTSC", "C64C NTSC", "C64 old NTSC",
    "Drean",
    "C64 SX PAL", "C64 SX NTSC",
    "Japanese", "C64 GS",
    "PET64 PAL", "PET64 NTSC",
    "Ultimax", NULL
};


/** \brief  List of VIC-II models
 *
 * Used in the VIC-II model widget
 */
static const vice_gtk3_radiogroup_entry_t c64_vicii_models[] = {
    { "PAL",        VICII_MODEL_PALG },
    { "Old PAL",    VICII_MODEL_PALG_OLD },
    { "NTSC",       VICII_MODEL_NTSCM },
    { "Old NTSC",   VICII_MODEL_NTSCM_OLD },
    { "PAL-N",      VICII_MODEL_PALN },
    { NULL, -1 }
};


/** \brief  Identify the canvas used to create a window
 *
 * \return  window index on success, -1 on failure
 */
static int identify_canvas(video_canvas_t *canvas)
{
    if (canvas != vicii_get_canvas()) {
        return -1;
    }

    return PRIMARY_WINDOW;
}

/** \brief  Pre-initialize the UI before the canvas window gets created
 *
 * \return  0 on success, -1 on failure
 */
int c64ui_init_early(void)
{
    ui_machine_window_init();
    ui_set_identify_canvas_func(identify_canvas);

    INCOMPLETE_IMPLEMENTATION();
    return 0;
}


/** \brief  Initialize the UI
 *
 * \return  0 on success, -1 on failure
 */
int c64ui_init(void)
{
    machine_model_widget_getter(c64model_get);
    machine_model_widget_setter(c64model_set);
    machine_model_widget_set_models(c64_model_list);

    video_model_widget_set_title("VIC-II model");
    video_model_widget_set_resource("MachineVideoStandard");
    video_model_widget_set_models(c64_vicii_models);

    settings_sampler_set_devices_getter(sampler_get_devices);

    /* work around VSID again */
    clockport_device_widget_set_devices((void *)clockport_supported_devices);

    /* I/O extension function pointers */
    carthelpers_set_functions(
            cartridge_save_image,
            cartridge_flush_image,
            cartridge_type_enabled,
            cartridge_enable,
            cartridge_disable);

    /* uicart_set_detect_func(cartridge_detect); only cbm2/plus4 */
    uicart_set_list_func(cartridge_get_info_list);
    uicart_set_attach_func(cartridge_attach_image);
    uicart_set_freeze_func(cartridge_trigger_freeze);
    uicart_set_detach_func(cartridge_detach_image);

    INCOMPLETE_IMPLEMENTATION();
    return 0;
}


/** \brief  Shut down the UI
 */
void c64ui_shutdown(void)
{
    INCOMPLETE_IMPLEMENTATION();
}
