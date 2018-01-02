# Virtualbox machine with VTApi + VTServer in Docker container
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