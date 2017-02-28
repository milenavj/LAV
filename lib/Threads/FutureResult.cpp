#include "lav/Threads/FutureResult.h"

using namespace Threads;


FutureResult::FutureResult() 
 :m_exec_status{FutureResult::ExecStatus::Pending}, m_promise{std::promise<int>()}, m_shared_future{m_promise.get_future().share()}
	{}


FutureResult::FutureResult(FutureResult &&ft)
:m_exec_status{std::move(ft.m_exec_status)}, m_promise{std::move(ft.m_promise)}, m_shared_future{std::move(ft.m_shared_future)}
	{}

	

FutureResult& FutureResult::operator=(FutureResult &&oth)
	{
		if(&oth != this)
		{
			m_exec_status = std::move(oth.m_exec_status);
			m_promise = std::move(oth.m_promise);
			m_shared_future = std::move(oth.m_shared_future);
		}
		return *this;
	}

FutureResult::~FutureResult() {}


void FutureResult::setResult(int val)
	{
		m_promise.set_value(val);
	}

std::shared_future<int>& FutureResult::getSharedFuture()
	{
		return m_shared_future;
	}

FutureResult::ExecStatus FutureResult::getStatus()
	{
		return m_exec_status;
	}

void FutureResult::setStatus(FutureResult::ExecStatus es)
	{
		m_exec_status = es;
	}
