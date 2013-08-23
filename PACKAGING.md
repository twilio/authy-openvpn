# Building packages for distros.

This explains how to build the rmp and deb packages.


## Debian.

Modify debian/changelog and then run `dpkg-buildpackage -us -uc -b -rfakeroot` from the root directory.

##### Verifying package.
  
  Run liniant and puiparts

    lintian package-version.changes 
    piuparts binpackage-version.deb

        
