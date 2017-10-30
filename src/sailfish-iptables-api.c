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


#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "sailfish-iptables-api.h"

#define ERR(fmt,arg...) connman_error(fmt, ## arg)
#define INFO(fmt,arg...) connman_info(fmt, ## arg)

static gboolean validate_ip_address(gint type, const gchar* ip)
{
	gboolean rval = false;
	if(ip && strlen(ip))
	{			
		struct addrinfo hints;
		struct addrinfo *result = NULL;
		
		memset(&hints, 0, sizeof(struct addrinfo));
		
		if (type == 6)
			hints.ai_family = AF_INET6;
		else
			hints.ai_family = AF_INET;
		hints.ai_flags = AI_NUMERICHOST;
		
		if(!getaddrinfo(ip,NULL,&hints,&result))
		{
			if(result)
				freeaddrinfo(result);
			rval = true;
		}
	}
	return rval;
}

static gboolean sailfish_iptables_api_save_firewall(const gchar* path)
{
	INFO("%s %s %s", PLUGIN_NAME, "SAVE", (path ? path : "null"));
	connman_iptables_save();
	return true;
}

static gboolean sailfish_iptables_api_load_firewall(const gchar* path)
{
	INFO("%s %s %s", PLUGIN_NAME, "LOAD", (path ? path : "null"));
	connman_iptables_restore();
	return true;
}

static gboolean sailfish_iptables_api_clear_firewall()
{
	INFO("%s %s", PLUGIN_NAME, "CLEAR");
	return true;
}

static gboolean sailfish_iptables_api_set_input_policy(const gchar* policy)
{
	gint ret = 0;
	gboolean rval = false;
	if(policy && !(ret = connman_iptables_change_policy("output","forward",policy)))
		rval = true;
		
	INFO("%s %s %s %d", PLUGIN_NAME, "CHANGE INPUT POLICY",policy,ret);
	return rval;
}

static gboolean sailfish_iptables_api_add_to_filter_table(gboolean type,
	const gchar* ip)
{	
	gboolean rval = false;
	INFO("%s %s %s", PLUGIN_NAME, (type ? "BAN" : "UNBAN"), (ip ? ip : "null"));
	
	if(ip && strlen(ip) && validate_ip_address(4,ip))
	{
		gchar* rule = g_strdup_printf("%s %s %s %s","-s",ip,"-j","DROP");
		
		gint result = 0;
		
		if(type) // ban
		{
			if(!(result = connman_iptables_append("filter","INPUT",rule)))
				INFO("%s %s", PLUGIN_NAME, "connman_iptables_append");
			else
				INFO("%s %s %s %d", PLUGIN_NAME,
					"connman_iptables_append failure", rule, result);
		}
		else //unban
		{
			if(!(result = connman_iptables_delete("filter","INPUT",rule)))
				INFO("%s %s", PLUGIN_NAME, "connman_iptables_delete success");
			else
				INFO("%s %s %s %d", PLUGIN_NAME,
					"connman_iptables_delete failure", rule, result);
		}
		
		if(result == 0 && !connman_iptables_commit("filter"))
		{
			INFO("%s %s", PLUGIN_NAME, "connman_iptables_commit");
			rval = true;
		}
		else
			INFO("%s %s", PLUGIN_NAME, "connman_iptables_commit failed");
		
		g_free(rule);
	}
	
	return rval;
}

DBusMessage* sailfish_iptables_save_firewall(DBusConnection *connection,
			DBusMessage *message, void *user_data)
{
	const gchar* path = NULL;
	dbus_uint32_t result = false;
	
	if(dbus_message_get_args(message, NULL,
		DBUS_TYPE_STRING, &path,
		DBUS_TYPE_INVALID))
	{
		if((result = sailfish_iptables_api_save_firewall(path)))
		{
			DBusMessage *signal = sailfish_iptables_signal(
					SAILFISH_IPTABLES_API_SIGNAL_SAVE,"%s", path);
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
	const gchar* path = NULL;
	dbus_bool_t result = false;
	
	if(dbus_message_get_args(message, NULL,
		DBUS_TYPE_STRING, &path,
		DBUS_TYPE_INVALID))
	{
		if((result = sailfish_iptables_api_load_firewall(path)))
		{
			DBusMessage *signal = sailfish_iptables_signal(
					SAILFISH_IPTABLES_API_SIGNAL_LOAD,"%s",path);
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
					SAILFISH_IPTABLES_API_SIGNAL_CLEAR,NULL,NULL);
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
	const gchar* ip = NULL;
	dbus_bool_t result = false;
	
	if(dbus_message_get_args(message, NULL,
		DBUS_TYPE_STRING, &ip,
		DBUS_TYPE_INVALID))
	{
		if((result = sailfish_iptables_api_add_to_filter_table(true,ip)))
		{
			DBusMessage *signal = sailfish_iptables_signal(
					SAILFISH_IPTABLES_API_SIGNAL_BAN,"%s",ip);
			if(signal)
				sailfish_iptables_send_signal(signal);
		}
	}

	DBusMessage* reply = dbus_message_new_method_return(message);
	DBusMessageIter iter;
	dbus_message_iter_init_append(reply,&iter);
	
	dbus_message_iter_append_basic(&iter,
		DBUS_TYPE_BOOLEAN,
		&result);

	return reply;
}

DBusMessage* sailfish_iptables_unban_v4address(DBusConnection *connection,
		DBusMessage *message, void *user_data)
{
	const gchar* ip = NULL;
	dbus_bool_t result = false;
	
	if(dbus_message_get_args(message, NULL,
		DBUS_TYPE_STRING, &ip,
		DBUS_TYPE_INVALID))
	{
		if((result = sailfish_iptables_api_add_to_filter_table(false,ip)))
		{
			DBusMessage *signal = sailfish_iptables_signal(
					SAILFISH_IPTABLES_API_SIGNAL_UNBAN,"%s",ip);
			if(signal)
				sailfish_iptables_send_signal(signal);
		}
	}

	DBusMessage* reply = dbus_message_new_method_return(message);
	DBusMessageIter iter;
	dbus_message_iter_init_append(reply,&iter);
	
	dbus_message_iter_append_basic(&iter,
		DBUS_TYPE_BOOLEAN,
		&result);

	return reply;
}

DBusMessage* sailfish_iptables_change_input_policy(DBusConnection *connection,
			DBusMessage *message, void *user_data)
{
	const gchar* policy = NULL;
	dbus_uint32_t result = false;
	
	if(dbus_message_get_args(message, NULL,
		DBUS_TYPE_STRING, &policy,
		DBUS_TYPE_INVALID))
	{
		if((result = sailfish_iptables_api_set_input_policy(policy)))
		{
			DBusMessage *signal = sailfish_iptables_signal(
					SAILFISH_IPTABLES_API_SIGNAL_POLICY,"%s",policy);
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
