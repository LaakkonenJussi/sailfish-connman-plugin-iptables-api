/*
 *
 *  Connection Manager dbus api implementation for SailfishOS
 *
 *  Copyright (C) 2017 Jolla Ltd. All rights reserved.
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
 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define CONNMAN_API_SUBJECT_TO_CHANGE
#include <connman/dbus.h>

#include <errno.h>
#include "sailfish-iptables-api-dbus.h"

#define INFO(fmt,arg...) connman_info(fmt, ## arg)

static const GDBusSignalTable signals[] = {
		{ GDBUS_SIGNAL("Initialize", NULL) },
		{ GDBUS_SIGNAL("STOP", NULL) },
		{ }
	};
	
static const GDBusMethodTable methods[] = {
		{ GDBUS_METHOD("ManageFirewall", 
			GDBUS_ARGS(SAILFISH_IPTABLES_API_INPUT),
			GDBUS_ARGS(SAILFISH_IPTABLES_API_RESULT),
			sailfish_iptables_manage)
		},
		{ GDBUS_METHOD("SaveFirewall", 
			GDBUS_ARGS(SAILFISH_IPTABLES_API_INPUT),
			GDBUS_ARGS(SAILFISH_IPTABLES_API_RESULT),
			sailfish_iptables_manage)
		},
		{ GDBUS_METHOD("LoadFirewall", 
			GDBUS_ARGS(SAILFISH_IPTABLES_API_INPUT),
			GDBUS_ARGS(SAILFISH_IPTABLES_API_RESULT),
			sailfish_iptables_manage)
		},
		{ GDBUS_METHOD("ClearFirewall", 
			GDBUS_ARGS(SAILFISH_IPTABLES_API_INPUT),
			GDBUS_ARGS(SAILFISH_IPTABLES_API_RESULT),
			sailfish_iptables_manage)
		},
		{ GDBUS_METHOD("BanIP", 
			GDBUS_ARGS(SAILFISH_IPTABLES_API_INPUT_ADDRESS),
			GDBUS_ARGS(SAILFISH_IPTABLES_API_RESULT),
			sailfish_iptables_ban_v4address)
		},
		{ GDBUS_METHOD("UnbanIP", 
			GDBUS_ARGS(SAILFISH_IPTABLES_API_INPUT_ADDRESS),
			GDBUS_ARGS(SAILFISH_IPTABLES_API_RESULT),
			sailfish_iptables_unban_v4address)
		},
		{ GDBUS_METHOD("GetVersion", 
			NULL,
			GDBUS_ARGS(SAILFISH_IPTABLES_API_RESULT_VERSION),
			sailfish_iptables_version)
		},
		{ }
	};


int sailfish_iptables_api_dbus_register() {
	
	int rval = 0;
	
	DBusConnection* conn = dbus_connection_ref(connman_dbus_get_connection());
	if(conn)
	{
		if(g_dbus_register_interface(conn,
			SAILFISH_IPTABLES_PATH_PREFIX,
			SAILFISH_IPTABLES_INTERFACE,
			methods,
			signals,
			NULL,
			NULL,
			NULL))
		{
			
			DBusMessage *signal = dbus_message_new_signal(
						SAILFISH_IPTABLES_PATH_PREFIX,
						SAILFISH_IPTABLES_INTERFACE,
						"Initialize");
			g_dbus_send_message(conn,signal);
			
			DBusMessage *msg = dbus_message_new_method_call(
						CONNMAN_SERVICE,
						SAILFISH_IPTABLES_PATH_PREFIX,
						SAILFISH_IPTABLES_INTERFACE,
						"ManageFirewall");
			DBusMessageIter args;
			dbus_message_iter_init_append(msg,&args);
			int value = 1;
			dbus_uint32_t serial = 0;
			if(!dbus_message_iter_append_basic(&args,DBUS_TYPE_INT32,&value))
				INFO("IPTABLES API DBUS CANNOT APPEND ITER");
			if(!dbus_connection_send(conn,msg,&serial))
				INFO("IPTABLES API DBUS CANNOT SEND MESSAGE");
			else
				INFO("IPTABLES API DBUS MESSAGE SENT");
			dbus_connection_flush(conn);
			dbus_message_unref(msg);
		}
		else
		{
			INFO("IPTABLES API DBUS REGISTER FAILED");
			rval = -1;
		}
		dbus_connection_unref(conn);
	}
	else
	{
		INFO("IPTABLES API DBUS REGISTER FAILED");
		rval = -1;
	}
	INFO("%s %s %s", "IPTABLES API DBUS REGISTER:",
			SAILFISH_IPTABLES_PATH_PREFIX,
			SAILFISH_IPTABLES_INTERFACE);
	return rval;
}

int sailfish_iptables_api_dbus_unregister()
{
	int rval = 0;

	DBusConnection* conn = dbus_connection_ref(connman_dbus_get_connection());
	if(conn)
	{
		if(g_dbus_unregister_interface(conn,
			SAILFISH_IPTABLES_PATH_PREFIX,
			SAILFISH_IPTABLES_INTERFACE))
		{
			INFO("IPTABLES API DBUS UNREGISTER");
		}
		else
		{
			INFO("IPTABLES API DBUS UNREGISTER FAILED");
			rval = -1;
		}
	}
	else
		rval = -1;
	
	return rval;
}


