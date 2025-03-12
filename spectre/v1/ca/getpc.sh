#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color
RESET='\033[0m'

echo -e "${RED}=========v1=========${NC}"
objdump -d v1 | grep --color=always "safe>:" -B2
echo -e "${RED}=========vic=========${NC}"
objdump -d vic | grep --color=always "safe>:" -B2