# Building packages for distros.

This explains how to build the rmp and deb packages.


## Debian.

Modify debian/changelog and then run `dpkg-buildpackage -us -uc -b -rfakeroot` from the root directory.

##### Verifying package.
  
  Run liniant and puiparts

    lintian package-version.changes 
    piuparts binpackage-version.deb


## CentOS

First  setup the RPM environment: http://wiki.centos.org/HowTos/SetupRpmBuildEnvironment

Specially important is the `.rpmmacros`:

	cat ~/.rpmmacros 
		%_topdir /home/authy/authy/authy-open-vpn/rpmbuild


Then ceate a tarball with the version number in the name. 

You can create a tarball using

	tar cvzf authy-open-vpn-4.0.tar.gz authy-open-vpn-4.0

Move it to the SOURCE DIRECTORY:

	ls SOURCES
		authy-open-vpn-3.0.tar.gz

The whole RPM config is in the SPECS file so just run: 
	
	rpmbuild -ba SPECS/authy-open-vpn.spec

This will generate the RPM. Remember to move it to the packages directory.
