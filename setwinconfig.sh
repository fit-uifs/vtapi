#!/bin/bash
#
# Set configuration for Netbeans on Windows OS of VTApi project
# by Tomas Volf, 
# bugreport: ivolf(at)fit.vutbr.cz


# copying NetBeans configuration files for Windows
echo "Copying NetBeans config files for Windows..."
for i in nbprivatecfg-win/*;
do
  if [ ! -d "$i" ]; then
    continue
  fi
  cp -bfR "$i" ./
done