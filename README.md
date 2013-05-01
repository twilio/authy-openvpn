# Authy Open VPN

 Authy Open VPN provides two plugins for OpenVpn:
 
 * The First one is `authy-openvpn.so` that is the basic plugin that
   uses the API of authy.com to only use client cert + authy token to
   authenticate users to your vpn.
   
 * The Second one is `pam.so` that is the complementary plugin that
   you could use with the auth-openvpn plugin, to use cert + user/pass
   + token to authenticate users to your vpn.
   
## Important things to know before you use this plugin

This plugin uses client certificates in which the CN (Common Name) is
equal to the Authy ID of the users. So this implies the regeneration
of yours clients/users certificates.

It is also important to know that while you are regenerating the
certificates, you will need to input the Authy ID in the CN field for
the specific client (Note that this means that the admin will need to
register the users in advance, this is needed because openvpn doesn't
use an enrollment interface).

Authy will use the password field of the openvpn interface by default
to let the users input their tokens, when this plugin is used together
with the pam module, it will let the users use the username field for
the pam porpouse and the password field will be the concatenation of
the PAM password with the Authy Token. For example a user with
username/password joe/pass and Authy Token 1234567 will input in the
username joe and in the password field pass1234567

# Installation

There are 3 options:

1.  Build the plugin from the source code.
2.  Use the debian package (.deb).
3.  Use the red hat package (.rpm).


## Dependencies

* libc6
* libcurl4-openssl-dev
* libpam0g-dev

## Basic Instruction to create the .deb

* Create the .tar.gz of the code
* dpkg-buildpackage -rfakeroot
