/* $Id$ */

/*  Copyright 2008 Fabian Nowak (timystery@arcor.de)
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

#include <gtk/gtk.h>

#include <libxfce4util/libxfce4util.h>

#include <stdio.h>

#include <string.h>

/* #ifdef HAVE_CONFIG_H */
 #include <config.h>
/* #endif */

#include <sensors-interface-common.h>
#include <sensors-interface.h>
#include <types.h>

void
print_usage ()
{
    printf (_("Xfce4 Sensors %s\n"
                      "Displays information about your sensors and ACPI.\n"
                      "Synopsis: \n"
                      "  xfce4-sensors options\n"
                      "where options are one or more of the following:\n"
                      "  -h, --help Print this help dialog.\n"
                      "\n"
                      "This program is published under the GPL v2.\n"), PACKAGE_VERSION
                );
}

int
main (int argc, char **argv)
{
    t_sensors_dialog *sd;
    GtkWidget *dlg, *header, *vbox, *notebook;
    t_sensors *sensors;

    if ( argc > 1 && (strcmp(argv[1], "--help")==0 || strcmp(argv[1], "-h")==0) )
    {
        print_usage ();
        return 0;
    }

    /* initialize */
    sensors = sensors_new (NULL);
    sd->sensors = sensors;

    sd = g_new0 (t_sensors_dialog, 1);

    /* start and populate */
    dlg = (GtkWidget *) gtk_dialog_new_with_buttons (_("Xfce 4 Sensors Viewer"),
                NULL, // anciently: GTK_WINDOW(gtk_get_toplevel(plugin));
                GTK_DIALOG_NO_SEPARATOR, // anciently: | GTK_DIALOG_DESTROY_WITH_PARENT
                GTK_STOCK_CLOSE, GTK_RESPONSE_OK, NULL);

    gtk_container_set_border_width (GTK_CONTAINER (dlg), 2);

    header = (GtkWidget *) xfce_create_header (NULL, _("View sensor values"));
    gtk_widget_set_size_request (GTK_BIN (header)->child, -1, 32);
    gtk_container_set_border_width (GTK_CONTAINER (header), BORDER-2);
    gtk_widget_show (header);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dlg)->vbox), header,
                        FALSE, TRUE, 0);

    vbox = GTK_DIALOG (dlg)->vbox;

    sd->dialog = dlg;

    sd->myComboBox = gtk_combo_box_new_text();

    init_widgets (sd);

    gtk_combo_box_set_active (GTK_COMBO_BOX(sd->myComboBox), 0);

    notebook = gtk_notebook_new ();

    gtk_box_pack_start (GTK_BOX(vbox), notebook, TRUE, TRUE, 0);
    gtk_widget_show(notebook);

    add_sensors_frame (notebook, sd);

    /* show window and run forever */
    gtk_main();

    return 0;
}