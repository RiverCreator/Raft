ulimit -c unlimited
nohup ./raft 3 0 > 0.txt &
sleep 1
nohup ./raft 3 1 > 1.txt &
sleep 1
nohup ./raft 3 2 > 2.txt &
# sleep 1
# nohup ./raft 5 3 > 3.txt &
# sleep 1
# nohup ./raft 5 4 > 4.txt &