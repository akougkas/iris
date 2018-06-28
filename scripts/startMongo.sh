#!/usr/bin/env bash
mkdir /tmp/logs
sudo rm -r /home/anthony/temp/shard*
mkdir -p /home/anthony/temp/shard0/rs0
mkdir -p /home/anthony/temp/shard1/rs0
mkdir -p /home/anthony/temp/shard2/rs0
mongod --replSet set0 --logpath "/tmp/logs/rs0_37017.log" --dbpath /home/anthony/temp/shard0/rs0 --port 37017 --fork --shardsvr --oplogSize 50 --timeStampFormat "ctime"
mongod --replSet set1 --logpath "/tmp/logs/rs0_37018.log" --dbpath /home/anthony/temp/shard1/rs0 --port 37018 --fork --shardsvr --oplogSize 50 --timeStampFormat "ctime"
mongod --replSet set2 --logpath "/tmp/logs/rs0_37018.log" --dbpath /home/anthony/temp/shard2/rs0 --port 37019 --fork --shardsvr --oplogSize 50 --timeStampFormat "ctime"
sleep 3
mongo -port 37017 << EOF
config = { _id: "set0", members:[{ _id : 0, host : "localhost:37017" }]};
rs.initiate(config);
EOF
mongo -port 37018 << EOF
config = { _id: "set1", members:[{ _id : 0, host : "localhost:37018" }]};
rs.initiate(config);
EOF
mongo -port 37019 << EOF
config = { _id: "set2", members:[{ _id : 0, host : "localhost:37019" }]};
rs.initiate(config);
EOF
sudo rm -r /home/anthony/temp/config
mkdir -p /home/anthony/temp/config
mongod --replSet configset --logpath "/tmp/logs/cfg.log" --dbpath /home/anthony/temp/config --port 57040 --fork --configsvr
mongo -port 57040 << EOF
config = { _id: "configset", members:[{ _id : 0, host : "localhost:57040" }]};
rs.initiate(config);
EOF
mongos --logpath "/tmp/logs/mongos.log" --configdb "configset/localhost:57040" --fork --timeStampFormat "ctime"
mongo -port 27017 << EOF
db.adminCommand( { addshard : "set0/localhost:37017" } );
db.adminCommand( { addshard : "set1/localhost:37018" } );
db.adminCommand( { addshard : "set2/localhost:37019" } );
EOF