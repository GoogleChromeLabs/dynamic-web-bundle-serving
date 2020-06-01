#!/bin/bash

{ time brotli -q $1 -f -o $3 $4 ; } 2> $2
