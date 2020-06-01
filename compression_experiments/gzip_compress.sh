#!/bin/bash

{ time gzip -$1 -f -k -c $4 > $3; } 2> $2
