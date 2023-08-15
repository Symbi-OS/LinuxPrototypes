set -x
taskset -c 1 bash -c '../../artifacts/redis/fed36/redis-server --protected-mode no --save '' --appendonly no' 
