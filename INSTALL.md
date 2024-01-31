A. Prerequisites

* This document is written only for CentOS systems (7.x 64bit). It may be used for other Linux systems such as Ubuntu, but there is no guarantee that it would function properly.
* The quota system is only tested with iRODS server 4.2.x, it cannot run with the earlier versions,  but may work with later ones.
* Follow this link https://packages.irods.org/ to install renci-irods repository before installing the quota system.
* All irods-externals-* packages have to be installed.
* The group “Development Tools” has to be installed.
* The openssl-devel, irods-devel and libcurl-devel packages have to be installed.

B. From RPM file

* At this stage, you could download the RPM package from github.com to install.
* However, the steps below will guide you on how to make a rpm package from source code and then install/configure it.

C. From source code

* `git clone https://github.com/hydroshare/hydroshare-quota.git	
* `cd hydroshare-quota`
* Modify the iRODS version in the line number 3 in the file: CMakeLists.txt to match with the version of the current iRODS server if needed. The current supported version is 4.2.4
* Use cmake on irods-externals-cmake package to run: cmake . 
  It is probably in the /opt/irods-externals/cmake-<version>/bin directory
* make package
* `rpm -Uvh hydroshare-quota-microservices*.rpm`
* Using irods account to run some iCommands to test… e.g. icd, ils… then check the newest log file in the directory: /var/lib/irods/log. 
  If get errors are “copy permission”, we have to grant the write permission for irods user on /etc/irods directory:
    - `setfacl -Rm u:irods:rwx /etc/irods`
    - `chmod -R 775 /etc/irods`
* See configuration document to modify the file /etc/irods/hydroshare.re
* Edit the file /etc/irods/server_config.json then change:
```
        "re_rulebase_set": [
                "core"
        ],
```
to:
```
        "re_rulebase_set": [
                "hydroshare,core"
        ],
```
* `ichmod  –r  –M  inherit  <HydroShare Root Dir>`
* `ichmod  –r  –M  own   rods   <HydroShare Root Dir>`

The last two commands will help all MSI(s) run faster and safer. There is no need to implement the code to switch to admin privileges.

* `imeta set -C <HydroShare Root Dir> resetQuotaDir 1`

This command will recalculate all user’s usage inside the HydroShare Root Dir then update the correct values to the “bags” Collection.

* `docker exec -u hydro-service -ti hydroshare python manage.py update_used_storage`

Run this command on the host server, it will sync all values from iRODS layer to Django data

* Update the warning message on the **Admin Site -> Theme -> Quota** Django admin panel to be: 

“Your quota for HydroShare resources is {allocated}{unit} in HydroShare zone. You currently have resources that consume {used}{unit}, {percent}% of your quota. Once your quota reaches 100% you will no longer be able to create new resources in HydroShare.”
