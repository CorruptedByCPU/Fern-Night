#!/bin/bash
readelf -s ${1} | sort -k 2
