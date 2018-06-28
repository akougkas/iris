#!/usr/bin/env bash
sudo sync && echo 3 > sudo /proc/sys/vm/drop_caches
