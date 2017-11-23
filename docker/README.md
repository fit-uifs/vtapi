# VTApi + VTServer as a Docker container
## Build VTApi + VTServer Docker container
```shell
docker build -t vtapidev .
```

## Useful parameters for docker run
* -h vtapidev => hostname of container
* -v <srcdir>:<mountdir> => mount <srcdir> folder of host to <mountdir> folder of container
* -p <srcport>:<dstpost> => forward <srcport> port of host to <dstport> port of container
* --rm => container behaviour is like a snapshot - already starts from default state

## Run of VTApi + VTServer Docker container
```shell
docker run -it <parameters> --name vtapidev vtapidev   # creates container and runs it also
```

## Other useful Docker commands
```shell
docker start vtapidev    # starts container
docker stop vtapidev     # stops running container
docker attach vtapidev   # attaches running container
docker ps                # list of containers
```

&nbsp;

---

# Some notes for Windows
## Using VirtualBox? Don't have Windows X Professional Edition?
If you don't have Professional Edition of Windows with Hyper-V support, you need to use "Docker Toolbox"
(https://www.docker.com/products/docker-toolbox).

If you use VirtualBox (it can't be combined with Hyper-V), I recommend to use "Docker Toolbox" also.

## Docker Toolbox
If you use Docker Toolbox, there will be created VirtualBox machine, in which Docker is processed.

**Be carefull of shared folders**, because there is supported only C:\Users directory, which is shared
to this VirtualBox machine and which you can mount to docker container. If you would like to share
another location, there is small workaround:
```shell
docker-machine stop default
VBoxManage sharedfolder add default --name "/mount/path/in/virtual/machine" --hostpath "D:\some\folder\to\share" --automount
docker-machine start default
```

Thereafter you can mount this folder to container without any further problems:
```shell
docker run -it <parameters> -v /mount/path/in/virtual/machine:/mount/path/in/container --name vtapidev vtapidev
```