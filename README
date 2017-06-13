## DEPENDENCIES
---
* CMake 2.8.9
cmake

* OpenCV 3.2
libopencv-dev

* PostgreSQL 9.6
libpq-dev

* libpqtypes 1.5
libpqtypes-dev
http://libpqtypes.esilo.com

* SQLite 3.8
libsqlite3-dev

* POCO 1.61
libpoco-dev
http://pocoproject.org

* Boost 1.54
libboost-thread-dev libboost-program-options-dev

* Protocol Buffers 2.5
libprotobuf-dev libprotoc-dev

* ZeroMQ 4.0.4
libzmq3-dev
http://zeromq.org

* Python 2.x

## HOW TO BUILD
---

1. install dependencies from packages (see above)

2. install rpcz
```bash
cd 3rdparty/rpcz
mkdir build cd build
cmake ..
make
sudo make install
cd ../python
python setup.py install
```

2. compile interfaces
```bash
./compile_interfaces.sh
```

3a. compile for development (into install/)
```bash
./build_debug.sh
```

3b. compile for release (into /usr/local)
```bash
./build_release.sh
```

3. optionally install python VTServer client
```bash
cd pyclient
python setup.py install
```


## HOW TO RUN
---

Development server:
1. configure `./vtapi_debug.conf`
2. run vtserver:
`./run_vtserver_debug.sh`

Release server:
1. create config file like `./vtapi_example.conf`
2. run vtserver:
`vtserver --config=/path/to/config.conf`

Testing server:
1. install python VTServer client (see above)
2. create testing python script like such:
```python
import vtclient.client as vtclient
cl = vtclient.VTServerClient("tcp://127.0.0.1:8719")
addDataset = cl.call('addDataset', {'name': test_dataset})
print cl.call('getDatasetList', {})
cl.call('deleteDataset', {'dataset_id': addDataset['dataset_id']})
```
