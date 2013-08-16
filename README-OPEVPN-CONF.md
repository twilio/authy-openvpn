# Instructions on setting a OpenVPN server for dev.

Start installing openvpn package.

## OpenVPN config

Config is on `/etc/openvpn/server.conf`

    port 1194
    proto tcp

    dev tun

    ca ca.crt
    cert server.crt
    key server.key  # This file should be kept secret
    tls-auth ta.key 0 # This file is secret

    dh dh1024.pem

    # ethernet bridging. See the man page for more info.
    server 10.8.0.0 255.255.255.0

    # previously assigned.
    ifconfig-pool-persist ipp.txt

    keepalive 10 120


    # Enable compression on the VPN link.
    # If you enable it here, you must also
    # enable it in the client config file.
    comp-lzo

    persist-key
    persist-tun

    status openvpn-status.log
    log         openvpn.log
    log-append  openvpn.log

    # Set the appropriate level of log
    # file verbosity.
    #
    # 0 is silent, except for fatal errors
    # 4 is reasonable for general usage
    # 5 and 6 can help to debug connection problems
    # 9 is extremely verbose
    verb 4



## Server certificates

    cd /etc/openvpn/easy-rsa/ ## move to the easy-rsa directory
    sudo chown -R root:admin .  ## make this directory writable by the system administrators
    sudo chmod g+w . ## make this directory writable by the system administrators
    source ./vars ## execute your new vars file
    ./clean-all  ## Setup the easy-rsa directory (Deletes all keys)
    ./build-dh  ## takes a while consider backgrounding
    ./pkitool --initca ## creates ca cert and key
    ./pkitool --server server ## creates a server cert and key
    ## If you get this error: 
    ##    "The correct version should have a comment that says: easy-rsa version 2.x"
    ## Try This:
    ##     sudo ln -s openssl-1.0.0.cnf openssl.cnf
    ## Refer to: https://bugs.launchpad.net/ubuntu/+source/openvpn/+bug/998918
    cd keys
    openvpn --genkey --secret ta.key  ## Build a TLS key
    sudo cp server.crt server.key ca.crt dh1024.pem ta.key ../../


## Client certificates


    cd /etc/openvpn/easy-rsa/ ## move to the easy-rsa directory
    source ./vars             ## execute the vars file
    KEY_CN=client ./pkitool client          ## create a cert and key named "client"
    ## Note: if you get a 'TXT_DB error number 2' error you may need to specify
    ## a unique KEY_CN, for example: KEY_CN=client ./pkitool client

Now move the necessary keys to a folder

    mkdir ~/client-keys/
    cd /etc/openvpn/easy-rsa/keys
    sudo cp ca.crt client.crt ta.key client.key ~/client-keys/

Now move client-keys to you client machine


## Client Setup

Use Viscosity VPN client. 

##### Advanced

Use OpenVPN version 2.2

##### General

Protocol: TCP
Device: TUN

##### Authentication

Type: SSL/TLS CLient

CA: ca.crt
CERT: client.crt
Key: client.key

TLs-Auth: ta.key
Direction 1  #super important

##### Options

Turn on: Perstist Tun, Persist Key, No Bind, Pull Options
LZO COmpression: Off


