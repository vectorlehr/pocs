## 使用
```
cd vic
make
sudo insmod leaky.ko
taskset -c [threading1] ./secret
cd atk
make
sudo taskset -c [threading2] ./attacker
```
## 说明
利用present位的MDS变种，本质上是Zombieload
当攻击成功后会输出`S`

## 其他
可以尝试在/etc/default/grub中加入nokaslr nopti