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