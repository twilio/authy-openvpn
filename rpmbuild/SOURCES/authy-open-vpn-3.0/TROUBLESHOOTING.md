# Troubleshooting installation.

Authy VPN has a lot of debug logs to help you diagnose issues.
Start by editing your vpn server.conf so that it creates a log file.

## Logging

Add the following lines at the end of:  `/etc/openvpn/server.conf`

    status openvpn-status.log
    log         openvpn.log
    log-append  openvpn.log
    verb  4

## Verifying that the authy plugin is being properly loaded

If the pluging is loaded correctly, it will log this line at
initialization:

    sudo cat openvpn.log | grep '\[Authy\]'
    [Authy] Authy Two-Factor Authentication started


## Authentication logs

Authy will also log all the authentication activity on openvpn.log

#### Example log when auth fails

    [Authy] Authy Two-Factor Authentication started
    [Authy] Authenticating:  username=d@authy.com, token=1234567 and AUTHY_ID=1
    * About to connect() to api.authy.com port 443 (#0)
    *   Trying 107.23.117.172...
    * Connected to api.authy.com (107.23.117.172) port 443 (#0)
    * successfully set certificate verify locations:
    *   CAfile: none
      CApath: /etc/ssl/certs
    * SSL connection using ECDHE-RSA-AES256-SHA
    * Server certificate:
    * 	 subject: serialNumber=STgISFc5XC8h7Ocjt9n9-dNQ/cxTm75c; OU=GT76495531; OU=See www.rapidssl.com/resources/cps (c)12; OU=Domain Control Validated - RapidSSL(R); CN=api.authy.com
    * 	 start date: 2012-12-16 08:56:17 GMT
    * 	 expire date: 2014-01-18 09:08:03 GMT
    * 	 subjectAltName: api.authy.com matched
    * 	 issuer: C=US; O=GeoTrust, Inc.; CN=RapidSSL CA
    * 	 SSL certificate verify ok.
    > GET /protected/json/verify/1234567/1?api_key=004436f9c97c9988253bd95933296580 HTTP/1.1
    Host: api.authy.com
    Accept: */*

    < HTTP/1.1 401 Unauthorized
    < Server: nginx/1.1.19
    < Date: Fri, 16 Aug 2013 20:14:00 GMT
    < Content-Type: application/json;charset=utf-8
    < Content-Length: 88
    < Connection: keep-alive
    < Status: 401 Unauthorized
    < WWW-Authenticate: Basic realm="Access Denied"
    < 
    * Connection #0 to host api.authy.com left intact
    [Authy] Curl response: Code=0, Body={"success":"false","message":"token is invalid","errors":{"message":"token is invalid"}}
    [Authy] Auth failed for d@authy.com

