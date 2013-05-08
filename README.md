# Authy Open VPN Quick Start

## Pre-requisites

1. Authy API Key: https://www.authy.com/signup
2. An OpenVPN installation ([How To](http://openvpn.net/index.php/open-source/documentation/howto.html))
3. Your box must have the development toolchain (gcc, make ..) and
development  headers for curl and pam (for compilation)
4. You are ready to rumble!

## Quick Installation

### Getting the plugin

#### Debian based systems (as root)

	curl 'https://github.com/authy/authy-open-vpn/blob/master/debian/authy-open-vpn-1.1_1.1-1ubuntu1_i386.deb?raw=true'	-o authy-openvpn.deb
	dpkg -i authy-openvpn.deb

#### Debian based systems (with sudo). Ubuntu

	curl 'https://github.com/authy/authy-open-vpn/blob/master/debian/authy-open-vpn-1._1.0-1ubuntu1_i386.deb?raw=true' -o authy-openvpn.deb
	sudo dpkg -i authy-openvpn.deb

#### Red Hat based systems

	curl 'https://github.com/authy/authy-open-vpn/blob/master/rpmbuild/RPMS/x86_64/authy-open-vpn-1.0-1.el6.x86_64.rpm?raw=true' -o authy-openvpn.rpm
	rpm -i authy-openvpn.rpm

### Configuring OpenVPN to use the plugin

	echo "plugin /usr/lib/authy/authy-openvpn.so https://api.authy.com/protected/json AUTHY_API_KEY" >> /etc/openvpn/server.conf

Replace `AUTHY_API_KEY` with you key

### Restarting you server

#### Ubuntu

	sudo service openvpn restart

#### Debian

	/etc/init.d/openvpn restart
    
#### Red Hat and Fedora Core Linux

	/sbin/service openvpn restart
    
### Creating the /etc/authy.conf

Before start you will need to add to yours client configuration
(`client.conf`)

	auth-user-pass

The `authy.conf` file is needed to match the openvpn users with the
Authy ID.

For example if you aren't using user/pass, you can start using your
companies email as username and the authy token as password.

Giving this case the `authy.conf` will look like:

	user1@mycompany.com 1234
	user2@mycompany.com 1235
	.
	.
	.
	usern@mycompany.com 9999

If you are also using the common name to identify your clients
certificates to assign network groups, we can also check that the
username match with it. And the `authy.conf` will look like:

	user1@mycompany.com CN1 1234
	user2@mycompany.com CN2 1235
	.
	.
	.
	usern@mycompany.com CNn 9999



# Authy Open VPN

 Authy Open VPN provides two plugins for OpenVpn:
 
 * The First one is `authy-openvpn.so` that is the basic plugin that
   uses the API of authy.com to only use client cert + authy token to
   authenticate users to your vpn.
   
 * The Second one is `pam.so` that is the complementary plugin that
   you could use with the auth-openvpn plugin, to use cert +
   PAM user/pass + token to authenticate users to your vpn.
   
## Important things to know before you use this plugin

Authy will use the password field of the openvpn interface by default
to let the users input their tokens, when this plugin is used together
with the PAM module, it will let the users use the username field for
the pam porpouse and the password field will be the concatenation of
the PAM password with the Authy Token. For example a user with
username/password joe/pass and Authy Token 1234567 will input in the
username joe and in the password field pass1234567

## Building the plugin from the source code

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


## Configuring OpenVPN

At the end of the `server.conf` (it is usually at
`/etc/openvpn/server.conf`) or the plugins section you will need to
add the following line

	plugin /INSTALLATIONDIR/authy-openvpn.so APIURL APIKEY PAM

where:

INSTALLATIONDIR is usually /usr/lib/authy if you used `make install`
or one of the default packages (.deb or .rpm)

APIURL is https://api.authy.com/protected/json or
http://sandbox-api.authy.com/protected/json

APIKEY is your API Key something like d57d919d11e6b221c9bf6f7c882028f9

PAM should be `nopam` or `pam`, it is `nopam` by default so if you
decided to don't use the pam module you can avoid that param. Given
the case you want to use the pam module you should set that param to
`pam` and add

	plugin /INSTALLATIONDIR/pam.so YOUR_PAM

YOUR_PAM should be something like "login login USERNAME password
PASSWORD" it will usually work with any other module


## Basic Instructions to create the .deb

* Create the .tar.gz of the code
* dpkg-buildpackage -rfakeroot

## Basic Instructions to create the .rpm

* Create the .tar.gz of the code and move it inside the rpmbuild/SOURCES
* rpmbuild -v -bb --clean SPECS/authy-open-vpn.spec
