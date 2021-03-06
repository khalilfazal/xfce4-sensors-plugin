/* $Id$ */
/*  Copyright 2008-2010 Fabian Nowak (timystery@arcor.de)
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
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Note for programmers and editors: Try to use 4 spaces instead of Tab! */

#ifndef XFCE4_SENSORS_INTERFACE_H
#define XFCE4_SENSORS_INTERFACE_H

/* Gtk includes */
#include <gtk/gtk.h>

/* Local includes */
#include "sensors-interface-common.h"

/* Initializing and filling functions */
void fill_gtkTreeStore (GtkTreeStore *model, t_chip *chip, t_tempscale scale, t_sensors_dialog *sd);

void free_widgets (t_sensors_dialog *ptr_sensors_dialog);

void init_widgets (t_sensors_dialog *sd);

void reload_listbox (t_sensors_dialog *sd);

/* GUI builder functions */
void add_sensor_settings_box ( GtkWidget * vbox, t_sensors_dialog * sd);

void add_type_box (GtkWidget * vbox, t_sensors_dialog * sd);

void add_temperature_unit_box (GtkWidget *vbox, t_sensors_dialog *sd);

void add_sensors_frame (GtkWidget * notebook, t_sensors_dialog * sd);

void add_update_time_box (GtkWidget * vbox, t_sensors_dialog * sd);

/* Callbacks */
//void temperature_unit_change (GtkWidget *widget, t_sensors_dialog *sd);




#endif /* XFCE4_SENSORS_INTERFACE_H */
