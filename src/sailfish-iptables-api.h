#ifndef __SAILFISH_IPTABLES_API_H_
#define __SAILFISH_IPTABLES_API_H_

#include "sailfish-iptables-api-dbus.h"

enum sailfish_iptables_api_methods {
	SAILFISH_IPTABLES_API_GET_PROPERTY = 1,
	SAILFISH_IPTABLES_API_SET_PROPERTY
};

enum sailfish_iptables_api_ipt_operations {
	SAILFISH_IPTABLES_API_NEW_CHAIN = 1,
	SAILFISH_IPTABLES_API_DELETE_CHAIN,
	SAILFISH_IPTABLES_API_FLUSH_CHAIN,
	SAILFISH_IPTABLES_API_INSERT_RULE,
	SAILFISH_IPTABLES_API_APPEND_RULE,
	SAILFISH_IPTABLES_API_DELETE_RULE,
	SAILFISH_IPTABLES_API_CHANGE_POLICY
};

enum sailfish_iptables_api_ipt_manage {
	SAILFISH_IPTABLES_API_IPT_COMMIT = 1,
	SAILFISH_IPTABLES_API_IPT_LOAD,
	SAILFISH_IPTABLES_API_IPT_CLEAR
};

struct sailfish_iptables_api_iptables_rule
{
	int operation; // sailfish_api_ipt_operations
	const gchar* table;
	const gchar* chain;
	const gchar* policy;
	const gchar* rule;
};

struct sailfish_iptables_api_iptables_config
{
	int operation; // sailfish_api_ipt_manage
	const gchar* file_path;
};

static gboolean sailfish_iptables_api_save_firewall(const gchar* path);
static gboolean sailfish_iptables_api_load_firewall(const gchar* path);
static gboolean sailfish_iptables_api_clear_firewall();
static gboolean sailfish_iptables_api_set_input_policy(const gchar* policy);

#endif
