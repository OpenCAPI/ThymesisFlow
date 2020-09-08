## Libthymesisflow

Low levels APIs and configuration scripts to test and setup disaggregated memory 
via [ThymesisFlow](https://github.com/OpenCAPI/ThymesisFlow).

We consider two possible scenarios. 
The first case is a mock of the AFUs (Accelerator Function Unit ) setup. 
This is mainly used for unit testing the APIs.
The second scenario describes step by step how to setup remote memory using 
Thymesisflow AFUs on Power 9 machines.

- [Dependencies](#dependencies)
- [Build Mock](#build-mock)
- [Unit Tests](#unit-tests)
- [Remote Memory Setup](#remote-memory-setup)
- [Find your AFU](#find-your-afu)


### Dependencies

```
sudo apt-get install libocxl-dev
```


### Build Mock 

Build the library as following in case you want to test client and server interactions

```
make MOCK=1
```

Add `DEBUG=1` for debugging information

### Unit Tests

The following steps describe how to setup and run unit tests.

The first step requires to build unit tests.

```
make tests MOCK=1
```

To start the Thymesisflow agent, run the following command:

```
./bin/thymesisf-agent
```

To launch unit tests, run the following command.

```
./testbin/unittest
```


### Remote memory setup


In this scenario we assume that you flashed both memory and compute design on your FPGAS.
If not, you can find the instructions to [bring up the design](https://github.com/OpenCAPI/ThymesisFlow#how-to-bring-up-the-design).

While the [mock](#build-mock) build is used for testing, in this scenario each requests will configure compute or memory node AFUs.
For this scenario, recompile agent and CLI as following. 

```
make
```

**Note:** use `DEBUG=1` for debugging information

On both Compute and Memory side you can now run the server process. 

```
sudo ./bin/thymesisf-agent -s <SOCKET_PATH>
```

Where `SOCKET_PATH` is the path of the UNIX stream socket. 
This is used for local communications between agent and clients.
The default value is `/tmp/thymesisflow.sock`.

#### Setup Memory side

On the **Memory node** you can reserve memory as following:

```
sudo ./bin/thymesisf-cli attach-memory \
    --afu <AFU_NAME> \
    --cid <CIRCUIT_ID> \
    --size <MEMORY_SIZE>
```

Where:

* `AFU_NAME` is the prefix of the character device that can be found in `/dev/ocxl/`. Reference [Find your AFU](#find-your-afu)

* `CIRCUIT_ID` is an alphanumeric string representing your connection
* `MEMORY_SIZE` represents the amount of memory (in bytes) to be reserved. This needs to be a multiple of 256 MB

An output example:

```
2020-07-29 16:26:55, INFO ,Successfully allocated memory - EA: 0x79a3ff0d0080 
```

`EA` is the effective address required to configure the compute side. 

#### Setup Compute side

On the **Compute node** we can now attach the remote memory as following:

```
sudo ./bin/thymesisf-cli attach-compute \
    --afu <AFU_NAME> \
    --cid <CIRCUIT_ID> \
    --size <MEMORY_SIZE> \
    --ea <EFFECTIVE_ADDRESS>
```

Where:

* `AFU_NAME` is the prefix of the character device that can be found in `/dev/ocxl/`. Reference [Find your AFU](#find-your-afu)
* `CIRCUIT_ID` is an alphanumeric string representing your connection
* `MEMORY_SIZE` represents the amount of memory (in bytes) to be reserved. This needs to be a multiple of 256 MB
* `EFFECTIVE_ADDRESS` address returned as `EA` during the [memory side setup](# Setup Memory side)

#### Stats counters

To keep track of the traffic on the channel, Thymesisflow provides three counters reporting:

* `rx (flits/sec)`: number of flits read from the channel.
* `tx (flits/sec)`: number of flits transmitted on the channel.
* `latency (cycles/sec)`: average latency on the channel.

**Note:** Each counter resets at each read. Therefore, a read each seconds would report the `flits/sec`.
Given that each flit is 32B, you can compute `rx` and `tx` bandwidth in `B/s` respectively by multiplying `tx*32` and `rx*32`

To read the counters you can run the following command on the compute node:

```
python ./scripts/read_counters.py
```

### Detach remote memory

Once the connection is not needed anymore, we can release the preallocated resources.

**Note:** it is necessary to detach the compute side first to avoid machine faults.


#### Detach Compute side

```
sudo ./bin/thymesisf-cli detach-compute --cid <CIRCUIT_ID>
```

#### Detach Memory side

```
sudo ./bin/thymesisf-cli detach-compute --cid <CIRCUIT_ID>
```


### Find your AFU

AFUs respective character devices are available in `/dev/ocxl/` 
using the pattern `/dev/ocxl/<AFU name>.<location>.<index>`.

You can inspect the available AFUs as following:
```
$ ls /dev/ocxl/
IBM,RMEM.0006:00:00.1.0
```

In case of Thymesisflow, AFUs are exposed with the name `IBM,RMEM`.

