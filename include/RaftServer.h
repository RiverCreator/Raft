#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <chrono>
#include <unistd.h>
#include <fcntl.h>
#include <thread>
#include <condition_variable>
#include "ThreadPool.h"
#include "RaftRPC.h"
#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/grpcpp.h>
#include "raft.grpc.pb.h"
#define HEART_BEART_PERIOD 100000
/**
 * @brief 记录其他节点的ip和port
 * 
 */
struct PeersInfo{
    int m_peerId;
    std::string ip;
    int port;
};

/**
 * @brief 记录LogEntry的指令和任期号
 * 
 */
struct LogEntry{
    LogEntry(std::string cmd="",int term =-1):m_cmd(cmd),m_term(term){}
    std::string m_cmd;
    int m_term;
};
/**
 * @brief 持久化内容
 * @param logs 持久化的log
 * @param cur_term 当前任期
 * @param voteFor 投票给谁了
 */
struct Persister{
    std::vector<LogEntry> logs;
    int cur_term;
    int votedFor;
};

/**
 * @brief 提交日志 将加入的日志的index和term 以及当前节点是不是leader返回
 * 
 */
struct StartRet{
    StartRet():m_cmdIndex(-1), m_curTerm(-1), isLeader(false){}
    int m_cmdIndex;
    int m_curTerm;
    bool isLeader;
};

/**
 * @brief 记录操作 
 * 
 */
struct Operation{
    std::string getCmd(){
        return op + " " + key + " " + value;
    }
    std::string op;
    std::string key;
    std::string value;
    //下面两个参数暂时没用
    int clientId;
    int requestId;
};

class Raft{
public:
    Raft(std::string ip,int port);
    ~Raft();
    // //监听投票
    // void ListenForVote();
    // //监听Append
    // void ListenForAppend();
    //处理日志同步
    void ProcessEntriesLoop();
    //选举超时的loop
    void ElectionLoop();
    //发起重新选举 请求投票
    void CallRequestVote(int clientPeerId);
    //回复Vote
    rf::ResponseVote ReplyVote(const rf::RequestVote* request);
    //回复Append
    rf::AppendEntriesResponse ReplyAppend(const rf::AppendEntriesRequest* request);
    //发起AppendRPC的线程
    void SendAppendEntries(int clientPeerId);
    //向上层应用日志的守护线程
    void ApplyLogLoop();
    //定义节点状态
    enum RAFT_STATE {LEADER=0,CANDIDATE,FOLLOWER};
    //初始化，预先记录其他节点id和ip：port，设定当前id
    void Init(std::vector<PeersInfo>& peers, int id); 
    //设置广播心跳时间
    void SetBellTime();
    //获取当前节点termId，是否leader
    std::pair<int,bool> GetState();
    //判断当前节点日志是否可以给CANDIDATE投票 传入任期号和日志index，在vote的时候调用
    bool CheckLogUptodate(int term, int index);
    //插入新日志
    void PushBackLog(LogEntry log);
    //持久化
    void SaveRaftState();
    //读取持久化状态
    void ReadRaftState();
    //停止
    void Stop();
    //解析string cmd
    std::vector<LogEntry> GetCmdAndTerm(std::string text);
    //反序列化 读取持久化到文件中的内容
    bool Deserialize();
    //序列化 将log entry持久化到文件中
    void Serialize();
    //计算间隔时间
    int GetDuration(timeval last);
    //RPC服务loop线程
    void RPCLoop(std::string ip,int port);
    //用于接收cmd后向Raft层提交日志
    StartRet Start(Operation op);
private:
    std::vector<PeersInfo> m_peers; //记录的其他节点的id, ip和port
    Persister persister;
    int peer_id; //当前节点id
    bool m_stop;
    std::vector<LogEntry> m_logs; //当前内存中的log

    int m_curTerm; // 当前任期
    int m_voteFor; // 表示要投票给谁 因为有可能多个同时发起投票，而只能投票给一个，如果投过票了并且发起投票节点的任期id大于当前，则继续投票
    std::vector<int> m_nextIndex; //将要赋值给从节点的日志索引
    std::vector<int> m_matchIndex; //每个从节点匹配上leader的日志索引
    int m_lastApplied; //最后被应用到状态机的log entry index
    int m_commitIndex; //将被提交的log entry index

    int recvVotes; //如果是candidate的话，接收到的投票数量
    int finishedVote; //完成投票的数量
    int cur_peerId; //记录candidate请求vote到哪一个从机 或appendentry到哪一个从机

    RAFT_STATE m_state; //当前所处什么角色状态
    int m_leaderId; //认定的leader id
    struct timeval m_lastWakeTime; //记录follower最后一次接收到心跳的时间
    struct timeval m_lastBroadcastTime; //记录leader最后一次发送心跳的时间
    
    std::mutex mtx;
    std::condition_variable cond_;

    RaftGrpcImpl rpc_server;
    std::vector<std::unique_ptr<rf::RaftServerRpc::Stub>> stub_ptrs_;
    std::vector<int> dead;
    std::vector<std::shared_ptr<grpc_impl::Channel>> stub_channels_;
    std::thread rpc_thread;
    std::thread process_entry_thread; //同步log entry还有心跳包 loop
    std::thread election_loop_thread; //超时选举loop
    std::thread applylog_thread; //真正提交log loop
    ThreadPool& thread_pool_; //vote和append任务的线程池
};