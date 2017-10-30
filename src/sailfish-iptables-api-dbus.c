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

#include <errno.h>
#include "sailfish-iptables-api-dbus.h"

#define INFO(fmt,arg...) connman_info(fmt, ## arg)
#define ERR(fmt,arg...) connman_error(fmt, ## arg)

#define TOKEN_DELIM " "
#define MAX_TOKENS 20

static const GDBusSignalTable signals[] = {
		{ GDBUS_SIGNAL(
			SAILFISH_IPTABLES_API_SIGNAL_INIT,
			NULL)
		},
		{ GDBUS_SIGNAL(
			SAILFISH_IPTABLES_API_SIGNAL_STOP,
			NULL)
		},
		{ GDBUS_SIGNAL(
			SAILFISH_IPTABLES_API_SIGNAL_LOAD,
			GDBUS_ARGS(SAILFISH_IPTABLES_API_SIGNAL_MNG_PAR))
		},
		{ GDBUS_SIGNAL(
			SAILFISH_IPTABLES_API_SIGNAL_SAVE,
			GDBUS_ARGS(SAILFISH_IPTABLES_API_SIGNAL_MNG_PAR))
		},
		{ GDBUS_SIGNAL(
			SAILFISH_IPTABLES_API_SIGNAL_CLEAR,
			NULL)
		},
		{ GDBUS_SIGNAL(
			SAILFISH_IPTABLES_API_SIGNAL_BAN,
			GDBUS_ARGS(SAILFISH_IPTABLES_API_SIGNAL_BAN_PAR))
		},
		{ GDBUS_SIGNAL(
			SAILFISH_IPTABLES_API_SIGNAL_UNBAN,
			GDBUS_ARGS(SAILFISH_IPTABLES_API_SIGNAL_UNBAN_PAR))
		},
		{ GDBUS_SIGNAL(
			SAILFISH_IPTABLES_API_SIGNAL_POLICY,
			GDBUS_ARGS(SAILFISH_IPTABLES_API_SIGNAL_POLICY_CHAIN, SAILFISH_IPTABLES_API_SIGNAL_POLICY_TYPE))
		},
		{ }
	};
	
static const GDBusMethodTable methods[] = {
		{ GDBUS_METHOD("SaveFirewall", 
			GDBUS_ARGS(SAILFISH_IPTABLES_API_INPUT_PATH),
			GDBUS_ARGS(SAILFISH_IPTABLES_API_RESULT),
			sailfish_iptables_save_firewall)
		},
		{ GDBUS_METHOD("LoadFirewall", 
			GDBUS_ARGS(SAILFISH_IPTABLES_API_INPUT_PATH),
			GDBUS_ARGS(SAILFISH_IPTABLES_API_RESULT),
			sailfish_iptables_load_firewall)
		},
		{ GDBUS_METHOD("ClearFirewall", 
			NULL,
			GDBUS_ARGS(SAILFISH_IPTABLES_API_RESULT),
			sailfish_iptables_clear_firewall)
		},
		{ GDBUS_METHOD("BanIPv4", 
			GDBUS_ARGS(SAILFISH_IPTABLES_API_INPUT_ADDRESS),
			GDBUS_ARGS(SAILFISH_IPTABLES_API_RESULT),
			sailfish_iptables_ban_v4address)
		},
		{ GDBUS_METHOD("UnbanIPv4", 
			GDBUS_ARGS(SAILFISH_IPTABLES_API_INPUT_ADDRESS),
			GDBUS_ARGS(SAILFISH_IPTABLES_API_RESULT),
			sailfish_iptables_unban_v4address)
		},
		{ GDBUS_METHOD("ChangeInputPolicy", 
			GDBUS_ARGS(SAILFISH_IPTABLES_API_INPUT_POLICY),
			GDBUS_ARGS(SAILFISH_IPTABLES_API_RESULT),
			sailfish_iptables_change_input_policy)
		},
		{ GDBUS_METHOD("GetVersion", 
			NULL,
			GDBUS_ARGS(SAILFISH_IPTABLES_API_RESULT_VERSION),
			sailfish_iptables_version)
		},
		{ }
	};
	
gboolean dbus_message_add_parameters(DBusMessage *msg, 
	va_list *params, gchar** str_tokens)
{
	DBusMessageIter iter;
	dbus_message_iter_init_append(msg,&iter);
	
	gint index = 0;
	gboolean rval = true;
	
	while(str_tokens[index])
	{
		void* arg = NULL;
		gint var = 0;
		gint type = 0; 
		switch(str_tokens[index][1])
		{
			case 's':
				type = DBUS_TYPE_STRING;
				arg = (char*)va_arg(*params,char*);
				break;
			case 'i':
			case 'd':
				type = DBUS_TYPE_INT32;
				var = va_arg(*params,int);
				arg = &var;
				break;
			case 'c':
				type = DBUS_TYPE_INT32;
				var = va_arg(*params,int);
				arg = &var;
				break;
			case 'b':
				type = DBUS_TYPE_BOOLEAN;
				var = va_arg(*params,int);
				arg = &var;
				break;
			default:
				rval = false;
		}
		if(!dbus_message_iter_append_basic(&iter,type,&arg))
		{
			ERR("%s %s %d", "dbus_message_add_parameters()",
				"Failed to add parameters, last type: ", type);
			rval = false;
			break;
		}
			
		index++;
	}
		
	return rval;

}
void sailfish_iptables_send_signal(DBusMessage *signal)
{
	DBusConnection* connman_dbus = dbus_connection_ref(
			connman_dbus_get_connection());
			
	g_dbus_send_message(connman_dbus,signal);
	dbus_connection_unref(connman_dbus);
}

DBusMessage* sailfish_iptables_signal(const gchar* signal_name,
	const gchar* param_fmt, ...)
{
	if(!signal_name) return NULL;
	
	DBusMessage *signal = dbus_message_new_signal(
					SAILFISH_IPTABLES_PATH_PREFIX,
					SAILFISH_IPTABLES_INTERFACE,
					signal_name);
	if(param_fmt && strlen(param_fmt))
	{
		va_list params;
		va_start(params,param_fmt);
		gchar** tokens = g_strsplit(param_fmt,TOKEN_DELIM,MAX_TOKENS);
		
		if(!dbus_message_add_parameters(signal,&params,tokens))
		{
			dbus_message_unref(signal);
			ERR("saifish_iptables_signal(): failed to add parameter to signal");
			return NULL;
		}
		
		va_end(params);
		
		if(tokens)
			g_strfreev(tokens);
	}
	
	return signal;
}

gint sailfish_iptables_api_dbus_register() {
	
	gint rval = 0;
	
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
			
			DBusMessage *signal = sailfish_iptables_signal(
					SAILFISH_IPTABLES_API_SIGNAL_INIT,NULL,NULL);
				
			if(signal)
				sailfish_iptables_send_signal(signal);
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

gint sailfish_iptables_api_dbus_unregister()
{
	gint rval = 0;

	DBusConnection* conn = dbus_connection_ref(connman_dbus_get_connection());
	if(conn)
	{
		if(g_dbus_unregister_interface(conn,
			SAILFISH_IPTABLES_PATH_PREFIX,
			SAILFISH_IPTABLES_INTERFACE))
		{
			DBusMessage *signal = sailfish_iptables_signal(
					SAILFISH_IPTABLES_API_SIGNAL_STOP,NULL,NULL);
			if(signal)
				sailfish_iptables_send_signal(signal);
		}
		else
		{
			INFO("IPTABLES API DBUS UNREGISTER FAILED");
			rval = -1;
		}
	}
	else 
	{
		INFO("IPTABLES API DBUS UNREGISTER FAILED");
		rval = -1;
	}
	
	INFO("IPTABLES API DBUS UNREGISTER");
	return rval;
}


