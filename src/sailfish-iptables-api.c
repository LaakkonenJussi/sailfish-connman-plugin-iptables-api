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


#include "sailfish-iptables-api.h"

#define ERR(fmt,arg...) connman_error(fmt, ## arg)
#define INFO(fmt,arg...) connman_info(fmt, ## arg)

static bool sailfish_iptables_api_save_firewall()
{
	INFO("SAILFISH IPTABLES API SAVE");
	return true;
}

static bool sailfish_iptables_api_clear_firewall()
{
	INFO("SAILFISH IPTABLES API CLEAR");
	return true;
}

static bool sailfish_iptables_api_load_firewall()
{
	INFO("SAILFISH IPTABLES API LOAD");
	return true;
}

DBusMessage* sailfish_iptables_manage(DBusConnection *connection,
					DBusMessage *message, void *user_data)
{

	dbus_int32_t operation;
	if (dbus_message_get_args(message, NULL,
					DBUS_TYPE_INT32, &operation,
					DBUS_TYPE_INVALID))
	{
		switch(operation)
		{
			case CONNMAN_DBUS_IPTABLES_API_IPT_COMMIT:
				sailfish_iptables_api_save_firewall(NULL);
				break;
			case CONNMAN_DBUS_IPTABLES_API_IPT_LOAD:
				sailfish_iptables_api_load_firewall(NULL);
				break;
			case CONNMAN_DBUS_IPTABLES_API_IPT_CLEAR:
				sailfish_iptables_api_clear_firewall();
				break;
			default:
			 	return NULL;
		}
		return NULL;
	}
	else
		return NULL;	
}

static int sailfish_ipt_api_init(void)
{
	INFO("INITIALIZE IPTABLES API");
	
	struct connman_dbus_iptables_api_data *data = g_new0(
		struct connman_dbus_iptables_api_data,1);
	
	data->path 		= SAILFISH_IPTABLES_PATH_PREFIX;
	data->interface = SAILFISH_IPTABLES_INTERFACE;
	
	data->methods = NULL;
	
	struct connman_dbus_iptables_api_method *method = g_new0(
		struct connman_dbus_iptables_api_method,1);
	
	method->name = "ManageFirewall";
	method->method_call = sailfish_iptables_manage;
	method->args_in = g_new0(struct GDBusArgInfo,1);
	method->args_in->name = "operation";
	method->args_in->signature = "i";
	method->args_out = NULL;
	
	data->methods = method;
	
	connman_dbus_iptables_api_register(data);
	
	g_free(data->methods);
	g_free(data);
	
	return 0;
}

static void sailfish_ipt_api_exit(void)
{
	INFO("EXIT IPTABLES API");
	
	struct connman_dbus_iptables_api_data *data = g_new0(
		struct connman_dbus_iptables_api_data,1);
	
	data->path 		= SAILFISH_IPTABLES_PATH_PREFIX;
	data->interface = SAILFISH_IPTABLES_INTERFACE;
	
	connman_dbus_iptables_api_unregister(data);

	g_free(data);
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
