#!/bin/bash
ant clean
ant build
dos2unix ./linux/work/arduino
ant run
