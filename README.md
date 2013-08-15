# Authy Open VPN

With Authy OpenVPN plugin you can add Two-Factor Authentication to
your vpn server in just minutes. This plugin supports certificate based
authentication and/or PAM.  

With Authy your users can authenticate using Authy mobile app or a hardware dongle.  

_For hardware dongles, SMS and phone calls please contact sales@authy.com_  

## Pre-Requisites

1. Authy API Key. Get one free at: [https://www.authy.com/signup](https://www.authy.com/signup).
2. OpenVPN installation ([How To](http://openvpn.net/index.php/open-source/documentation/howto.html))

## Quick Installation

#### Using source code


##### Require libs

1. build-essential: gcc, libc
2. libcurl with SSL: libcurl4-openssl-dev 
3. libpam: libpam0g-dev

##### Compiling and installing

1. Compile and install.
        curl `https://codeload.github.com/authy/authy-open-vpn/zip/master` -o authy-openvpn.zip && tar -zxvf authy-openvpn.zip
        cd authy-open-vpn-master
        sudo make install

2. Get your free Authy API KEY from [https://www.authy.com/signup](https://www.authy.com/signup).

3. Finally configure the plugin.  

        sudo ./postinstall


4. Restart your server (see below).

5. Start adding users using `sudo authy_vpn_add_users` (see below).

#### Using Ubuntu and Debian packages

1. Download the deb package.
    - i386

            curl 'https://github.com/authy/authy-open-vpn/blob/master/debian/authy-open-vpn-2.2_2.2-ubuntu_i386.deb?raw=true' -o authy-openvpn.deb
    - amd64

            curl 'https://github.com/authy/authy-open-vpn/blob/master/debian/authy-open-vpn-2.3_2.3-ubuntu_amd64.deb?raw=true' -o authy-openvpn.deb


2. Move your OpenVPN config file to /etc/openvpn/server.conf
  
        sudo mv /etc/openvpn/[your-open-vpn].conf /etc/openvpn/server.conf


3. Install debian package.

        sudo dpkg -i authy-openvpn.deb

        During installation you will be asked your API Key


4. Restart your server (see below).

5. Start adding users using `sudo authy_vpn_add_users` (see below)


#### CentOS and RedHat based systems

1. Download the rpm package.

        curl 'https://github.com/authy/authy-open-vpn/blob/master/rpmbuild/RPMS/x86_64/authy-open-vpn-2.2-2.el6.x86_64.rpm?raw=true' -o authy-openvpn.rpm

2. Install package.

        rpm -i authy-openvpn.rpm

3. Finally configure the plugin.

        bash /usr/lib/authy/postinstall


4. Restart your server (see below).

5. Start adding users using `sudo authy_vpn_add_users` (see below)

####  Windows install

You need to copy the following dlls  `authy-openvpn.dll`, `lib/msvcr100.dll` and `lib/normaliz.dll` to `OpenVPN\bin`, and `curl-bundle-ca.crt` to `OpenVPN\config\`

Add the following line to your `server.ovpn`

    plugin "C:\\Program Files\\OpenVPN\\bin\\authy-openvpn.dll" https://api.authy.com/protected/json AUTHY_API_KEY nopam

And create the `authy-vpn.conf` inside `C:\\Program Files\\OpenVPN\\config`, remember that the this file follows one of the following patterns

    USERNAME AUTHY_ID

or

    USERNAME COMMON_NAME AUTHY_ID

Remember that the last one is to also check the match between `USERNAME` and `COMMON_NAME`


### Restarting your OpenVPN server

#### Ubuntu

	sudo service openvpn restart

#### Debian

	/etc/init.d/openvpn restart
    
#### CentOS and RedHat

	/sbin/service openvpn restart

<br/>
<br/>

## Adding Users

To add users make sure you have their cellphone numbers. 

The Authy VPN plugin comes with a script, that helps you register users.

To start adding users type:

    sudo authy_vpn_add_users 
    
    sudo authy_vpn_add_users
    This script is to add users to Authy Open VPN
    For each user you will need to provide the vpn login, e-mail, country code and cellphone
    For PAM, login is the *nix login or your PAM login username.
    For certificate based Auth we recommend you use e-mails as the login.
    Login: liz@authy.com
    Email: liz@authy.com
    Country Code (EG. 1 for US): 1
    Cellphone: 347-388-2229
    Registering the user with Authy
    ...
    Success: Now, user liz@authy.com has Two-Factor Authentication enabled.

<br/>
<br/>
  
  
## How Authy-VPN works

Authy stores it's configuration in the file `/etc/authy-vpn.conf`
The files format is:

    username authy_id

For example for `liz@authy.com` it would look:

    sudo cat /etc/authy-vpn.conf
    liz@authy.com 12323

When liz is login in, she will type `liz@authy.com` as her username and the
token as the password.

You can edit this file by hand or using `authy_vpn_add_users`

### With Certificates based Authentication

In this scenario user needs: username + certificate + token to login.

If you're  already using certificates to authenticate your vpn users you won't
need to regenerate them. All you have to do is edit `etc/uthy-vpn.conf' were you tell
authy the users login and the AUTHY_ID.

##### Example authy-vpn.conf for a user joe with AUTHY_ID 10229

    joe@company.com 10229

Here the user will enter `joe@company.com` as username and the
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

<br/>
<br/>
<br/>

## Optional: Authy OpenVPN with Common Name Verification

Authy by default does not verify that the common name in the certificate matches the login.
This means a user can logon with someone elses certificate and a different Two-Factor Auth login.


This normaly ok as most of the time all users in the VPN have the same priviledges and routes.
If this is not the case we suggest you verify the common name matches the Two-Factor login.
This is accomplish by modifying authy-vpn.conf to add the common name to every login.

### Example authy-vpn.conf for a user joe with Common Name joe1 and AUTHY_ID 10229

    joe joe1 10229

This will check that joe and the common name from the certificate
(joe1) matches before proceding with the authentication

## VPN Client configuration for all users

Your users will need to add

    auth-user-pass

to their `client.conf`. This is to ensure that the OpenVPN client asks
for username and password (this is where they enter the token).
