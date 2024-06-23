#include <iostream>
#include <functional>
#include <mutex>
#include "raft.grpc.pb.h"
class RaftGrpcImpl:public rf::RaftServerRpc::Service{
public:
    RaftGrpcImpl();
    virtual ~RaftGrpcImpl();
    /*
        rpc  AppendEntries(AppendEntriesRequest) returns(AppendEntriesResponse);
        rpc  Vote(RequestVote) returns(ResponseVote);
        rpc  InstallSnapShot(InstallSnapShotRequest) returns(InstallSnapShotResponse);
    */
    ::grpc::Status AppendEntries(::grpc::ServerContext* context, const rf::AppendEntriesRequest* request, rf::AppendEntriesResponse* response);
    ::grpc::Status Vote(::grpc::ServerContext* context, const rf::RequestVote* request, rf::ResponseVote* response);
    ::grpc::Status InstallSnapShot(::grpc::ServerContext* context, const rf::InstallSnapShotRequest* request, rf::InstallSnapShotResponse* response);
    void SetAppendCallBack(std::function<rf::AppendEntriesResponse(const rf::AppendEntriesRequest*)> fn);
    void SetVoteCallBack(std::function<rf::ResponseVote(const rf::RequestVote*)> fn);
    void SetInstallSnapShotCallBack(std::function<void()> fn);
private:
    std::function<rf::AppendEntriesResponse(const rf::AppendEntriesRequest*)> appcb;
    std::function<rf::ResponseVote(const rf::RequestVote*)> votecb;
    std::function<void()> snapcb;
    std::mutex mtx;
};