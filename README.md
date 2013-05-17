# Authy Open VPN

With Authy OpenVPN plugin you can add Two-Factor Authentication to
your vpn server in just minutes. This plugin supports certificate based
authentication and/or PAM.  

With Authy your users can authenticate using Authy mobile app or a hardware dongle.  

_For hardware dongles and phone calls please contact sales@authy.com_  

## Pre-Requisites

1. Authy API Key. Get one free at: [https://www.authy.com/signup](https://www.authy.com/signup).
2. OpenVPN installation ([How To](http://openvpn.net/index.php/open-source/documentation/howto.html))

## Quick Installation

### Using source code

1. Compile and install.

        curl `https://codeload.github.com/authy/authy-open-vpn/zip/master` -o authy-openvpn.zip && tar -zxvf authy-openvpn.zip
        cd authy-open-vpn-master
        sudo make install

2. Get your free Authy API KEY from [https://www.authy.com/signup](https://www.authy.com/signup).

3. Finally configure the plugin.  

        sudo ./postinstall

### Using Ubuntu and Debian packages

1. Download the deb package.

        curl 'https://github.com/authy/authy-open-vpn/blob/master/debian/authy-open-vpn-2.1_2.1-ubuntu_i386.deb?raw=true' -o authy-openvpn.deb

2. Install package.

        sudo dpkg -i authy-openvpn.deb

### CentOS and RedHat based systems

1. Download the rpm package.

        curl 'https://github.com/authy/authy-open-vpn/blob/master/rpmbuild/RPMS/x86_64/authy-open-vpn-2.1-2.el6.x86_64.rpm?raw=true' -o authy-openvpn.rpm

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
Token(which he gets from the app) as the password. The
certificate is transparently checked before this happens.  

### PAM based Auth

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

## Authy OpenVPN with Common Name Verification
### Example authy-vpn.conf for a user joe with Common Name joe1 and AUTHY_ID 10229

    joe joe1 10229

This will check that joe and the common name from the certificate
(joe1) matches before proceding with the authentication

## Client configuration

You will need to add

    auth-user-pass

to your `client.conf` this is to ensure that the OpenVPN client asks
for username and password

## Basic windows installation

You need to copy the following dlls  `authy-openvpn.dll`, `lib/msvcr100.dll` and `lib/normaliz.dll` to `OpenVPN\bin`, and `curl-bundle-ca.crt` to `OpenVPN\config\`

Add the following line to your `server.ovpn`

    plugin "C:\\Program Files\\OpenVPN\\bin\\authy-openvpn.dll" https://api.authy.com/protected/json AUTHY_API_KEY nopam

And create the `authy-vpn.conf` inside `C:\\Program Files\\OpenVPN\\config`, remember that the this file follows one of the following patterns

    USERNAME AUTHY_ID

or

    USERNAME COMMON_NAME AUTHY_ID

Remember that the last one is to also check the match between `USERNAME` and `COMMON_NAME`


## Basic Instructions to create the .deb

* Create the .tar.gz of the code
* dpkg-buildpackage -rfakeroot

## Basic Instructions to create the .rpm

* Create the .tar.gz of the code and move it inside the rpmbuild/SOURCES
* rpmbuild -v -bb --clean SPECS/authy-open-vpn.spec

## Basic Instruction to create the .dll

* Open the .sln file in VS (we used VS2010), rebuild the project and that should be enough
