set -x
taskset -c 1 bash -c 'shortcut.sh -be -s "write->__x64_sys_write" -s "read->__x64_sys_read" --- ../../artifacts/redis/fed36/redis-server --protected-mode no --save '' --appendonly no' 
