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

#ifndef __CALLBACKS_H
#define __CALLBACKS_H

/* Package includes */
#include <sensors-interface-common.h>

/* Global includes */
/* #include <glib.h> */
#include <gtk/gtk.h>

/*
void sensor_entry_changed (GtkWidget *widget, t_sensors_dialog *sd);

void list_cell_text_edited (GtkCellRendererText *cellrenderertext,
                            gchar *path_str, gchar *new_text, t_sensors_dialog *sd);
*/

/* there should also be some "private" callbacks such as closing/qutting
 * the application.
 */
void close_window_requested ();

void on_main_window_response (GtkWidget *dlg, int response, t_sensors_dialog *sd);

void on_font_set (GtkWidget *widget, gpointer data);

/* Functions for implementing the sensors interface common callback functions */

void adjustment_value_changed_  (GtkWidget *widget, t_sensors_dialog *sd); // for update timer box

void sensor_entry_changed_ (GtkWidget *widget, t_sensors_dialog *sd);

void list_cell_text_edited_ (GtkCellRendererText *cellrenderertext,
                      gchar *path_str, gchar *new_text, t_sensors_dialog *sd);

void list_cell_toggle_ (GtkCellRendererToggle *cell, gchar *path_str,
                  t_sensors_dialog *sd);

void list_cell_color_edited_ (GtkCellRendererText *cellrenderertext, gchar *path_str,
                       gchar *new_color, t_sensors_dialog *sd);

void minimum_changed_ (GtkCellRendererText *cellrenderertext, gchar *path_str,
                 gchar *new_value, t_sensors_dialog *sd);

void maximum_changed_ (GtkCellRendererText *cellrenderertext, gchar *path_str,
            gchar *new_value, t_sensors_dialog *sd);

void temperature_unit_change_ (GtkWidget *widget, t_sensors_dialog *sd);


#endif /* __CALLBACKS_H */
