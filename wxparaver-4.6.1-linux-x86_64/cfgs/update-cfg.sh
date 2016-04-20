#! /bin/bash

cfgs=`find . -name "*.cfg"`
for cfg in ${cfgs}; do
  if [[ `grep "version" "${cfg}"`x != "x" ]]; then
    echo  -n "[${cfg}] : Backup (.bak) and updated."
    cp ${cfg} ${cfg}.bak
    sed 's/version/#ParaverCFG\nConfigFile.Version:/g; s/number_of_windows/ConfigFile.NumWindows:/g' ${cfg}.bak > ${cfg}
  fi
done
