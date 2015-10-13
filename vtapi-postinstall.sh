#!/bin/sh

##########################################################
###   Post-installation guide script for VTApi         ###
###                                                    ###
###   Authors:                                         ###
###     * Tomáš Volf, ivolf@fit.vutbr.cz               ###
###     * Vojtěch Fröml, ifroml@fit.vutbr.cz           ###
###                                                    ###
###   Bug reporting:                                   ###
###     => Tomáš Volf, ivolf@fit.vutbr.cz              ###
##########################################################

function getPgConfigPath() {
  pg_config_attempts=2
  
  pg_config >/dev/null 2>&1
  if test "${?}" != 0
  then
    read -p "Executable pg_config could not be found in path through the system path, please type the pg_config path manually: " -i "/usr/local/" -e pg_config_path
  else
    echo "pg_config"
    return 0
  fi

  # -1 => pg_config was not found at 3 attempts
  # -2 => pg_config was found
  while test ${pg_config_attempts} -ge 0
  do
    pg_config_attempts=$(( $pg_config_attempts - 1 ))
    
    if [[ "${pg_config_path}" != */pg_config ]]
    then
      pg_config_path=${pg_config_path}/pg_config
    fi
    
    if test -e ${pg_config_path}
    then
      if test -x ${pg_config_path} 
      then
        echo ${pg_config_path}
	pg_config_attempts=-2
	break
      fi
    else
      if test ${pg_config_attempts} != -1
      then
        read -p "WARNING: It seems that you typed wrong pg_config path. Please check it and correct it: " -i "${pg_config_path}" -e pg_config_path
      else
        >&2 echo "ERROR: Can not continue due to the pg_config could not be found."
        exit
      fi
    fi
  done

  return `test ${pg_config_attempts} == -2`
}

function qDefault() {
  msg=${1}
  default=${2}
  
  read -p "${msg}   ( default: ${default} ): " readvalue
  if test "${readvalue}" = ""
  then
    readvalue=${default}
  fi
  
  echo ${readvalue}
  return 0
}

# Function to manage Yes/No question with default value
# $1 => prompt message
# $2 => default value
function qYesNo() {
  msg=${1}
  default=${2}
  
  msg_default=
  if test "${default}" != "" \
     && (test "${default}" = "y" || test "${default}" = "Y" || test "${default}" = "yes" || test "${default}" = "Yes" || test "${default}" = "YES" \
         || test "${default}" = "n" || test "${default}" = "N" || test "${default}" = "no" || test "${default}" = "No" || test "${default}" = "NO")
  then
    msg_default="; default: ${default}"
  fi
  
  while true
  do
    read -p "${msg}   ( y/n${msg_default} ): " readvalue
    if test "${msg_default}" != "" && test "${readvalue}" = ""
    then
      readvalue=${default}
      break
    elif test "${readvalue}" = "y" || test "${readvalue}" = "Y" || test "${readvalue}" = "yes" || test "${readvalue}" = "Yes" || test "${readvalue}" = "YES"
    then
      readvalue=y
      break
    elif test "${readvalue}" = "n" || test "${readvalue}" = "N" || test "${readvalue}" = "no" || test "${readvalue}" = "No" || test "${readvalue}" = "NO"
    then
      readvalue=n
      break
    fi
  done
  
  echo ${readvalue}
  return 0
}

# Function to manage Yes/No question with default values and to manage/prepare log configuration
# $1 => prompt message
# $2 => default value
# $3 => log value to manage/prepare
function qLogsYesNo() {
  msg=${1}
  default=${2}
  cfg_log_value=${3}

  answer=`qYesNo "${msg}" ${default}`
  if test "${answer}" = "n"
  then
    cfg_log_value="# ${cfg_log_value}"
  fi
  
  echo ${cfg_log_value}
  return 0
}





# TODO: trimming...
echo
echo "---------- CONFIG file settings ------------------"
echo 

# TODO: check if file exists -> question to replace
cfg_fname=`qDefault "Enter name of vtapi.conf file" ./vtapi.conf`


echo
echo "----------- PostgreSQL settings -------------------"
echo 

dbhost=`qDefault "Enter PostgreSQL server address" localhost`
dbport=`qDefault "Enter PostgreSQL port" 5432`
dbuser=`qDefault "Enter username to connect PostgreSQL" vidte`

while true
do
  read -sp "Enter password for ${dbuser}@${dbhost}: " dbpasswd
  echo
  if test "${dbpasswd}" = ""
  then
    >&2 echo "Error: password can not be empty - try it again."
    echo
  else
    break
  fi
done

dbname=`qDefault "Enter PostgreSQL database to use" vidte`



echo
echo "---------- VTApi directories settings ------------"
echo

cfg_datasets_dir=`qDefault "Enter the location of video-data directory" ./vtapi_data`
cfg_modules_dir=`qDefault "Enter the location of VTApi module binaries directory" ./vtapi_modules`



echo
echo "---------- VTApi logger settings -----------------"
echo

cfg_log_err=`qLogsYesNo "Would you like to log error messages?" y log_errors`
cfg_log_warn=`qLogsYesNo "Would you like to log warning messages?" n log_warnings`
cfg_log_debug=`qLogsYesNo "Would you like to log debug messages?" n log_debug`



# TODO: is enabled quotation of path in config file?
echo "# VTApi configuration file


############## Data location ##############

# Folder containing your datasets
datasets_dir=${cfg_datasets_dir}

# Folder containing your VTApi module binaries
modules_dir=${cfg_modules_dir}

# Sets default dataset
# dataset=demo_dataset

############# Connection #############

# Database connection string
#
#### PostgreSQL ####
# postgresql://[user[:password]@[netloc][:port][/dbname][?param1=value1$...]
#
# example:
# connection=postgresql://mjackson:pass123@localhost:4321/mydb

#### SQLite ####
# sqlite://[db_folder]
#
# example:
# connection=sqlite:///mnt/vtapi_db

connection=postgresql://${dbuser}@${dbhost}:${dbport}/${dbname}


############## Logging ##############

# Log file location (leave empty for standard output)
logfile=${cfg_log_fname}

# Log error messages (uncomment to enable)
${cfg_log_err}

# Log warning messages (uncomment to enable)
${cfg_log_warn}

# Log verbose debugging information (uncomment to enable)
${cfg_log_debug}" > "${cfg_fname}"



echo
echo "=================================================="
echo

req_db_guide=`qYesNo "Would you like to continue with DATABASE OPERATIONS GUIDE?"`
if test "${req_db_guide}" = "y"
then
  is_localhost=$(( `expr "${dbhost}" = "localhost"` || `expr "${dbhost}" = "$(hostname)"` ))

  req_create_user=`qYesNo "Would you like to create user \"${dbuser}\" in PostgreSQL?"`
  if test "${req_create_user}" = "y"
  then
    if test $is_localhost
    then
      createuser "${dbuser}"
    else
      >&2 echo "WARNING: If you would like to create user, you need to do this manually on remote host."
    fi
  fi
  
  req_save_passwd=`qYesNo "Would you like to save user password to ~/.pgpass file?"`
  if test "${req_save_passwd}" = "y"
  then
    pgpass_line="${dbhost}:${dbport}:*:${dbuser}:${dbpasswd}"
    pgpass_linedb="${dbhost}:${dbport}:${dbname}:${dbuser}:${dbpasswd}"
    if test "`cat ~/.pgpass | grep "^${pgpass_line}$"`" = "${pgpass_line}" || test "`cat ~/.pgpass | grep "^${pgpass_linedb}$"`" = "${pgpass_linedb}"
    then
      >&2 echo "WARNING: User password was not added to ~/.pgpass file due to it is already contained in it."
    else
      echo "${pgpass_line}" >> ~/.pgpass
    fi
  fi
  
  psql="`$(getPgConfigPath) --bindir`/psql"
  
  req_create_db=`qYesNo "Would you like to create database \"${dbname}\" in PostgreSQL?"`
  if test "${req_create_db}" = "y"
  then
    if test $is_localhost
    then
      createdb "${dbname}" -O "${dbuser}"
    else
      >&2 echo "WARNING: User need to have PERMISSION to create database."
      "${psql}" -h "${dbhost}" -p ${dbport} -U "${dbuser}" -d "postgres" -c "CREATE DATABASE ${dbname};"
    fi
  fi
  
#  req_clean=`qYesNo "Would you like to clean existing VTApi data in DB?"`
#  if test "${req_clean}" = "y"
#  then
#    "${psql}" -h "${dbhost}" -p ${dbport} -U "${dbuser}" -d "${dbname}" -c "SELECT public.VT_dataset_drop_all();"
#  fi

  req_default_dataset=`qYesNo "Would you like to reset VTApi infrastructure data?"`
  if test "${req_default_dataset}" = "y"
  then
    initdir=${0%/*};
    "${psql}" -h "${dbhost}" -p ${dbport} -U "${dbuser}" -d "${dbname}" -f "${initdir}/sql/postgresql/pg_createdb.sql"
  fi
fi