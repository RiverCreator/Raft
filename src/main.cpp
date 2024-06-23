#include "RaftServer.h"
#define COMMON_PORT 1629
using namespace std;
int main(int argc, char* argv[]){
    if(argc < 3){
        printf("loss parameter of peersNum and clientid\n");
        exit(-1);
    }
    int peersNum = atoi(argv[1]);
    if(peersNum % 2 == 0){
        printf("the peersNum should be odd\n");  //必须传入奇数，这是raft集群的要求
        exit(-1);
    }
    int clientid = atoi(argv[2]);
    srand((unsigned)time(NULL));
    vector<PeersInfo> peers(peersNum);
    for(int i = 0; i < peersNum; i++){
        peers[i].m_peerId = i;
        peers[i].port = COMMON_PORT + i;                    //vote的RPC端口
        peers[i].ip = "192.168.203.128";
        // printf(" id : %d port1 : %d, port2 : %d\n", peers[i].m_peerId, peers[i].m_port.first, peers[i].m_port.second);
    }
    Raft server(peers[clientid].ip,peers[clientid].port);
    server.Init(peers,clientid);

    //------------------------------test部分--------------------------
    usleep(400000); //等待时间选举成功
    std::cout<<"Start operation"<<std::endl;
    //如果当前是LEADER 则接收1000个操作
    while(1){
        if(server.GetState().second){
            for(int j = 0; j < 1000; j++){
                Operation opera;
                opera.op = "put";
                opera.key = to_string(j);
                opera.value = to_string(j);
                server.Start(opera);
                usleep(50000);
            }
        }
    }
}