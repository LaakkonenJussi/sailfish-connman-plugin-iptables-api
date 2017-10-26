/*
 *
 *  Connection Manager Sailfish iptables API plugin
 *
 *  Copyright (C) 2017 Jolla Ltd.
 *  Contact: Jussi Laakkonen <jussi.laakkonen@jolla.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#define CONNMAN_API_SUBJECT_TO_CHANGE
#define PLUGIN_NAME "SAILFISH_IPTABLES_API"

#include "sailfish-iptables-api.h"

#define ERR(fmt,arg...) connman_error(fmt, ## arg)
#define INFO(fmt,arg...) connman_info(fmt, ## arg)

static gboolean sailfish_iptables_api_save_firewall(const char* path)
{
	INFO("%s %s", PLUGIN_NAME, "SAVE");
	connman_iptables_commit("filter");
	return true;
}

static gboolean sailfish_iptables_api_load_firewall(const char* path)
{
	INFO("%s %s", PLUGIN_NAME, "LOAD");
	return true;
}

static gboolean sailfish_iptables_api_clear_firewall()
{
	INFO("%s %s", PLUGIN_NAME, "CLEAR");
	connman_iptables_cleanup();
	return true;
}

DBusMessage* sailfish_iptables_save_firewall(DBusConnection *connection,
			DBusMessage *message, void *user_data)
{
	const char* path = NULL;
	dbus_uint32_t result = false;
	
	if(dbus_message_get_args(message, NULL,
		DBUS_TYPE_STRING, &path,
		DBUS_TYPE_INVALID))
	{
		if((result = sailfish_iptables_api_save_firewall(path)))
		{
			DBusMessage *signal = sailfish_iptables_signal(
					SAILFISH_IPTABLES_API_SIGNAL_SAVE,NULL);
			if(signal)
				sailfish_iptables_send_signal(signal);
		}
	}
	
	DBusMessage* reply = dbus_message_new_method_return(message);
	DBusMessageIter iter;
	dbus_message_iter_init_append(reply,&iter);
	
	if(!dbus_message_iter_append_basic(&iter,
		DBUS_TYPE_BOOLEAN,
		&result))
	{
		dbus_message_unref(reply);
		reply = g_dbus_create_error(message,DBUS_ERROR_NO_MEMORY,
			"failed to add parameter to reply.");
	}

	return reply;
}
					
DBusMessage* sailfish_iptables_load_firewall(DBusConnection *connection,
			DBusMessage *message, void *user_data)
{
	const char* path = NULL;
	dbus_bool_t result = false;
	
	if(dbus_message_get_args(message, NULL,
		DBUS_TYPE_STRING, &path,
		DBUS_TYPE_INVALID))
	{
		if((result = sailfish_iptables_api_load_firewall(path)))
		{
			DBusMessage *signal = sailfish_iptables_signal(
					SAILFISH_IPTABLES_API_SIGNAL_LOAD,NULL);
			if(signal)
				sailfish_iptables_send_signal(signal);
		}
	}
	
	DBusMessage* reply = dbus_message_new_method_return(message);
	DBusMessageIter iter;
	dbus_message_iter_init_append(reply,&iter);
	
	if(!dbus_message_iter_append_basic(&iter,
		DBUS_TYPE_BOOLEAN,
		&result))
	{
		dbus_message_unref(reply);
		reply = g_dbus_create_error(message,DBUS_ERROR_NO_MEMORY,
			"failed to add parameter to reply.");
	}

	return reply;
}

DBusMessage* sailfish_iptables_clear_firewall(DBusConnection *connection,
			DBusMessage *message, void *user_data)
{
	dbus_bool_t result = sailfish_iptables_api_clear_firewall();
		
	DBusMessage *signal = sailfish_iptables_signal(
					SAILFISH_IPTABLES_API_SIGNAL_CLEAR,NULL);
	if(signal)
		sailfish_iptables_send_signal(signal);
		
	DBusMessage* reply = dbus_message_new_method_return(message);
	DBusMessageIter iter;
	dbus_message_iter_init_append(reply,&iter);
	
	if(!dbus_message_iter_append_basic(&iter,
		DBUS_TYPE_BOOLEAN,
		&result))
	{
		dbus_message_unref(reply);
		reply = g_dbus_create_error(message,DBUS_ERROR_NO_MEMORY,
			"failed to add parameter to reply.");
	}

	return reply;
}

DBusMessage* sailfish_iptables_version(DBusConnection *connection,
			DBusMessage *message, void *user_data)
{
	DBusMessage* reply = dbus_message_new_method_return(message);
	DBusMessageIter iter;
	dbus_message_iter_init_append(reply,&iter);
	
	dbus_int32_t version = 1;
	
	dbus_message_iter_append_basic(&iter,
		DBUS_TYPE_INT32,
		&version);

	return reply;
}

DBusMessage* sailfish_iptables_ban_v4address(DBusConnection *connection,
		DBusMessage *message, void *user_data)
{
	DBusMessage* reply = dbus_message_new_method_return(message);
	DBusMessageIter iter;
	dbus_message_iter_init_append(reply,&iter);
	
	dbus_bool_t result = true;
	
	dbus_message_iter_append_basic(&iter,
		DBUS_TYPE_BOOLEAN,
		&result);

	return reply;
}

DBusMessage* sailfish_iptables_unban_v4address(DBusConnection *connection,
		DBusMessage *message, void *user_data)
{
	DBusMessage* reply = dbus_message_new_method_return(message);
	DBusMessageIter iter;
	dbus_message_iter_init_append(reply,&iter);
	
	dbus_bool_t result = true;
	
	dbus_message_iter_append_basic(&iter,
		DBUS_TYPE_BOOLEAN,
		&result);

	return reply;
}

static int sailfish_ipt_api_init(void)
{
	INFO("%s %s", PLUGIN_NAME, "INITIALIZE IPTABLES API");
	
	int err = sailfish_iptables_api_dbus_register();
	
	if(err < 0)
		INFO("%s %s", PLUGIN_NAME, "CANNOT REGISTER TO DBUS");
	else
		INFO("%s %s", PLUGIN_NAME, "REGISTER TO DBUS SUCCESS!");
	
	return 0;
}

static void sailfish_ipt_api_exit(void)
{
	INFO("%s %s", PLUGIN_NAME, "EXIT IPTABLES API");
	
	sailfish_iptables_api_dbus_unregister();
}

CONNMAN_PLUGIN_DEFINE(sailfish_ipt_api, "Sailfish iptables API", CONNMAN_VERSION,
	CONNMAN_PLUGIN_PRIORITY_DEFAULT, sailfish_ipt_api_init,
	sailfish_ipt_api_exit)

/*
 * Local Variables:
 * mode: C
 * c-basic-offset: 8
 * indent-tabs-mode: t
 * End:
 */
