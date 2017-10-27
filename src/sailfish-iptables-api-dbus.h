/*
 *  Connection Manager D-Bus iptables 
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
 */

#ifndef __SAILFISH_IPTABLES_API_DBUS_H_
#define __SAILFISH_IPTABLES_API_DBUS_H_

#include <stdbool.h>
#include <string.h>
#include <dbus/dbus.h>
#include <glib.h>

#include <connman/log.h>
#include <connman/plugin.h>
#include <connman/dbus.h>
#include <connman/exposed_api.h>
#include <connman/gdbus_external.h>



#define SAILFISH_IPTABLES_INTERFACE				"org.sailfishos.connman.mdm.iptables"
#define SAILFISH_IPTABLES_PATH_PREFIX			"/org/sailfishos/connman/mdm/iptables"
#define SAILFISH_IPTABLES_INTERFACE_VERSION		1

#define SAILFISH_IPTABLES_API_RESULT 			{"result", "b"}
#define SAILFISH_IPTABLES_API_RESULT_VERSION	{"version", "i"}
#define SAILFISH_IPTABLES_API_INPUT				{"operation","i"}
#define SAILFISH_IPTABLES_API_INPUT_PATH		{"path","s"}
#define SAILFISH_IPTABLES_API_INPUT_ADDRESS		{"ip","s"}

#define SAILFISH_IPTABLES_API_SIGNAL_INIT		"Initialize"
#define SAILFISH_IPTABLES_API_SIGNAL_STOP		"Shutdown"
#define SAILFISH_IPTABLES_API_SIGNAL_LOAD		"Load"
#define SAILFISH_IPTABLES_API_SIGNAL_SAVE		"Save"
#define SAILFISH_IPTABLES_API_SIGNAL_MNG_PAR	SAILFISH_IPTABLES_API_INPUT_PATH
#define SAILFISH_IPTABLES_API_SIGNAL_CLEAR		"Clear"

#define SAILFISH_IPTABLES_API_SIGNAL_BAN		"BanIPv4"
#define SAILFISH_IPTABLES_API_SIGNAL_UNBAN		"UnbanIPv4"
#define SAILFISH_IPTABLES_API_SIGNAL_BAN_PAR	SAILFISH_IPTABLES_API_INPUT_ADDRESS
#define SAILFISH_IPTABLES_API_SIGNAL_UNBAN_PAR	SAILFISH_IPTABLES_API_INPUT_ADDRESS



#ifdef __cplusplus
extern "C" {
#endif

int sailfish_iptables_api_dbus_register();
int sailfish_iptables_api_dbus_unregister();

/* These are connected to dbus */

DBusMessage* sailfish_iptables_save_firewall(DBusConnection *connection,
			DBusMessage *message, void *user_data);
					
DBusMessage* sailfish_iptables_load_firewall(DBusConnection *connection,
			DBusMessage *message, void *user_data);

DBusMessage* sailfish_iptables_clear_firewall(DBusConnection *connection,
			DBusMessage *message, void *user_data);

DBusMessage* sailfish_iptables_version(DBusConnection *connection,
			DBusMessage *message, void *user_data);
			
DBusMessage* sailfish_iptables_ban_v4address(DBusConnection *connection,
			DBusMessage *message, void *user_data);

DBusMessage* sailfish_iptables_unban_v4address(DBusConnection *connection,
			DBusMessage *message, void *user_data);

void sailfish_iptables_send_signal(DBusMessage *signal);
			
DBusMessage* sailfish_iptables_signal(const gchar* signal, const gchar* arg);

#ifdef __cplusplus
}
#endif

#endif /* __SAILFISH_IPTABLES_API_DBUS_H_ */

/*
 * Local Variables:
 * mode: C
 * c-basic-offset: 8
 * indent-tabs-mode: t
 * End:
 */
