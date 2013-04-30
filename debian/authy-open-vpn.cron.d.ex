#
# Regular cron jobs for the authy-open-vpn package
#
0 4	* * *	root	[ -x /usr/bin/authy-open-vpn_maintenance ] && /usr/bin/authy-open-vpn_maintenance
