# Authy Open VPN

With Authy OpenVPN plugin you can add Two-Factor Authentication to
your vpn server in just minutes. This plugin supports certificate based
authentication or/and PAM.  

With Authy your users can authenticate using Authy mobile app, SMS,
phone calls or a hardware dongle.

_For hardware dongles and phone calls please contact sales@authy.com_

## Pre-Requisites

1. Authy API Key. Get one free at: https://www.authy.com/signup
2. OpenVPN installation ([How To](http://openvpn.net/index.php/open-source/documentation/howto.html))

## Quick Installation

### Using source code

1. Compile and install

        curl `https://codeload.github.com/authy/authy-open-vpn/zip/master` -o authy-openvpn.zip && tar -zxvf authy-openvpn.zip
        cd authy-open-vpn-master
        sudo make install

2. Get your free Authy API KEY from https://www.authy.com/signup .

3. Finally configure the plugin.  

        sudo ./postinstall

### Using Ubuntu and Debian packages

1. Download the deb package.

        curl 'https://github.com/authy/authy-open-vpn/blob/master/debian/authy-open-vpn-1.1_1.1-1ubuntu1_i386.deb?raw=true' -o authy-openvpn.deb

2. Install package.

        sudo dpkg -i authy-openvpn.deb

### CentOS and RedHat based systems

1. Download the rpm package.

        curl 'https://github.com/authy/authy-open-vpn/blob/master/rpmbuild/RPMS/x86_64/authy-open-vpn-1.1-1.el6.x86_64.rpm?raw=true' -o authy-openvpn.rpm

2. Install package.

        rpm -i authy-openvpn.rpm

3. Finally configure the plugin.

        bash /usr/lib/authy/postinstall

### Restarting your server

#### Ubuntu

	sudo service openvpn restart

#### Debian

	/etc/init.d/openvpn restart
    
#### CentOS and RedHat

	/sbin/service openvpn restart

## How Authy VPN works

### Certificates based Auth

In this scenario user needs: username + certificate + token to login.

If you are already using certificates to authenticate your vpn users you won't
need to regenerate them. Authy uses a authy-vpn.conf were you tell
authy the users login and the AUTHY_ID.

##### Example authy-vpn.conf for a user with AUTHY_ID 10229

    user1@company.com 10229

Here the user will enter `user1@company.com` as username and the
Token(which he gets from the app or SMS) as the password. The
certificate is transparently checked before this happens.  

### Authy plugin with PAM

If you are using PAM before you can still use authy Two-Factor
Authentication.

To use PAM simply answer that you are going to use PAM during the
postinstall script.


##### Example authy-vpn.conf for a user joe with AUTHY_ID 10229

    joe 10229

Here joe is the PAM login username.

Let's suppose joe password is `god`. So the user will enter `joe` as
username, and the password concatenated with the token in the password
field. EG.

    username:joe
    password:god1234567

`1234567` would be the Token.


## Adding your first user

This section will show how to properly setup Two-Factor Authentication
on one of your users.

The Authy plugin comes with a script to help with the users
registration and creation of the authy-vpn.conf

    sudo authy_vpn_add_users




And if you need to verify that joe's common name from the certificates
match. It will look like:

	joe COMMON_NAME_OF_JOE AUTHY_ID_OF_JOE


	auth-user-pass



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


# Last and very important

You as an openvpn admin will need to create your users in advance
within the Authy Dashboard

## Basic Instructions to create the .deb

* Create the .tar.gz of the code
* dpkg-buildpackage -rfakeroot

## Basic Instructions to create the .rpm

* Create the .tar.gz of the code and move it inside the rpmbuild/SOURCES
* rpmbuild -v -bb --clean SPECS/authy-open-vpn.spec
