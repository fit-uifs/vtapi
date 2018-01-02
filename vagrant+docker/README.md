# Virtualbox machine with VTApi + VTServer in Docker container
This can create new VirtualBox machine, where VTApi is placed in Docker container. Moreover, this script can create machine with full graphical interface of underlying OS (Ubuntu), because of interactive startup.

## Create VirtualBox machine
```shell
vagrant up
```

## Useful parameters for vagrant
* ssh => SSH connection to VirtualBox machine
* suspend => save curent state of VirtualBox machine and suspend it
* destroy => destroy VirtualBox machine

## Run VTApi + VTServer Docker container
```shell
docker run -it -h vtapidev -p 5432:5432 --name vtapidev vtapidev
```

### Docker how to
For more information about Docker, move to [Docker](../docker) part of VTApi.