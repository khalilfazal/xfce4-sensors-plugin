/* $Id$ */
/*  Copyright 2004-2017 Fabian Nowak (timystery@arcor.de)
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/* Package includes */
#include <acpi.h>
#include <types.h>

/* Glib includes */
#include <glib.h>

/* Global includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>


static gchar *
strip_key_colon_spaces (gchar *ptr_string)
{
    gchar *ptr_position = ptr_string;

    /* Skip everything before the ':' */
    if (strchr(ptr_string, ':'))
    {
      while (*(ptr_position++)) {
          if (*ptr_position == ':') {
              break;
          }
      }
      ptr_position++;
    }
    /* Skip all the spaces */
    while (*ptr_position) {
        if (*ptr_position != ' ') {
            break;
        }
        ptr_position++;
    }

    return ptr_position;
}


#ifdef HAVE_SYSFS_ACPI
static void
cut_newline (gchar *ptr_string)
{
    gint index;

    for (index=0; ptr_string[index]!='\0'; index++)
    {
        if (ptr_string[index]=='\n')
        {
            ptr_string[index] = '\0';
            break;
        }
    }
}
#endif


gint
read_thermal_zone (t_chip *ptr_chip)
{
    gint res_value = -2;
    DIR *ptr_directory;
    FILE *ptr_file;
    gchar *str_filename;
    struct dirent *ptr_dirent;
    t_chipfeature *ptr_chipfeature;
#ifdef HAVE_SYSFS_ACPI
    gchar buffer[1024];
#else
    gchar *str_zone;
#endif

    TRACE ("enters read_thermal_zone");

#ifdef HAVE_SYSFS_ACPI
    if ((chdir ("/sys/class/") == 0) && (chdir ("thermal/") == 0))
#else
    if ((chdir (ACPI_PATH) == 0) && (chdir (ACPI_DIR_THERMAL) == 0))
#endif
    {
        ptr_directory = opendir (".");
        if (!ptr_directory) {
            closedir (ptr_directory);
            res_value = -1;
        }
        else
        {
            while ((ptr_dirent = readdir (ptr_directory)))
            {
                if (strncmp(ptr_dirent->d_name, ".", 1)==0)
                    continue;

    #ifdef HAVE_SYSFS_ACPI
                str_filename = g_strdup_printf ("/sys/class/thermal/%s/temp", ptr_dirent->d_name);
    #else
                str_filename = g_strdup_printf ("%s/%s/%s/%s", ACPI_PATH,
                                            ACPI_DIR_THERMAL, ptr_dirent->d_name,
                                            ACPI_FILE_THERMAL);
    #endif
                ptr_file = fopen (str_filename, "r");
                if (ptr_file)
                {
                    DBG("parsing temperature file \"%s\"...\n", str_filename);
                    /* if (acpi_ignore_directory_entry (ptr_dirent))
                        continue; */

                    ptr_chipfeature = g_new0 (t_chipfeature, 1);

                    ptr_chipfeature->color = g_strdup("#0000B0");
                    ptr_chipfeature->address = ptr_chip->chip_features->len;
                    ptr_chipfeature->devicename = g_strdup (ptr_dirent->d_name);
                    ptr_chipfeature->name = g_strdup (ptr_chipfeature->devicename);
                    ptr_chipfeature->formatted_value = NULL; /*  Gonna refresh it in
                                                            sensors_get_wrapper or some
                                                            other functions */

    #ifdef HAVE_SYSFS_ACPI
                    if (fgets (buffer, 1024, ptr_file)!=NULL)
                    {
                        cut_newline (buffer);
                        ptr_chipfeature->raw_value = strtod (buffer, NULL) / 1000.0;
                        DBG ("Raw-Value=%f\n", ptr_chipfeature->raw_value);
                    }
    #else
                    str_zone = g_strdup_printf ("%s/%s", ACPI_DIR_THERMAL, ptr_dirent->d_name);
                    ptr_chipfeature->raw_value = get_acpi_zone_value (str_zone, ACPI_FILE_THERMAL);
                    g_free (str_zone);
    #endif

                    ptr_chipfeature->valid = TRUE;
                    ptr_chipfeature->min_value = 20.0;
                    ptr_chipfeature->max_value = 60.0;
                    ptr_chipfeature->class = TEMPERATURE;

                    g_ptr_array_add (ptr_chip->chip_features, ptr_chipfeature);

                    ptr_chip->num_features++; /* FIXME: actually I am just the same as
                        chip->chip_features->len */

                    fclose(ptr_file);
                }

                g_free (str_filename);
            } /* while */

            closedir (ptr_directory);
            TRACE ("leaves read_thermal_zone");

            res_value = 0;
        } /* if */
    }
    else {
        TRACE ("leaves read_thermal_zone");
    }

    return res_value;
}


gdouble
get_fan_zone_value (gchar *str_zonename)
{
    gdouble res_value = 0.0;

    FILE *ptr_file;
    gchar buffer [1024], *str_filename, *ptr_strippedbuffer;

    TRACE ("enters get_fan_zone_value for %s", str_zonename);

    str_filename = g_strdup_printf ("%s/%s/%s/%s", ACPI_PATH, ACPI_DIR_FAN,
                                str_zonename, ACPI_FILE_FAN);
    DBG("filename=%s", str_filename);
    ptr_file = fopen (str_filename, "r");
    if (ptr_file) {
        while (fgets (buffer, 1024, ptr_file)!=NULL)
        {
            if (strncmp (buffer, "status:", 7)==0)
            {
                ptr_strippedbuffer = strip_key_colon_spaces(buffer);
                DBG ("tmp=%s", ptr_strippedbuffer);
                if (strncmp (ptr_strippedbuffer, "on", 2)==0)
                    res_value = 1.0;
                else
                    res_value = 0.0;

                break;
            }
        }
        fclose (ptr_file);
    }

    g_free (str_filename);

    return res_value;
}


gdouble
get_battery_zone_value (gchar *str_zone)
{
    gdouble res_value = 0.0;

    FILE *ptr_file;
    gchar buffer [1024], *str_filename;

#ifndef HAVE_SYSFS_ACPI
    gchar *ptr_strippedbuffer;
#endif

    TRACE ("enters get_battery_zone_value for %s", str_zone);

#ifdef HAVE_SYSFS_ACPI
    str_filename = g_strdup_printf ("/sys/class/power_supply/%s/energy_now", str_zone);
#else
    str_filename = g_strdup_printf ("%s/%s/%s/%s", ACPI_PATH, ACPI_DIR_BATTERY,
                                str_zone, ACPI_FILE_BATTERY_STATE);
#endif
    DBG("str_filename=%s\n", str_filename);
    ptr_file = fopen (str_filename, "r");
    if (ptr_file) {
#ifdef HAVE_SYSFS_ACPI
        if (fgets (buffer, 1024, ptr_file)!=NULL)
        {
            cut_newline (buffer);
            res_value = strtod (buffer, NULL) / 1000.0;
        }
#else
        while (fgets (buffer, 1024, ptr_file)!=NULL)
        {
            if (strncmp (buffer, "remaining capacity:", 19)==0)
            {
                ptr_strippedbuffer = strip_key_colon_spaces(buffer);
                res_value = strtod (ptr_strippedbuffer, NULL);
                break;
            }
        }
#endif
        fclose (ptr_file);
    }

    g_free (str_filename);

    return res_value;
}


gint
read_battery_zone (t_chip *ptr_chip)
{
    gint res_value = -1;
    DIR *ptr_dir;
    FILE *ptr_file;
    gchar *str_filename;
#ifndef HAVE_SYSFS_ACPI
    gchar *ptr_strippedbuffer;
#endif
    gchar buffer[1024];
    struct dirent *ptr_dirent;
    t_chipfeature *ptr_chipfeature = NULL;

    TRACE ("enters read_battery_zone");

#ifdef HAVE_SYSFS_ACPI
    if ((chdir ("/sys/class") == 0) && (chdir ("power_supply") == 0)) {
#else
    if ((chdir (ACPI_PATH) == 0) && (chdir (ACPI_DIR_BATTERY) == 0)) {
#endif
        ptr_dir = opendir (".");

        while (ptr_dir && (ptr_dirent = readdir (ptr_dir)))
        {
            if (strncmp(ptr_dirent->d_name, "BAT", 3)==0)
            { /* have a battery subdirectory */

#ifdef HAVE_SYSFS_ACPI
                str_filename = g_strdup_printf ("/sys/class/power_supply/%s/model_name", ptr_dirent->d_name);
#else
                str_filename = g_strdup_printf ("%s/%s/%s/%s", ACPI_PATH,
                                            ACPI_DIR_BATTERY, ptr_dirent->d_name,
                                            ACPI_FILE_BATTERY_STATE);
#endif
                DBG ("str_filename=%s\n", str_filename);
                ptr_file = fopen (str_filename, "r");
                ptr_chipfeature = g_new0 (t_chipfeature, 1);
                if (ptr_file) {
                    ptr_chipfeature->address = ptr_chip->chip_features->len;
                    ptr_chipfeature->devicename = g_strdup (ptr_dirent->d_name);

#ifdef HAVE_SYSFS_ACPI
                    if (fgets (buffer, 1024, ptr_file)!=NULL)
                    {
                        cut_newline (buffer);
                        ptr_chipfeature->name = g_strdup (buffer);
                        DBG ("Name=%s\n", buffer);
                    }
#else
                    ptr_chipfeature->name = g_strdup (ptr_chipfeature->devicename);
#endif

                    ptr_chipfeature->valid = TRUE;
                    ptr_chipfeature->min_value = 0.0;
                    ptr_chipfeature->raw_value = 0.0;
                    ptr_chipfeature->class = ENERGY;
                    ptr_chipfeature->formatted_value = NULL;
                    ptr_chipfeature->color = g_strdup("#0000B0");

#ifdef HAVE_SYSFS_ACPI
                    fclose (ptr_file);
                }
                g_free (str_filename);
                str_filename = g_strdup_printf ("/sys/class/power_supply/%s/energy_now", ptr_dirent->d_name);
                ptr_file = fopen (str_filename, "r");
                if (ptr_file) {

                    if (fgets (buffer, 1024, ptr_file)!=NULL)
                    {
                        cut_newline (buffer);
                        ptr_chipfeature->raw_value = strtod (buffer, NULL);
                        DBG ("Raw-Value=%f\n", ptr_chipfeature->raw_value);
                    }
                    fclose (ptr_file);
                }
                g_free (str_filename);
                str_filename = g_strdup_printf ("/sys/class/power_supply/%s/alarm", ptr_dirent->d_name);
                ptr_file = fopen (str_filename, "r");
                if (ptr_file) {
                    if (fgets (buffer, 1024, ptr_file)!=NULL)
                    {
                        cut_newline (buffer);
                        ptr_chipfeature->min_value = strtod (buffer, NULL) / 1000.0;
                        DBG ("Min-Value=%f\n", ptr_chipfeature->min_value);
                    }
#else
                    while (fgets (buffer, 1024, ptr_file)!=NULL)
                    {
                        if (strncmp (buffer, "design capacity low:", 20)==0)
                        {
                            ptr_strippedbuffer = strip_key_colon_spaces(buffer);
                            ptr_chipfeature->min_value = strtod (ptr_strippedbuffer, NULL);
                        }
                        else if (strncmp (buffer, "remaining capacity:", 19)==0)
                        {
                            ptr_strippedbuffer = strip_key_colon_spaces(buffer);
                            ptr_chipfeature->raw_value = strtod (ptr_strippedbuffer, NULL);
                        }
                    }
#endif

                    fclose (ptr_file);

                    g_ptr_array_add (ptr_chip->chip_features, ptr_chipfeature);
                    ptr_chip->num_features++; /* FIXME: actually I am just the same
                                            as chip->chip_features->len */
                }
                else {
                    g_free (str_filename);
                    continue; /* what would we want to do with only
                                a maxval and no real value inside? */
                }

                g_free (str_filename);

                get_battery_max_value (ptr_dirent->d_name, ptr_chipfeature);

            }

            res_value = 0;
        }

        if (ptr_dir)
            closedir (ptr_dir);

        TRACE ("leaves read_battery_zone");
    }
    else
    {
        TRACE ("leaves read_battery_zone");
        res_value = -2;
    }

    return res_value;
}


void
get_battery_max_value (gchar *str_filename, t_chipfeature *ptr_chipfeature)
{
    FILE *ptr_file;
    gchar *str_pathtofile, buffer[1024];
#ifndef HAVE_SYSFS_ACPI
    gchar *ptr_strippedbuffer;
#endif

    TRACE ("enters get_battery_max_value");

#ifdef HAVE_SYSFS_ACPI
    str_pathtofile = g_strdup_printf ("/sys/class/power_supply/%s/energy_full", str_filename);
#else
    str_pathtofile = g_strdup_printf ("%s/%s/%s/%s", ACPI_PATH,
                                            ACPI_DIR_BATTERY, str_filename,
                                            ACPI_FILE_BATTERY_INFO);
#endif
    DBG ("str_pathtofile=%s\n", str_pathtofile);
    ptr_file = fopen (str_pathtofile, "r");
    if (ptr_file)
    {
#ifdef HAVE_SYSFS_ACPI
        if (fgets (buffer, 1024, ptr_file)!=NULL)
        {
            cut_newline (buffer);
            ptr_chipfeature->max_value = strtod (buffer, NULL) / 1000.0;
            DBG ("Max-Value=%f\n", ptr_chipfeature->max_value);
        }
#else
        while (fgets (buffer, 1024, ptr_file)!=NULL)
        {
            if (strncmp (buffer, "last full capacity:", 19)==0)
            {
                ptr_strippedbuffer = strip_key_colon_spaces(buffer);
                ptr_chipfeature->max_value = strtod (ptr_strippedbuffer, NULL);
                break;
            }
        }
#endif
        fclose (ptr_file);
    }

    g_free (str_pathtofile);

    TRACE ("leaves get_battery_max_value");
}


gint
read_fan_zone (t_chip *ptr_chip)
{
    gint res_value = -1;
    DIR *ptr_dir;
    FILE *ptr_file;
    gchar *str_filename;
    struct dirent *ptr_dirent;
    t_chipfeature *ptr_chipfeature;

    TRACE ("enters read_fan_zone");

    if ((chdir (ACPI_PATH) == 0) && (chdir (ACPI_DIR_FAN) == 0))
    {
        ptr_dir = opendir (".");

        while (ptr_dir && (ptr_dirent = readdir (ptr_dir)))
        {
            if (strncmp(ptr_dirent->d_name, ".", 1)==0)
                continue;

            str_filename = g_strdup_printf ("%s/%s/%s/%s", ACPI_PATH,
                                        ACPI_DIR_FAN, ptr_dirent->d_name,
                                        ACPI_FILE_FAN);
            ptr_file = fopen (str_filename, "r");
            if (ptr_file)
            {
                /* if (acpi_ignore_directory_entry (de))
                    continue; */

                ptr_chipfeature = g_new0 (t_chipfeature, 1);

                ptr_chipfeature->color = g_strdup("#0000B0");
                ptr_chipfeature->address = ptr_chip->chip_features->len;
                ptr_chipfeature->devicename = g_strdup (ptr_dirent->d_name);
                ptr_chipfeature->name = g_strdup (ptr_chipfeature->devicename);
                ptr_chipfeature->formatted_value = NULL; /* Gonna refresh it in
                                                        sensors_get_wrapper or some
                                                        other functions */
                ptr_chipfeature->raw_value = get_fan_zone_value (ptr_dirent->d_name);

                ptr_chipfeature->valid = TRUE;
                ptr_chipfeature->min_value = 0.0;
                ptr_chipfeature->max_value = 2.0;
                ptr_chipfeature->class = STATE;

                g_ptr_array_add (ptr_chip->chip_features, ptr_chipfeature);

                ptr_chip->num_features++; /* FIXME: actually I am just the same as
                    chip->chip_features->len */

                fclose(ptr_file);
            }

            g_free (str_filename);
            res_value = 0;
        }

        if (ptr_dir)
            closedir (ptr_dir);

        TRACE ("leaves read_fan_zone");
    }
    else {
        TRACE ("leaves read_fan_zone");
        res_value = -2;
    }
    return res_value;
}


gint
initialize_ACPI (GPtrArray *arr_ptr_chips)
{
    t_chip *ptr_chip;
    sensors_chip_name *ptr_chipname_tmp;
    gchar *str_acpi_info;
    TRACE ("enters initialize_ACPI");

    ptr_chip = g_new0 (t_chip, 1);
    ptr_chip->name = g_strdup(_("ACPI")); /* to be displayed */
    str_acpi_info = get_acpi_info();
    ptr_chip->description = g_strdup_printf (_("ACPI v%s zones"), str_acpi_info);
    g_free(str_acpi_info);
    ptr_chip->sensorId = g_strdup ("ACPI"); /* used internally */

    ptr_chip->type = ACPI;

    ptr_chipname_tmp = g_new0 (sensors_chip_name, 1);
    ptr_chipname_tmp->prefix = g_strdup(_("ACPI"));
    ptr_chipname_tmp->path = g_strdup(_("ACPI"));

    ptr_chip->chip_name = (sensors_chip_name *) ptr_chipname_tmp;

    ptr_chip->chip_features = g_ptr_array_new ();

    ptr_chip->num_features = 0;

    read_battery_zone (ptr_chip);
    read_thermal_zone (ptr_chip);
    read_fan_zone (ptr_chip);

    g_ptr_array_add (arr_ptr_chips, ptr_chip);

    TRACE ("leaves initialize_ACPI");

    return 4;
}


void
refresh_acpi (gpointer ptr_chipfeature, gpointer ptr_unused)
{
    gchar *str_filename, *str_zone, *str_state;
    t_chipfeature *cf;

#ifdef HAVE_SYSFS_ACPI
    FILE *ptr_file = NULL;
    gchar buffer[1024];
#endif

    TRACE ("enters refresh_acpi");

    g_assert(ptr_chipfeature!=NULL);

    cf = (t_chipfeature *) ptr_chipfeature;

    switch (cf->class) {
        case TEMPERATURE:
#ifdef HAVE_SYSFS_ACPI
            str_zone = g_strdup_printf ("/sys/class/thermal_zone/%s/temp", cf->devicename);
            ptr_file = fopen(str_zone, "r");
            if (ptr_file)
            {
              if (fgets (buffer, 1024, ptr_file))
              {
                cut_newline(buffer);
                cf->raw_value = strtod(buffer, NULL) / 1000.0;
              }
              fclose (ptr_file);
            }
#else
            str_zone = g_strdup_printf ("%s/%s", ACPI_DIR_THERMAL, cf->devicename);
            cf->raw_value = get_acpi_zone_value (str_zone, ACPI_FILE_THERMAL);
#endif
            g_free (str_zone);
            /* g_free (cf->formatted_value);
            cf->formatted_value = g_strdup_printf (_("%+5.1f °C"), cf->raw_value); */
            break;

        case ENERGY:
            /* zone = g_strdup_printf ("%s/%s", ACPI_DIR_BATTERY, cf->devicename); */
            cf->raw_value = get_battery_zone_value (cf->devicename); /* zone */
            /* g_free (zone); */
            /*  g_free (cf->formatted_value);
            cf->formatted_value = g_strdup_printf (_("%.0f mWh"), cf->raw_value); */
            break;

        case STATE:
            str_filename = g_strdup_printf ("%s/%s/%s/state", ACPI_PATH, ACPI_DIR_FAN, cf->devicename);

            str_state = get_acpi_value(str_filename); // return value is strdup'ped
            if (!str_state)
            {
                DBG("Could not determine fan state.");
                cf->raw_value = 0.0;
            }
            else
            {
                cf->raw_value = strncmp(str_state, "on", 2)==0 ? 1.0 : 0.0;
                g_free (str_state);
            }
            g_free (str_filename);

            /* g_free (cf->formatted_value);
            cf->formatted_value = g_strdup_printf (_("%.0f"), cf->raw_value); */
            break;

        default:
            printf ("Unknown ACPI type. Please check your ACPI installation "
                    "and restart the plugin.\n");
    }

    TRACE ("leaves refresh_acpi");
}


gint
acpi_ignore_directory_entry (struct dirent *ptr_dirent)
{
    TRACE ("enters and leaves acpi_ignore_directory_entry");

    return strcmp (ptr_dirent->d_name, "temperature");
}


/**
 * Obtains ACPI version information.
 * Might forget some space or tab bytes due to g_strchomp
 **/
gchar *
get_acpi_info (void)
{
    gchar *str_filename, *str_version;

    TRACE ("enters get_acpi_info");

    str_filename = g_strdup_printf ("%s/%s", ACPI_PATH, ACPI_INFO);
    str_version = get_acpi_value (str_filename);
    g_free (str_filename);

    if (!str_version)
    {
        str_filename = g_strdup_printf ("%s/%s_", ACPI_PATH, ACPI_INFO);
        str_version = get_acpi_value (str_filename);
        g_free (str_filename);

        if (!str_version)
            str_version = get_acpi_value ("/sys/module/acpi/parameters/acpica_str_version");
    }

    /* who knows, if we obtain non-NULL str_version at all... */
    if (str_version)
        str_version = g_strchomp (str_version);
    else
        str_version = g_strdup(_("<Unknown>"));

    TRACE ("leaves get_acpi_info");

    return str_version;
}


/* Note that zone will have to consist of two paths, e.g.
 *  thermal_zone and THRM.
 */
gdouble
get_acpi_zone_value (gchar *str_zone, gchar *str_filename)
{
    gchar *str_localfilename, *str_value;
    gdouble res_value = 0.0;

    TRACE ("enters get_acpi_zone_value for %s/%s", str_zone, str_filename);

    str_localfilename = g_strdup_printf ("%s/%s/%s", ACPI_PATH, str_zone, str_filename);
    str_value = get_acpi_value (str_localfilename);
    g_free(str_localfilename);

    TRACE ("leaves get_acpi_zone_value with correctly converted value");

    /* Return it as a double */
    if (str_value)
    {
        res_value = strtod (str_value, NULL);
        g_free (str_value);
    }

    return res_value;
}


/**
 * Get the value from inside an acpi's file.
 * @param str_filename An absolute filename, most likely starting with /proc/acpi...
 * @return value found inside as a character
 */
gchar *
get_acpi_value (gchar *str_filename)
{
    FILE *ptr_file;
    gchar buffer [1024], *ptr_valueinstring, *str_result = NULL;

    TRACE ("enters get_acpi_value for %s", str_filename);

    ptr_file = fopen (str_filename, "r");
    if (ptr_file)
    {
        fgets (buffer, 1024, ptr_file);
        fclose (ptr_file);

        ptr_valueinstring = strip_key_colon_spaces (buffer);

        TRACE ("leaves get_acpi_value with %s", ptr_valueinstring);

        str_result = g_strdup (ptr_valueinstring);
    }

    /* Have read the data */
    return str_result;
}


void
free_acpi_chip (gpointer ptr_chip)
{
    t_chip *ptr_chipcasted;

    ptr_chipcasted = (t_chip *) ptr_chip;

    if (ptr_chipcasted->chip_name->path)
        g_free (ptr_chipcasted->chip_name->path);

    if (ptr_chipcasted->chip_name->prefix)
        g_free (ptr_chipcasted->chip_name->prefix);
}
