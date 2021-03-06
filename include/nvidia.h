/* Copyright (c) 2011 Amir Aupov <fads93@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef XFCE4_SENSORS_NVIDIA_H
#define XFCE4_SENSORS_NVIDIA_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* Gtk/Glib includes */
#include <glib.h>
#include <NVCtrl/NVCtrl.h>
/*
 * Initialize nvidia sensor 
 */
int initialize_nvidia (GPtrArray *chips);

/*
 * Refresh an nvidia chip's feature
 */
void refresh_nvidia (gpointer chip_feature, gpointer data);


double get_nvidia_value (int gpu);

#endif /* XFCE4_SENSORS_NVIDIA_H */
