taskset -c 1 bash -c 'shortcut.sh -be -s "read->tcp_recvmsg" -s "write->tcp_sendmsg" --- ../../artifacts/redis/fed36/redis-server --protected-mode no --save '' --appendonly no'
