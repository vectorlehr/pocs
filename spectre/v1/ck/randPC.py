import os, sys, subprocess, re, time



def randPC():
    asmPattern = '''
    .rep {}
    nop
    .endr
    pushf
    lea safe, %r14
    mov %si, (%r14)
    # mfence

dump:
    nop
    nop
    .rep 10
    imul $1, %rdi
    .endr
    mov %rdi, %rax
    shl $8, %rax
    sahf                   # save flags
    
    jc target

.global safe
safe:
    nop
    nop
    mov $0x61, %rax
    shl $12, %rax
    mov (%rdx, %rax, 1), %r13b

    lahf
    popf
    ret


.global target
target:
    nop
    nop
    mov $0x40, %rax
    shl $12, %rax
    mov (%rdx, %rax, 1), %r13b
    
    lahf
    popf                # 9d
    ret                 # c3


'''
    # print(asmPattern.format(1000))

    head = '''
.text

.global vic
.align 0x1000
vic:
    '''

    valid_offset = []

    #0x492
    # 0x500
    # 0x592
    # 0x5c1
    # 0x777
    # 0x798
    # 0x879
    # 0x955
    # 0x957
    # 0x9d2
    # 0xb1d
    # 0xb60
    # 0xb8a
    # 0xbb9
    # 0xe28
    # 0xe88
    # 0xeea
    # 0xf03

    for i in range(0x0, 0x1000):
    # for i in [0x492, 0x500, 0x592, 0x5c1, 0x777, 0x798, 0x879, 0x955, 0x957, 0x9d2, 0xb1d, 0xb60, 0xb8a, 0xbb9, 0xe28, 0xe88, 0xeea, 0xf03]:
        print('offset: {}'.format(i))
        with open('vic.S', 'w') as f:
            f.write(head)
            f.write(asmPattern.format(i))
            f.write('\n')
        # os.system('make clean && make')
        try:
            subprocess.run('make clean > /dev/null && make > /dev/null', shell=True)

            # subprocess.run('taskset -c 1 ./v1 > /dev/null &', shell=True)

            with subprocess.Popen('sudo taskset -c 10 ./client', shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE) as p:
                out, err = p.communicate()
                out = out.decode('utf-8')
                err = err.decode('utf-8')

                # print(out)
                # idx: 62 score: 1000, second_idx:61, second score: 2
                # 提取idx，socre，second_idx，second_score
                # print(out)
                idx = re.findall(r'idx: (\d+)', out)
                if len(idx) == 0:
                    print('idx not found')
                    continue
                idx = int(idx[0], 16)
                score = re.findall(r'score: (\d+)', out)
                if len(score) == 0:
                    print('score not found')
                    continue
                score = int(score[0])
                second_score = re.findall(r'second score: (\d+)', out)
                if len(second_score) == 0:
                    print('second_score not found')
                    continue
                second_best = re.findall(r'second_idx:([0-9a-f]+)', out)
                if len(second_best) == 0:
                    print('second_best not found')
                    continue
                second_score = int(second_score[0], 10)
                second_best = int(second_best[0], 16)
                print(second_score, second_best, idx)
                if second_best in [0x61, 0x62] and idx in [0x61, 0x62] and (second_score + score) > 1010:
                    valid_offset.append(i)
                    with open('valid_offset.txt', 'a+') as f:
                        f.write(hex(i) + '\n')

                    print(out)

        except Exception as e:
            print(e)
            # subprocess.run('pkill -9 v1', shell=True)
        
        finally:
            pass

            # subprocess.run('pkill -9 v1', shell=True)

    for offset in valid_offset:
        print(hex(offset))


if __name__ == "__main__":
    randPC()