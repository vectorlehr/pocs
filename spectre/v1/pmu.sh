#!/bin/bash
# echo 16 | sudo tee  /proc/sys/vm/nr_hugepages 1>/dev/null
#allows user processes to access performance counters via the rdpmc instruction
sudo bash -c "echo 2 > /sys/bus/event_source/devices/cpu/rdpmc || exit"

sudo modprobe msr

# enable all IA32_PERF_GLOBAL_CTRL lower bits
sudo wrmsr -p 1 0x38F 0xf


# write event number to IA32_PERFEVTSEL0

# LD_BLOCKS.STORE_FORWARD
# Loads blocked by overlapping with store buffer that cannot be forwarded.
# sudo wrmsr -a 0x186 0x00410203

#LD_BLOCKS.NO_SR


# write event id to IA32_PERFEVTSELx

# MACHINE_CLEARS.MEMORY_ORDERING to IA32_PERFEVTSEL0
# BR_MISP_RETIRED.ALL_BRANCHES_PS
sudo wrmsr -p 1 0x186 0x004304C5

# MACHINE_CLEARS_COUNT
#  BR_MISP_RETIRED.CONDITIONAL_PS
sudo wrmsr -p 1 0x187 0x004301C5

# LD_BLOCKS.STORE_FORWARD
# BR_MISP_RETIRED.NEAR_TAKEN 0x004120C5
# ITLB.ITLB_FLUSH 
sudo wrmsr -p 1 0x188 0x00433FC1

# OTHER_ASSISTS.ANY in ia32_pmc2
# sudo wrmsr -a 0x188 0x00413FC1
# MEM_INST_RETIRED_LOCK_LOADS
# sudo wrmsr -a 0x188 0x004121D0

# MEM_LOAD_RETIRED.FB_HIT in ia32_pmc3
# FRONTEND_RETIRED.L1I_MISS
sudo wrmsr -p 1 0x189 0x004301C6




# bind another four events on core 2

#LD_BLOCKS_PARTIAL.ADDRESS_ALIAS
sudo wrmsr -p 36 0x186 0x00410107

# INT_MISC.CLEARS_COUNT
sudo wrmsr -p 36 0x187 0x0041010D

# LONGEST_LAT_CACHE.MISS
sudo wrmsr -p 36 0x188 0x0041412E

# LONGEST_LAT_CACHE.REFERENCE
sudo wrmsr -p 36 0x189 0x00414F2E