%define name authy-openvpn
%define version 4.0

Name:           %{name}
Version:        %{version}
Release:        3%{?dist}
Summary:        Authy Open VPN Two-Factor Authentication

Group:          System Environment/Libraries
License:        BSD
URL:            https://github.com/authy/authy-openvpn
Source0:        %{name}-%{version}.tar.gz

BuildRequires:	libcurl-devel


%description
 Authy Open VPN provides two plugins for OpenVpn
 * The First one is authy-openvpn.so that is the basic plugin that uses the API
 of authy.com to only use client cert + authy token to authenticate users to
 your vpn.

%prep
%setup -q


%build
make -j1


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
cp "scripts/post-install" $RPM_BUILD_ROOT/usr/lib/authy/

%clean
rm -rf $RPM_BUILD_ROOT

%post

echo "Remember to run \"bash $RPM_BUILD_ROOT/usr/lib/authy/post-install\" for configuration"

%files
%defattr(-,root,root,-)
/usr/lib/authy/authy-openvpn.so
/usr/lib/authy/post-install
/usr/sbin/authy-vpn-add-user

%changelog
* Tue Oct 29 2013 Authy <support at authy.com> - 4.0-1
- Release 4.


