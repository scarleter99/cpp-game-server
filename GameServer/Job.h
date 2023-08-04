#pragma once

class IJob
{
public:
	virtual void Execute() { }
};


class HealJob : public IJob
{
public:
	virtual void Execute() override
	{
		// _target은 찾아서
		// _target->AddHP(_healValue);
		cout << _target << "한테 힐" << _healValue << " 만큼 줌";
	}

public:
	uint64 _target = 0;
	uint32 _healValue = 0;
};

using JobRef = shared_ptr<IJob>;

class JobQueue
{
public:
	void Push(JobRef job)
	{
		WRITE_LOCK;
		_jobs.push(job);
	}

	JobRef Pop()
	{
		WRITE_LOCK;
		if (_jobs.empty())
			return nullptr;

		JobRef ret = _jobs.front();
		_jobs.pop();
		return ret;
	}

private:
	USE_LOCK;
	queue<JobRef> _jobs;
};