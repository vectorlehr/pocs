# smc
这个操作在分支中






1. load/store指令的影响
2. sahf/lahf 和 pushfq/popfq两套指令对BPU的影响 —— 两者成功率都不高，但是lahf/sahf的成功率高一些
3. 在修改flags前加入无关延缓指令 —— 排除
4. 在victim_branch调用前加入mfence —— 对侧信道成功率有效提高, 但是对pht的训练效果没有特别影响





使用sahf/lahf时，eflags前后变化：
```
(gdb) i r eflags
eflags         0x202               [ IF ]
(gdb) n
82          sahf                   # save flags
(gdb) i r eflags
eflags         0x206               [ PF IF ]
(gdb) n
84          jc target_normal
(gdb) i r eflags
eflags         0x203               [ CF IF ]
```

----------

使用pushfq/popfq时，eflags前后变化：
```
(gdb) i r eflags
eflags         0x202               [ IF ]
(gdb) n
79          push %rax
(gdb) i r eflags
eflags         0x202               [ IF ]
(gdb) n
80          popfq
(gdb) i r eflags
eflags         0x202               [ IF ]
(gdb) n
84          jc target_normal
(gdb) i r eflags
eflags         0x203               [ CF IF ]
```