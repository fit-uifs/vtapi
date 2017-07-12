## DEPENDENCIES

* CMake 2.8.9
* OpenCV 3.2
* PostgreSQL 9.6
* libpqtypes 1.5 (http://libpqtypes.esilo.com)
* SQLite 3.8
* POCO 1.61 (http://pocoproject.org)
* Boost 1.54
* Protocol Buffers 2.5
* ZeroMQ 4.0.4 (http://zeromq.org)
* Python 2.x

## HOW TO BUILD

1. install dependencies from packages (see above)

2. install rpcz
```bash
cd 3rdparty/rpcz
mkdir build
cd build
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

3. a) compile for development (into install/)
```bash
./build_debug.sh
```
OR
3. b) compile for release (into /usr/local)
```bash
./build_release.sh
```

4. optionally install python VTServer client
```bash
cd pyclient
python setup.py install
```


## HOW TO RUN

Development server:
1. configure `./vtapi_debug.conf`
2. run vtserver:
`./run_vtserver_debug.sh`

Release server:
1. create config file like `./vtapi_example.conf`
2. run vtserver:
`vtserver --config=/path/to/config.conf`

Testing the server:
1. install python VTServer client (see above)
2. create testing python script, eg.:
```python
import vtclient.client as vtclient
cl = vtclient.VTServerClient("tcp://127.0.0.1:8719")
addDataset = cl.call('addDataset', {'name': 'test_dataset'})
print cl.call('getDatasetList', {})
cl.call('deleteDataset', {'dataset_id': addDataset['dataset_id']})
```
