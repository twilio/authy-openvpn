# Authy Open VPN Quick Start

## Pre-requisites

1. Authy API Key: https://www.authy.com/signup
2. An OpenVPN installation ([How To](http://openvpn.net/index.php/open-source/documentation/howto.html))
3. (For compilation) Your box must have the development toolchain
(gcc, make ..) and development  headers for curl and pam

## Quick Installation

### From source

	curl `https://codeload.github.com/authy/authy-open-vpn/zip/master` -o authy-openvpn.zip && tar -zxvf authy-openvpn.zip
	cd authy-open-vpn-master
	sudo make install

Get your free Authy API KEY from https://www.authy.com/signup .

Finally configure the plugin.  

	sudo ./postinstall

### Using Ubuntu and Debian packages

	curl 'https://github.com/authy/authy-open-vpn/blob/master/debian/authy-open-vpn-1.1_1.1-1ubuntu1_i386.deb?raw=true'	-o authy-openvpn.deb
	sudo dpkg -i authy-openvpn.deb

### CentOS and RedHat based systems

	curl 'https://github.com/authy/authy-open-vpn/blob/master/rpmbuild/RPMS/x86_64/authy-open-vpn-1.1-1.el6.x86_64.rpm?raw=true' -o authy-openvpn.rpm
	rpm -i authy-openvpn.rpm
	bash /usr/lib/authy/postinstall

## Restarting your server

### Ubuntu

	sudo service openvpn restart

### Debian

	/etc/init.d/openvpn restart
    
### CentOS and RedHat

	/sbin/service openvpn restart

## How it works

If you were using certificates you won't need to regenerate your
certificates or avoid the use of these.

Authy plugin intercepts the auth user pass verification, to add the
TWO Factor authentication, and for this you will need to ask to your
users to add

	auth-user-pass

to their client configuration, that is usually `client.conf`. This
line is to force the prompt of username/password within the different
clients.

For example if you aren't using user/pass or PAM, you can start using
your companies email as username and the authy token as password.

### /etc/authy.conf

The `/etc/authy.conf` file is needed to match the openvpn users with the
Authy ID.

Giving this case the `/etc/authy.conf` will look like:

	user1@mycompany.com AUTHY_ID1
	user2@mycompany.com AUTHY_ID2
	.
	.
	.
	usern@mycompany.com AUTHY_IDN

If you are also using the common name in the client certificates  to
identify your clients certificates to assign network groups or any
other configuration that checks the common name, we can also check
that the username match with it. And the `/etc/authy.conf` will look
like:

	user1@mycompany.com CN1 AUTHY_ID1
	user2@mycompany.com CN2 AUTHY_ID2
	.
	.
	.
	usern@mycompany.com CNn AUTHY_IDN

For these cases the user `user1@mycompany.com` when asked for
username/password at the client interface will input
`user1@mycompany.com` in the username field and an authy token in the
password field.

### Authy plugin with PAM

If you were using PAM before you can still use authy two factor
authentication.

Note: It is important to know that you can easily setup the pam integration
just answering that you are going to use PAM in the postinstall script

For example you have the following user `joe` with password equal to
`superpass` and you start using the Authy plugin, he will just need to
change one thing in the authentication workflow, for username he will
keep inputting `joe` but in the password field he will need to
concatenate his password with his current authy token, inputting
something like `superpass1234567` where `1234567` is his current
token.

For this case your `/etc/authy.conf` will look like:

	joe AUTHY_ID_OF_JOE

And if you need to verify that joe's common name from the certificates
match. It will look like:

	joe COMMON_NAME_OF_JOE

# Last and very important

You as an openvpn admin will need to create your users in advance
within the Authy Dashboard.

## Basic Instructions to create the .deb

* Create the .tar.gz of the code
* dpkg-buildpackage -rfakeroot

## Basic Instructions to create the .rpm

* Create the .tar.gz of the code and move it inside the rpmbuild/SOURCES
* rpmbuild -v -bb --clean SPECS/authy-open-vpn.spec
