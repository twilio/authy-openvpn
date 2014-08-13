# Authy Open VPN

With Authy OpenVPN plugin you can add Two-Factor Authentication to
your vpn server in just minutes. This plugin supports certificate based
authentication, PAM or LDAP.

With Authy your users can authenticate using Authy mobile app or a hardware dongle.

_For hardware dongles, phone calls or LDAP please contact sales@authy.com_

## Pre-Requisites

1. Authy API Key. Get one free at: [https://www.authy.com/signup](https://www.authy.com/signup).
2. OpenVPN installation ([How To](http://openvpn.net/index.php/open-source/documentation/howto.html))

## Quick Installation

#### Using the source code

__This is the recommended way of installing.__

##### Required libs

1. Compiler:

        - Ubuntu: apt-get install build-essential
        - Centos: yum groupinstall 'Development Tools'

2. libcurl with SSL:

        - Ubuntu: apt-get install libcurl4-openssl-dev
        - CentOS: yum install libcurl-devel.x86_64

##### Compiling and installing

1. Compile and install.

        curl -L "https://github.com/authy/authy-openvpn/archive/master.tar.gz" -o authy-openvpn.tar.gz
        tar -zxvf authy-openvpn.tar.gz
        cd authy-openvpn-master
        make
        sudo make install

2. Get your free Authy API KEY from: [https://www.authy.com/signup](https://www.authy.com/signup).

3. Finally configure the plugin.

        sudo ./scripts/post-install

4. Restart your server (see below).

5. Start adding users using `sudo authy-vpn-add-user` (see below).

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

    sudo authy-vpn-add-user

    sudo authy-vpn-add-user
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
    Success: User liz@authy.com was registered with AUTHY_ID 12323.

<br/>
<br/>


## How Authy-VPN works

Authy stores it's configuration in the file `/etc/openvpn/authy/authy-vpn.conf`
The files format is:

    username authy_id

For example for `liz@authy.com` it would look:

    sudo cat /etc/openvpn/authy/authy-vpn.conf
    liz@authy.com 12323

When liz is login in, she will type `liz@authy.com` as her username and the
token as the password.

You can edit this file by hand or using `authy-vpn-add-user`

### With Certificates based Authentication

In this scenario user needs: username + certificate + token to login.

If you're  already using certificates to authenticate your vpn users you won't
need to regenerate them. All you have to do is edit '/etc/openvpn/authy/authy-vpn.conf' were you tell
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
`post-install` script.

#### After run the post-install script your server.conf should have the following lines:

    # This line was added by the authy-openvpn installer
    plugin /usr/lib/authy/authy-openvpn.so https://api.authy.com/protected/json [YOUR_API_KEY] pam

Make sure your pam openvpn plugin is loaded after the authy openvpn plugin.
Plugins are loaded in the order they appear in the config file, the result should look like:

    # This line was added by the authy-openvpn installer
    plugin /usr/lib/authy/authy-openvpn.so https://api.authy.com/protected/json [YOUR_API_KEY] pam

    plugin /usr/lib/openvpn/openvpn-auth-pam.so "login login USERNAME password PASSWORD"


Also your users will need to separate the password from the token during login
by using a '-' character.

Eg:
		[PASSWORD]-[TOKEN]


##### Example authy-vpn.conf for a user joe with AUTHY_ID 10229

    joe 10229

Here joe is the PAM login username.

Let's suppose joe password is `god`. So the user will enter `joe` as
username. On the password field he will enter his password followed by a `-` followed by the Authy Token.

EG.

    username:joe
    password:god-1234567

`1234567` would be the Authy Token and `god` his password.

<br/>
<br/>
<br/>



## SMS and Phone Calls

To use SMS or Phone calls the user will have to enter `sms` or `call` as
the password. The first authentication will fail. The user should then
wait for the SMS or Call to arrive and re-authenticate with the right
username and token.

Eg.

    auth#1:
      username: joe@authy.com
      password: sms

    auth#2:
      username: joe@authy.com
      password: 172839


## Optional: Authy OpenVPN with Common Name Verification

Authy by default does not verify that the common name in the certificate matches the login.
This means a user can logon with someone elses certificate and a different Two-Factor Auth login.


This normaly ok as most of the time all users in the VPN have the same privileges and routes.
If this is not the case we suggest you verify the common name matches the Two-Factor login.
This is accomplish by modifying authy-vpn.conf to add the common name to every login.

### Example authy-vpn.conf for a user joe with Common Name joe1 and AUTHY_ID 10229

    joe 10229 joe1

This will check that joe and the common name from the certificate
(joe1) matches before proceding with the authentication.

## VPN Client configuration for all users

Your users will need to add

    auth-user-pass

to their `client.conf`. This is to ensure that the OpenVPN client asks
for username and password (this is where they enter the token).

