%define name authy-open-vpn
%define version 2.3

Name:           %{name} 
Version:        %{version}
Release:        2%{?dist}
Summary:        Authy Open VPN Two-Factor Authentication

Group:          System Environment/Libraries
License:        BSD
URL:            https://github.com/authy/authy-open-vpn
Source0:        %{name}-%{version}.tar.gz

BuildRequires:	libcurl-devel, pam-devel  


%description
 Authy Open VPN provides two plugins for OpenVpn
 * The First one is authy-openvpn.so that is the basic plugin that uses the API
 of authy.com to only use client cert + authy token to authenticate users to
 your vpn.
 * The Second one is pam.so that is the complementary plugin that you could use
 with the auth-openvpn plugin, to use cert + user/pass + token to authenticate
 users to your vpn.

%prep
%setup -q


%build
make -j1


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
cp postinstall $RPM_BUILD_ROOT/usr/lib/authy/

%clean
rm -rf $RPM_BUILD_ROOT

%post

echo "Remember to run \"bash $RPM_BUILD_ROOT/usr/lib/authy/postinstall\" for configuration"

%files
%defattr(-,root,root,-)
/usr/lib/authy/authy-openvpn.so
/usr/lib/authy/pam.so
/usr/lib/authy/postinstall
/usr/sbin/authy_vpn_add_users

%changelog
* Fri May 20 2013 Sebastian Arcila Valenzuela (sarcilav) <sebastian at authy.com> - 2.3-2
- Release 2.3


