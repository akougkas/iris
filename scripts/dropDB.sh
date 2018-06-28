#!/usr/bin/env bash
mongo --port 27017 << EOF
use $1;
db.dropDatabase();
EOF