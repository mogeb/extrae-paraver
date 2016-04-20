#! /bin/bash

if [ $# -ne 1 ]; then
  echo "Usage: ${0} install_path"
  echo "  install_path: Destiny path; can contain user-defined cfgs."
  echo "                cfgs with same name are always rewritten with newer versions."
  echo "                Older installation is kept in a .cfgs.backup.tar.gz file"  
  exit 1
fi

INSTALL_PATH=${1}

# Test if install_path has some ./cfgs directory
if [[ `basename ${INSTALL_PATH}` = "cfgs" ]]; then
  LOCAL_CFGS_DIR=${INSTALL_PATH}
else
  LOCAL_CFGS_DIR=${INSTALL_PATH}/cfgs
fi

mkdir -p ${LOCAL_CFGS_DIR} 
echo "[paraver CFGs] Installation directory: ${LOCAL_CFGS_DIR}"


# Keep older backups if they exist
if [[ `ls -1A ${LOCAL_CFGS_DIR}/.cfgs.backup.* | wc -l` != "0" ]]; then
  MSG_PRINTED=0
  for OLDER_BACKUP in `ls -A ${LOCAL_CFGS_DIR}/.cfgs.backup.*` ; do
    if [ ${MSG_PRINTED} -eq 0 ]; then
      echo "[paraver CFGs] Keeping older backups:"
      MSG_PRINTED=1
    fi
    echo "[paraver CFGs]   ${OLDER_BACKUP}"
    mv ${OLDER_BACKUP} .
  done
fi


# Backup previous content
if [[ `ls -1 ${LOCAL_CFGS_DIR} | wc -l` != "0" ]]; then
  BACKUP_TIME=`date '+%Y%m%d-%H%M%S'`
  tar zcvf .cfgs.backup.${BACKUP_TIME}.tar.gz ${LOCAL_CFGS_DIR} &> /dev/null
  echo "[paraver CFGs] Backup current ./cfgs directory into .cfgs.backup.${BACKUP_TIME}.tar.gz"
fi 


# Install
echo -n "[paraver CFGs] Copying new cfgs..."
cp -ap ./* ${LOCAL_CFGS_DIR}/ &> /dev/null
echo "done."
echo -n "[paraver CFGs] Copying backups..."
cp -ap ./.cfgs* ${LOCAL_CFGS_DIR}/ &> /dev/null
echo "done."


# Clean
echo -n "[paraver CFGs] Cleaning..."
rm -f ${LOCAL_CFGS_DIR}/install.sh
rm -f ${LOCAL_CFGS_DIR}/build-sources-wxparaver.sh
rm -rf ${LOCAL_CFGS_DIR}/build-cfgs-*
find ${LOCAL_CFGS_DIR} -type d -name ".svn" -exec rm -rf {} \; &> /dev/null
find ${LOCAL_CFGS_DIR} -type f -name "svn-commit.*" -exec rm -rf {} \; &> /dev/null
find ${LOCAL_CFGS_DIR} -type f -name "*~" -exec rm -rf {} \; &> /dev/null
rm -f ./.cfgs.backup.* &> /dev/null
echo "done."

echo "[paraver CFGs] Installation finished."

