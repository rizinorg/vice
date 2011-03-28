/*
 * cbm2uires.h
 *
 * Written by
 *  Mathias Roslund <vice.emu@amidog.se>
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

#ifndef VICE_CBM2UIRES_H_
#define VICE_CBM2UIRES_H_

#include "uires.h"
#include "intl.h"

static struct TranslateNewMenu UI_MENU_NAME[] = {
  TITLE(IDMS_FILE, NULL)
#ifndef AMIGA_AROS
    ITEM(IDMS_AUTOSTART_IMAGE,        NULL, IDM_AUTOSTART)
    ITEMSEPARATOR()
#endif
    ITEM(IDMS_ATTACH_DISK_IMAGE,      NULL, NULL)
      SUB(IDMS_DRIVE_8,               "8",  IDM_ATTACH_8)
      SUB(IDMS_DRIVE_9,               "9",  IDM_ATTACH_9)
      SUB(IDMS_DRIVE_10,              "0",  IDM_ATTACH_10)
      SUB(IDMS_DRIVE_11,              "1",  IDM_ATTACH_11)
    ITEM(IDMS_DETACH_DISK_IMAGE,      NULL, NULL)
      SUB(IDMS_DRIVE_8,               NULL, IDM_DETACH_8)
      SUB(IDMS_DRIVE_9,               NULL, IDM_DETACH_9)
      SUB(IDMS_DRIVE_10,              NULL, IDM_DETACH_10)
      SUB(IDMS_DRIVE_11,              NULL, IDM_DETACH_11)
      SUBSEPARATOR()
      SUB(IDMS_ALL,                   "A",  IDM_DETACH_ALL)
    ITEM(IDMS_FLIP_LIST,              NULL, NULL)
      SUB(IDMS_ADD_CURRENT_IMAGE,     "I",  IDM_FLIP_ADD)
      SUB(IDMS_REMOVE_CURRENT_IMAGE,  "K",  IDM_FLIP_REMOVE)
      SUB(IDMS_ATTACH_NEXT_IMAGE,     "N",  IDM_FLIP_NEXT)
      SUB(IDMS_ATTACH_PREVIOUS_IMAGE, "B",  IDM_FLIP_PREVIOUS)
      SUBSEPARATOR()
      SUB(IDMS_LOAD_FLIP_LIST,        NULL, IDM_FLIP_LOAD)
      SUB(IDMS_SAVE_FLIP_LIST,        NULL, IDM_FLIP_SAVE)
    ITEMSEPARATOR()
    ITEM(IDMS_ATTACH_TAPE_IMAGE,      "T",  IDM_ATTACH_TAPE)
    ITEM(IDMS_DETACH_TAPE_IMAGE,      NULL,          IDM_DETACH_TAPE)
    ITEM(IDMS_DATASSETTE_CONTROL,     NULL, NULL)
      SUB(IDMS_STOP,                  NULL, IDM_DATASETTE_CONTROL_STOP)
      SUB(IDMS_START,                 NULL, IDM_DATASETTE_CONTROL_START)
      SUB(IDMS_FORWARD,               NULL, IDM_DATASETTE_CONTROL_FORWARD)
      SUB(IDMS_REWIND,                NULL, IDM_DATASETTE_CONTROL_REWIND)
      SUB(IDMS_RECORD,                NULL, IDM_DATASETTE_CONTROL_RECORD)
      SUB(IDMS_RESET,                 NULL, IDM_DATASETTE_CONTROL_RESET)
      SUB(IDMS_RESET_COUNTER,         NULL, IDM_DATASETTE_RESET_COUNTER)
    ITEMSEPARATOR()
    ITEMTOGGLE(IDMS_PAUSE,            NULL, IDM_PAUSE)
    ITEM(IDMS_MONITOR,                "M",  IDM_MONITOR)
    ITEM(IDMS_RESET,                  NULL, NULL)
      SUB(IDMS_HARD,                  NULL, IDM_RESET_HARD)
      SUB(IDMS_SOFT,                  "R",  IDM_RESET_SOFT)
      SUBSEPARATOR()
      SUB(IDMS_DRIVE_8,               NULL, IDM_RESET_DRIVE8)
      SUB(IDMS_DRIVE_9,               NULL, IDM_RESET_DRIVE9)
      SUB(IDMS_DRIVE_10,              NULL, IDM_RESET_DRIVE10)
      SUB(IDMS_DRIVE_11,              NULL, IDM_RESET_DRIVE11)
    ITEMSEPARATOR()
    ITEM(IDMS_EXIT,                   "X",  IDM_EXIT)

  TITLE(IDMS_EDIT, NULL)
    ITEM(IDMS_COPY,  NULL, IDM_COPY)
    ITEM(IDMS_PASTE, NULL, IDM_PASTE)

  TITLE(IDMS_SNAPSHOT, NULL)
    ITEM(IDMS_LOAD_SNAPSHOT_IMAGE,           NULL, IDM_SNAPSHOT_LOAD)
    ITEM(IDMS_SAVE_SNAPSHOT_IMAGE,           NULL, IDM_SNAPSHOT_SAVE)
    ITEMSEPARATOR()
    ITEM(IDMS_START_STOP_RECORDING,          NULL, IDM_EVENT_TOGGLE_RECORD)
    ITEM(IDMS_START_STOP_PLAYBACK,           NULL, IDM_EVENT_TOGGLE_PLAYBACK)
    ITEM(IDMS_SET_MILESTONE,                 "G",  IDM_EVENT_SETMILESTONE)
    ITEM(IDMS_RETURN_TO_MILESTONE,           "H",  IDM_EVENT_RESETMILESTONE)
    ITEM(IDMS_RECORDING_START_MODE,          NULL, NULL)
      SUBTOGGLE(IDMS_SAVE_NEW_SNAPSHOT,      NULL, IDM_EVENT_START_MODE_SAVE)
      SUBTOGGLE(IDMS_LOAD_EXISTING_SNAPSHOT, NULL, IDM_EVENT_START_MODE_LOAD)
      SUBTOGGLE(IDMS_START_WITH_RESET,       NULL, IDM_EVENT_START_MODE_RESET)
      SUBTOGGLE(IDMS_OVERWRITE_PLAYBACK,     NULL, IDM_EVENT_START_MODE_PLAYBACK)
    ITEM(IDMS_SELECT_HISTORY_DIR,            NULL, IDM_EVENT_DIRECTORY)
    ITEMSEPARATOR()
    ITEM(IDMS_SAVE_STOP_MEDIA_FILE,          "C",  IDM_MEDIAFILE)
    ITEMSEPARATOR()
    ITEM(IDMS_START_SOUND_RECORD,            NULL, IDM_SOUND_RECORD_START)
    ITEM(IDMS_STOP_SOUND_RECORD,             NULL, IDM_SOUND_RECORD_STOP)
    ITEMSEPARATOR()

  TITLE(IDMS_OPTIONS, NULL)
    ITEM(IDMS_REFRESH_RATE,               NULL, NULL)
      SUBTOGGLE(IDMS_AUTO,                NULL, IDM_REFRESH_RATE_AUTO)
      SUBTOGGLE(IDMS_1_1,                 NULL, IDM_REFRESH_RATE_1)
      SUBTOGGLE(IDMS_1_2,                 NULL, IDM_REFRESH_RATE_2)
      SUBTOGGLE(IDMS_1_3,                 NULL, IDM_REFRESH_RATE_3)
      SUBTOGGLE(IDMS_1_4,                 NULL, IDM_REFRESH_RATE_4)
      SUBTOGGLE(IDMS_1_5,                 NULL, IDM_REFRESH_RATE_5)
      SUBTOGGLE(IDMS_1_6,                 NULL, IDM_REFRESH_RATE_6)
      SUBTOGGLE(IDMS_1_7,                 NULL, IDM_REFRESH_RATE_7)
      SUBTOGGLE(IDMS_1_8,                 NULL, IDM_REFRESH_RATE_8)
      SUBTOGGLE(IDMS_1_9,                 NULL, IDM_REFRESH_RATE_9)
      SUBTOGGLE(IDMS_1_10,                NULL, IDM_REFRESH_RATE_10)
  ITEM(IDMS_MAXIMUM_SPEED,                NULL, NULL)
      SUBTOGGLE(IDMS_200_PERCENT,         NULL, IDM_MAXIMUM_SPEED_200)
      SUBTOGGLE(IDMS_100_PERCENT,         NULL, IDM_MAXIMUM_SPEED_100)
      SUBTOGGLE(IDMS_50_PERCENT,          NULL, IDM_MAXIMUM_SPEED_50)
      SUBTOGGLE(IDMS_20_PERCENT,          NULL, IDM_MAXIMUM_SPEED_20)
      SUBTOGGLE(IDMS_10_PERCENT,          NULL, IDM_MAXIMUM_SPEED_10)
      SUBTOGGLE(IDMS_NO_LIMIT,            NULL, IDM_MAXIMUM_SPEED_NO_LIMIT)
      SUBSEPARATOR()
      SUBTOGGLE(IDMS_CUSTOM,              NULL, IDM_MAXIMUM_SPEED_CUSTOM)
    ITEMTOGGLE(IDMS_WARP_MODE,            "W",  IDM_TOGGLE_WARP_MODE)
    ITEMSEPARATOR()
    ITEMTOGGLE(IDMS_FULLSCREEN,           "D", IDM_TOGGLE_FULLSCREEN)
    ITEMTOGGLE(IDMS_FULLSCREEN_STATUSBAR, NULL, IDM_TOGGLE_STATUSBAR)
    ITEMSEPARATOR()
    ITEMTOGGLE(IDMS_VIDEO_CACHE,          NULL, IDM_TOGGLE_VIDEOCACHE)
    ITEMTOGGLE(IDMS_DOUBLE_SIZE,          NULL, IDM_TOGGLE_DOUBLESIZE)
    ITEMTOGGLE(IDMS_DOUBLE_SCAN,          NULL, IDM_TOGGLE_DOUBLESCAN)
#if defined(HAVE_PROTO_CYBERGRAPHICS_H) && defined(HAVE_XVIDEO)
    ITEMTOGGLE(IDMS_VIDEO_OVERLAY,        NULL, IDM_TOGGLE_OVERLAY)
#endif
    ITEMSEPARATOR()
    ITEM(IDMS_SWAP_JOYSTICKS,             "J",  IDM_SWAP_JOYSTICK)
    ITEM(IDMS_SWAP_USERPORT_JOYSTICKS,    NULL, IDM_SWAP_USERPORT_JOYSTICK)
    ITEM(IDMS_ENABLE_JOY_KEYS,            NULL, IDM_JOYKEYS_TOGGLE)
    ITEM(IDMS_OPPOSITE_JOY_DIR,           NULL, IDM_OPPOSITE_JOY_DIR)
    ITEMSEPARATOR()
    ITEMTOGGLE(IDMS_SOUND_PLAYBACK,       NULL, IDM_TOGGLE_SOUND)
    ITEMSEPARATOR()
    ITEMTOGGLE(IDMS_TRUE_DRIVE_EMU,       NULL, IDM_TOGGLE_DRIVE_TRUE_EMULATION)
    ITEMTOGGLE(IDMS_AUTOSTART_HANDLE_TDE, NULL, IDM_TOGGLE_AUTOSTART_HANDLE_TDE)
    ITEMTOGGLE(IDMS_VIRTUAL_DEVICE_TRAPS, NULL, IDM_TOGGLE_VIRTUAL_DEVICES)
    ITEM(IDMS_DRIVE_SYNC_FACTOR,          NULL, NULL)
      SUBTOGGLE(IDMS_PAL,                 NULL, IDM_SYNC_FACTOR_PAL)
      SUBTOGGLE(IDMS_NTSC,                NULL, IDM_SYNC_FACTOR_NTSC)

  TITLE(IDMS_SETTINGS, NULL)
    ITEM(IDMS_AUTOSTART_SETTINGS,         NULL, IDM_AUTOSTART_SETTINGS)
    ITEM(IDMS_VIDEO_SETTINGS,             NULL, IDM_VIDEO_SETTINGS)
    ITEM(IDMS_CBM2_SETTINGS,              NULL, IDM_CBM2_SETTINGS)
    ITEM(IDMS_PERIPHERAL_SETTINGS,        NULL, IDM_DEVICEMANAGER)
    ITEM(IDMS_DRIVE_SETTINGS,             NULL, IDM_DRIVE_SETTINGS)
    ITEM(IDMS_PRINTER_SETTINGS,           NULL, IDM_PRINTER_SETTINGS)
    ITEM(IDMS_DATASETTE_SETTINGS,         NULL, IDM_DATASETTE_SETTINGS)
    ITEM(IDMS_VICII_SETTINGS,             NULL, IDM_VICII_SETTINGS)
#ifdef AMIGA_OS4
    ITEM(IDMS_JOYSTICK_SETTINGS,          NULL, IDM_JOY_SETTINGS)
#else
    ITEM(IDMS_JOYSTICK_SETTINGS,          NULL, NULL)
      SUB(IDMS_JOYSTICK_DEVICE_SELECT,    NULL, IDM_JOY_DEVICE_SELECTION)
      SUB(IDMS_JOYSTICK_FIRE_SELECT,      NULL, IDM_JOY_FIRE_SELECTION)
#endif
    ITEM(IDMS_KEYBOARD_SETTINGS,          NULL, IDM_KEYBOARD_SETTINGS)
    ITEM(IDMS_SOUND_SETTINGS,             NULL, IDM_SOUND_SETTINGS)
    ITEM(IDMS_SID_SETTINGS,               NULL, IDM_SID_SETTINGS)
    ITEM(IDMS_COMPUTER_ROM_SETTINGS,      NULL, IDM_COMPUTER_ROM_SETTINGS)
    ITEM(IDMS_DRIVE_ROM_SETTINGS,         NULL, IDM_DRIVE_ROM_SETTINGS)
    ITEM(IDMS_RAM_SETTINGS,               NULL, IDM_RAM_SETTINGS)
    ITEM(IDMS_RS232_SETTINGS,             NULL, IDM_RS232_SETTINGS)
    ITEM(IDMS_CART_IO_SETTINGS,           NULL, NULL)
      SUB(IDMS_ACIA_SETTINGS,             NULL, IDM_ACIA_SETTINGS)
    ITEMSEPARATOR()
    ITEM(IDMS_SAVE_CURRENT_SETTINGS_FILE, NULL, IDM_SETTINGS_SAVE_FILE)
    ITEM(IDMS_LOAD_SAVED_SETTINGS_FILE,   NULL, IDM_SETTINGS_LOAD_FILE)
    ITEM(IDMS_SAVE_CURRENT_SETTINGS,      NULL, IDM_SETTINGS_SAVE)
    ITEM(IDMS_LOAD_SAVED_SETTINGS,        NULL, IDM_SETTINGS_LOAD)
    ITEM(IDMS_SET_DEFAULT_SETTINGS,       NULL, IDM_SETTINGS_DEFAULT)
    ITEMSEPARATOR()
    ITEMTOGGLE(IDMS_SAVE_SETTING_ON_EXIT, NULL, IDM_TOGGLE_SAVE_SETTINGS_ON_EXIT)
    ITEMTOGGLE(IDMS_CONFIRM_ON_EXIT,      NULL, IDM_TOGGLE_CONFIRM_ON_EXIT)

  TITLE(IDMS_LANGUAGE, NULL)
    ITEM(IDMS_LANGUAGE_ENGLISH,   NULL, IDM_LANGUAGE_ENGLISH)
    ITEM(IDMS_LANGUAGE_DANISH,    NULL, IDM_LANGUAGE_DANISH)
    ITEM(IDMS_LANGUAGE_GERMAN,    NULL, IDM_LANGUAGE_GERMAN)
    ITEM(IDMS_LANGUAGE_FRENCH,    NULL, IDM_LANGUAGE_FRENCH)
    ITEM(IDMS_LANGUAGE_HUNGARIAN, NULL, IDM_LANGUAGE_HUNGARIAN)
    ITEM(IDMS_LANGUAGE_ITALIAN,   NULL, IDM_LANGUAGE_ITALIAN)
    ITEM(IDMS_LANGUAGE_KOREAN,    NULL, IDM_LANGUAGE_KOREAN)
    ITEM(IDMS_LANGUAGE_DUTCH,     NULL, IDM_LANGUAGE_DUTCH)
    ITEM(IDMS_LANGUAGE_POLISH,    NULL, IDM_LANGUAGE_POLISH)
    ITEM(IDMS_LANGUAGE_RUSSIAN,   NULL, IDM_LANGUAGE_RUSSIAN)
    ITEM(IDMS_LANGUAGE_SWEDISH,   NULL, IDM_LANGUAGE_SWEDISH)
    ITEM(IDMS_LANGUAGE_TURKISH,   NULL, IDM_LANGUAGE_TURKISH)

  TITLE(IDMS_HELP, NULL)
    ITEM(IDMS_ABOUT,                NULL, IDM_ABOUT)
    ITEMSEPARATOR()
    ITEM(IDMS_COMMAND_LINE_OPTIONS, NULL, IDM_CMDLINE)
    ITEMSEPARATOR()
    ITEM(IDMS_CONTRIBUTORS,         NULL, IDM_CONTRIBUTORS)
    ITEM(IDMS_LICENSE,              NULL, IDM_LICENSE)
    ITEM(IDMS_NO_WARRANTY,          NULL, IDM_WARRANTY)
  END()
};

static struct NewMenu UI_TRANSLATED_MENU_NAME[sizeof(UI_MENU_NAME)/sizeof(UI_MENU_NAME[0])];

#endif /* VICE_CBM2UIRES_H_ */
