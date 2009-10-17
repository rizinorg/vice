/*
 * viceglview.h - VICEGLView
 *
 * Written by
 *  Christian Vogelgsang <chris@vogelgsang.org>
 *  Michael Klein <michael.klein@puffin.lb.shuttle.de>
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

#include "video.h"
#include "videoparam.h"

#import <Cocoa/Cocoa.h>
#import <CoreVideo/CVDisplayLink.h>

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>

struct video_param_s;

#define MOUSE_HIDE_DELAY 4
#define MOUSE_IS_HIDDEN  -1
#define MOUSE_IS_SHOWN   -2

#define MAX_BUFFERS     8

#define NUM_KEY_MODIFIERS 32

struct texture_s {
    BYTE            *buffer;            /* raw data of texture */
    GLuint           bindId;            /* GL ID for binding */
    unsigned long    timeStamp;         /* when the machine wrote into buffer */
    int              frameNo;
};
typedef struct texture_s texture_t;

@interface VICEGLView : NSOpenGLView
{
    // Texture
    int         numTextures;            /* how many textures are set up? */
    NSSize      textureSize;            /* size of canvas/texture for machine drawing */
    int         textureByteSize;        /* size of texture in bytes */
    texture_t   texture[MAX_BUFFERS];   /* manage up to MAX_BUFFERS textures */

    // View
    NSSize      viewSize;               /* size of canvas view on screen (might be smaller than GL view) */
    NSPoint     viewOrigin;             /* origin of canvas view inside GL view */
    int         canvasId;               /* the canvas id assigned to this view */
    
    // Keyboard
    unsigned int lastKeyModifierFlags;
    unsigned int modifierKeyCode[NUM_KEY_MODIFIERS];
    
    // Mouse
    BOOL        trackMouse;
    float       mouseXScale;
    float       mouseYScale;
    int         oldX;
    int         oldY;
    NSTimer *   mouseHideTimer;
    int         mouseHideInterval;
    
    // OpenGL
    NSRecursiveLock *glLock;
    BOOL             isOpenGLReady;
    BOOL             postponedReconfigure;
    struct video_param_s video_param;   /* a copy of the current params */

    // DisplayLink
    CVDisplayLinkRef displayLink;
    BOOL             displayLinkEnabled;  /* display link is running */
    BOOL             displayLinkSynced;   /* display link delivers valid refresh rate */
    float            screenRefreshPeriod; /* refresh rate of screen */

    // MultiBuffer (size is "numTextures")
    BOOL             multiBufferEnabled;/* flag to enable multi buffer */
    BOOL             overwriteBuffer;
    int              syncWritePos;
    int              writePos;          /* position in ring buffer where to write to */ 
    int              displayPos;        /* position in ring buffer where to display from */
    int              numDrawn;
    unsigned long    lockTime;
    unsigned long    drawDisplayDelta; 
    float            blendAlpha;
    unsigned long    firstDrawTime;
    unsigned long    lastDrawTime;
}

// ----- interface -----

- (id)initWithFrame:(NSRect)rect;
- (void)dealloc;

// the canvas was reconfigured i.e. new parameters were set
- (void)reconfigure:(struct video_param_s *)vieo_param;

// the size of the canvas changed -> adapt textures and resources to new size
- (void)resize:(NSSize)size;

// get next render buffer for drawing by emu. may return NULL if out of buffers
- (BYTE *)beginMachineDraw:(unsigned long)timeStamp frame:(int)frameNo;

// end rendering into buffer
- (void)endMachineDraw;

// report current canvas pitch in bytes
- (int)getCanvasPitch;

// report current canvas depth in bits
- (int)getCanvasDepth;

// register the id for this canvas
- (void)setCanvasId:(int)canvasId;

// return the current canvas id assigned to this view
- (int)canvasId;

// ----- local -----

- (int)calcBlend:(unsigned long)now;
- (void)toggleBlending:(BOOL)on;

- (BOOL)setupDisplayLink;
- (void)shutdownDisplayLink;
- (float)getDisplayLinkRefreshPeriod;

- (void)initTextures;
- (void)deleteAllTextures;
- (void)setupTextures:(int)num withSize:(NSSize)size;
- (void)updateTexture:(int)i;

- (void)mouseMove:(NSPoint)pos;

@end

