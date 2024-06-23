ps aux | grep "./raft" | awk '{print $2}' | xargs kill -9
#rm -rf persister-*