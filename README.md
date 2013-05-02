# Authy Open VPN

 Authy Open VPN provides two plugins for OpenVpn:
 
 * The First one is `authy-openvpn.so` that is the basic plugin that
   uses the API of authy.com to only use client cert + authy token to
   authenticate users to your vpn.
   
 * The Second one is `pam.so` that is the complementary plugin that
   you could use with the auth-openvpn plugin, to use cert +
   user/pass + token to authenticate users to your vpn.
   
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

## Building the plugin from the source code

Your environment needs to satisfy the list of
[dependencies](#dependencies) before you proceed to the compilation

You can download the code from
[GitHub](https://github.com/authy/authy-open-vpn/archive/master.zip)
or clone with `git clone https://github.com/authy/authy-open-vpn.git`

Once you have the code in your local machine and your working
directory is pointing where your downloaded the code

	make

This will compile the plugin an generate the following shared objects
`authy-openvpn.so` and `pam.so`, once you have the plugin object you
can install wherever you want, just remember where do you install the
plugins because you will need that later to config the plugins withing
the server configuration of the vpn.

The other option is to run as root

	make install

This will compile and install both elements of the plugin in
`/usr/lib/authy`

## Installing/Getting the .deb

## Installing/Getting the .rpm

## Configuring OpenVPN

At the end of the `server.conf` (it is usually at
`/etc/openvpn/server.conf`) or the plugins section you will need to
add the following line

	plugin /INSTALLATIONDIR/authy-openvpn.so APIURL APIKEY PAM

where:

INSTALLATIONDIR is usually /usr/lib/authy if you used `make install`
or one of the default packages (.deb or .rpm)

APIURL is https://api.authy.com/protected/json or
http://sandbox-api.authy.com

APIKEY is your API Key something like d57d919d11e6b221c9bf6f7c882028f9

PAM should be `nopam` or `pam`, it is `nopam` by default so if you
decided to don't use the pam module you can avoid that param. Given
the case you want to use the pam module you should set that param to
`pam` and add

	plugin /INSTALLATIONDIR/pam.so YOUR_PAM

YOUR_PAM should be something like "login login USERNAME password
PASSWORD" it will usually work with any other module

## Dependencies

* libc6
* libcurl
* pam-dev

## Basic Instructions to create the .deb

* Create the .tar.gz of the code
* dpkg-buildpackage -rfakeroot

## Basic Instructions to create the .rpm

* Create the .tar.gz of the code and move it inside the rpmbuild/SOURCES
* rpmbuild -v -bb --clean SPECS/authy-open-vpn.spec
