# iris
## Sumamry
There is an ocean of available storage solutions in modern high- performance and distributed systems. These solutions consist of Parallel File Systems (PFS) for the more traditional high-performance computing (HPC) systems and of Object Stores for emerging cloud environments. In this project, we propose a unified storage access system, called IRIS (i.e., I/O Redirection via Integrated Storage). IRIS enables unified data access and seamlessly bridges the semantic gap between file systems and object stores. With IRIS, emerging High-Performance Data Analytics software has capable and diverse I/O support. IRIS can bring us closer to the convergence of HPC and Cloud environments by combining the best storage subsystems from both worlds. Experimental results show that IRIS can grant more than 7x improvement in performance than existing solutions.

## Who do I talk to?
* Research Assistant Professor Anthony Kougkas akougkas@iit.edu
* Hariharan Devarajan (PhD) hdevarajan@hawk.iit.edu
* Professor Xian-He Sun sun@iit.edu

### How to compile MPICH
`./configure --prefix /opt/mpich --enable-fast=03 --enable-shared --enable-romio --enable-threads --disable-fortran --disable-fc --with-pvfs2=/opt/orangefs/`
### TCMALLOC
`sudo apt-get install libgoogle-perftools-dev`
### MongoDB
`sudo service mongod start`

`cat /var/log/mongodb/mongod.log`

`sudo service mongod stop`

* LOCALLY:

`mkdir /tmp/logs`

`sudo rm -r /home/anthony/temp/shard*`

`mkdir -p /home/anthony/temp/shard0/rs0`

`mkdir -p /home/anthony/temp/shard1/rs0`

`mkdir -p /home/anthony/temp/shard2/rs0`

`mongod --replSet set0 --logpath "/tmp/logs/rs0_37017.log" --dbpath /home/anthony/temp/shard0/rs0 --port 37017 --fork --shardsvr --oplogSize 50 --timeStampFormat "ctime"`

`mongod --replSet set1 --logpath "/tmp/logs/rs0_37018.log" --dbpath /home/anthony/temp/shard1/rs0 --port 37018 --fork --shardsvr --oplogSize 50 --timeStampFormat "ctime"`

`mongod --replSet set2 --logpath "/tmp/logs/rs0_37018.log" --dbpath /home/anthony/temp/shard2/rs0 --port 37019 --fork --shardsvr --oplogSize 50 --timeStampFormat "ctime"`

`sleep 3`

`mongo -port 37017 << EOF
config = { _id: "set0", members:[{ _id : 0, host : "localhost:37018" }]};
rs.initiate(config);
EOF`

`mongo -port 37018 << EOF
config = { _id: "set1", members:[{ _id : 0, host : "localhost:37018" }]};
rs.initiate(config);
EOF`

`mongo -port 37019 << EOF
config = { _id: "set2", members:[{ _id : 0, host : "localhost:37019" }]};
rs.initiate(config);
EOF`

`sudo rm -r /home/anthony/temp/config`

`mkdir -p /home/anthony/temp/config`

`mongod --replSet configset --logpath "/tmp/logs/cfg.log" --dbpath /home/anthony/temp/config --port 57040 --fork --configsvr`

`mongo -port 57040 << EOF
config = { _id: "configset", members:[{ _id : 0, host : "localhost:57040" }]};
rs.initiate(config);
EOF`

`mongos --logpath "/tmp/logs/mongos.log" --configdb "configset/localhost:57040" --fork --timeStampFormat "ctime"`

`mongo -port 27017 << EOF
db.adminCommand( { addshard : "set/localhost:37017" } );
db.adminCommand( { addshard : "set/localhost:37018" } );
db.adminCommand( { addshard : "set/localhost:37019" } );
EOF`

### DEPENDENCIES
`sudo apt-get install pkg-config libssl-dev libsasl2-dev`

`wget https://github.com/mongodb/libbson/releases/download/1.6.1/libbson-1.6
.1.tar.gz`

`tar xzf libbson-1.6.1.tar.gz && cd libbson-1.6.1`

`./configure --prefix=/usr --libdir=/usr/lib --enable-static --disable-automatic-init-and-cleanup
make && sudo make install`

`wget https://github.com/mongodb/mongo-c-driver/releases/download/1.6.1/mongo-c-driver-1.6.1.tar.gz`

`tar xzf mongo-c-driver-1.6.1.tar.gz && cd mongo-c-driver-1.6.1`

`./configure --prefix=/usr --libdir=/usr/lib --enable-static --disable-automatic-init-and-cleanup`

`make && sudo make install`

`wget https://github.com/mongodb/mongo-cxx-driver/archive/r3.1.1.tar.gz`

`tar -xzf r3.1.1.tar.gz && cd mongo-cxx-driver-r3.1.1/build`

`export PKG_CONFIG_PATH=/usr/lib`

`cmake -DCMAKE_BUILD_TYPE=Release -DBSONCXX_POLY_USE_MNMLSTC=1 -DCMAKE_INSTALL_PREFIX=/usr/local ..`

`sudo make EP_mnmlstc_core`

`make && sudo make install`

### OrangeFS
Compile with:

`sudo apt-get install linux-headers-$(uname -r)`

`wget https://s3.amazonaws.com/download.orangefs.org/current/source/orangefs-2.9.6.tar.gz`

`tar -xzf orangefs-2.9.6.tar.gz && cd orangefs-2.9.6`

`./configure --prefix=/opt/orangefs --with-kernel=/lib/modules/`uname -r`/build --enable-shared`

`make && sudo make install && make kmod && sudo make kmod_prefix=/opt/orangefs kmod_install`

Fix the linker:

`add /opt/orangefs/lib to /etc/ld.so.conf.d/libc.conf`

`add /opt/orangeds/sbin and bin to PATH`

`sudo ldconfig`

Start the scripts:

`/opt/orangefs/bin/pvfs2-genconfig /opt/orangefs/etc/orangefs-server.conf`

`/opt/orangefs/sbin/pvfs2-server -f -a localhost /opt/orangefs/etc/orangefs-server.conf`

`/opt/orangefs/sbin/pvfs2-server -a localhost /opt/orangefs/etc/orangefs-server.conf`

`sudo insmod /opt/orangefs/lib/modules/4.4.0-83-generic/kernel/fs/pvfs2/pvfs2.ko`


`sudo /opt/orangefs/sbin/pvfs2-client -p /opt/orangefs/sbin/pvfs2-client-core`

`sudo mount -t pvfs2 tcp://localhost:3334/orangefs /mnt/orangefs`

Add this under <FileSystem> to fs_config.conf

    <StorageHints>
          DirectIOThreadNum 30 (48)          
          DirectIOOpsPerQueue 10 (20)          
     </StorageHints>

## How to tune the cluster

DISK:
READ-AHEAD TUNING

`blockdev --report`

`blockdev --setra 16384 /dev/sda` (16384 times of 512 bytes blocks).

Turning off disk read-ahead and write cache

`sudo hdparm -W 0 /dev/sda`

`sudo hdparm -A 0 /dev/sda`

`sudo hdparm -a 0 /dev/sda`

`sudo gedit /etc/fstab`  and add/delete sync,noatime,nodiratime

LIMITS:

`sudo vim /etc/security/limits.conf`

* soft nofile 65536
* hard nofile 65536

`sudo vim /etc/pam.d/common-session`

session required pam_limits.so

`sudo vim /etc/pam.d/common-session-noninteractive`

session required pam_limits.so

NETWORK:

`sudo sysctl -w net.core.rmem_max=8388608`

`sudo sysctl -w net.core.wmem_max=8388608`

`sudo sysctl -w net.ipv4.tcp_rmem=8388608`

`sudo sysctl -w net.ipv4.tcp_wmem=8388608`

check with:

`sysctl -a | fgrep tcp`

TCP_KEEP_ALIVE (recommended 120)

`sudo sysctl -w net.ipv4.tcp_keepalive_time=120`

LD_PRELOAD tutorial
https://github.com/babakbehzad/Recorder/blob/master/lib/recorder-hdf5.c
