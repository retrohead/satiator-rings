// Satiator Rings Menu

/*
** Jo Sega Saturn Engine
** Copyright (c) 2012-2017, Johannes Fetz (johannesfetz@gmail.com)
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the Johannes Fetz nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL Johannes Fetz BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

// program version, keep this length to avoid having to resize strings
#define VERSION "2.0"
#define _GNU_SOURCE
#define DEBUG

#include <jo/jo.h>
#ifdef DEBUG
#include "debug.h"
#endif
#include "sprite_manager.h"
#include "pad_controllers.h"
#include "background.h"
#include "font.h"
#include "sound.h"
#include "gui.h"
#include "options_file.h"

// program states
enum prog_state_types
{
    PROG_STATE_INITIALIZE,
    PROG_STATE_SPLASH,
    PROG_STATE_GAMELIST,
    PROG_STATE_BOOT,
    PROG_STATE_QUICKBOOT,
    PROG_STATE_MENU,
    PROG_STATE_OPTIONS,
    PROG_STATE_THEME,
    PROG_STATE_CONSOLE_INFO,
    PROG_STATE_SATIATOR_INFO,
    PROG_STATE_EXIT,
    PROG_STATE_EXIT_TO_MENU
};

extern enum prog_state_types prog_state;
extern jo_datetime dt;

// function prototypes to suppress compiler warnings
extern void *memcpy(void *dest, const void *src, unsigned int n);
extern char *strncpy(char *dest, const char *src, unsigned int n);
extern int snprintf(char *str, unsigned int size, const char *format, ...);
