#! /bin/sh

sudo apt-get update

# Docker install
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"
sudo apt-get update
sudo apt-get install -yqq docker-ce

# git, unzip
sudo apt-get install -yqq git unzip

# Prepare VTApiDev container for Docker
sudo docker build -t vtapidev "/tmp/vtapi-docker/"
sudo docker run -it -h vtapidev -p 5432:5432 --restart=always --name vtapidev vtapidev

# Other configuration
sudo loadkeys cz

# Create custom user if it is required
if test $login != ""
then
  sudo useradd --create-home -G adm,sudo,audio,video,plugdev,netdev,cdrom,floppy $login
  echo "${login}:${password}" | sudo chpasswd > /dev/null
fi

# Install X11 if it is required
if test $GUI -eq 1
then
  sudo apt-get install -yq ubuntu-desktop
  sudo reboot
fi