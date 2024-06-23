#include "RaftRPC.h"
RaftGrpcImpl::RaftGrpcImpl(){}
RaftGrpcImpl::~RaftGrpcImpl() {}
::grpc::Status RaftGrpcImpl::AppendEntries(::grpc::ServerContext *context,
                                           const rf::AppendEntriesRequest *request,
                                           rf::AppendEntriesResponse *response)
{
    rf::AppendEntriesResponse res=appcb(request);
    response->set_m_conflict_index(res.m_conflict_index());
    response->set_m_conflict_term(res.m_conflict_term());
    response->set_m_success(res.m_success());
    response->set_m_term(res.m_term());
    return grpc::Status::OK;
}

::grpc::Status RaftGrpcImpl::Vote(::grpc::ServerContext* context,
                    const rf::RequestVote* request,
                    rf::ResponseVote* response){
    rf::ResponseVote res=votecb(request);
    response->set_isaccepted(res.isaccepted());
    response->set_term(res.term());
    return grpc::Status::OK;
}

::grpc::Status RaftGrpcImpl::InstallSnapShot(::grpc::ServerContext* context,
                                const rf::InstallSnapShotRequest* request,
                                rf::InstallSnapShotResponse* response){
    
}

void RaftGrpcImpl::SetAppendCallBack(std::function<rf::AppendEntriesResponse(const rf::AppendEntriesRequest*)> fn){
    appcb = fn;
}

void RaftGrpcImpl::SetVoteCallBack(std::function<rf::ResponseVote(const rf::RequestVote*)> fn){
    votecb = fn;
}

void RaftGrpcImpl::SetInstallSnapShotCallBack(std::function<void()> fn){
    snapcb = fn;
}