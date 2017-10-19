#ifndef __SAILFISH_IPTABLES_API_H_
#define __SAILFISH_IPTABLES_API_H_

#include <stdbool.h>
#include <connman/dbus_iptables_api.h>
#include <connman/exposed_api.h>
#include <connman/log.h>
#include <connman/plugin.h>
#include <glib.h>

#define SAILFISH_IPTABLES_INTERFACE				"org.sailfishos.connman.mdm.iptables"
#define SAILFISH_IPTABLES_PATH_PREFIX			"/org/sailfishos/connman/mdm/iptables/manage"
#define SAILFISH_IPTABLES_INTERFACE_VERSION		1

#define SAILFISH_IPTABLES_API_RESULT 			{"result", "b"}
#define SAILFISH_IPTABLES_API_INPUT				{"operation","i"}
#define SAILFISH_IPTABLES_API_SIGNAL_INIT		"Initialize"
#define SAILFISH_IPTABLES_API_SIGNAL_STOP		"Shutdown"

typedef DBusMessage * (* GDBusMethodFunction) (DBusConnection *connection,
					DBusMessage *message, void *user_data);

struct GDBusArgInfo {
	const char *name;
	const char *signature;
};

DBusMessage* sailfish_iptables_manage(DBusConnection *connection,
					DBusMessage *message, void *user_data);
					
static bool sailfish_iptables_api_save_firewall();
static bool sailfish_iptables_api_clear_firewall();
static bool sailfish_iptables_api_load_firewall();


#endif
