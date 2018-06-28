#!/usr/bin/env bash
echo "Database ${1} -> sharding collection ${2}..."
mongo --port 27017 > /tmp/enableSharding.log << EOF
use $1;
sh.stopBalancer();
sh.getBalancerState();
sh.enableSharding("${1}");
sh.shardCollection("$1.$2",{_id:"hashed"}, false, {numInitialChunks:64});
use config;
db.settings.save( { _id:"chunksize", value: 4 } );
db.getSiblingDB("admin").runCommand( { setParameter: 1, syncdelay: 0 } );
EOF