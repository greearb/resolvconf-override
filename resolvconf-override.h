
/* -*- Mode: C; indent-tabs-mode: t -*- */

/*
 * Copyright 2013, 2017 Bastien Nocera
 * Copyright 2020 Ben Greear <greearb@candelatech.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the Gnome Library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301  USA.
 *
 * Authors: Bastien Nocera <hadess@hadess.net>
 *
 */

#ifndef __INCLUDE_RESOLVCONF_OVERRIDE
#define __INCLUDE_RESOLVCONF_OVERRIDE

void resolvconf_override_init(const char** servers, int len, int use_tcp);

#endif
