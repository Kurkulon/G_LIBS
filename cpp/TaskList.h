#ifndef TASKLIST_H__15_03_2034__13_39
#define TASKLIST_H__15_03_2034__13_39

#pragma once

#include "types.h"
#include "time.h"

#ifdef WIN32

#include <intrin.h>

#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class TaskList;

class Task
{
	friend class TaskList;

public:

	typedef void TaskProc(void);

protected:

	Task		*volatile _next;
	TaskProc	*volatile _proc;
	u32			_period;
	CTM32		_tm;
	
	void Call() { if (_tm.Check(_period)) _proc(); }

public:

	Task(TaskProc *proc, u32 period) : _next(0), _proc(proc), _period(period) {}
	
	void SetPeriod(u32 period) { _period = period; }

};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class TaskList
{
	friend class Task;

protected:

	Task	*volatile _first;
	Task	*volatile _last;
	Task	*volatile _cur;

	u32		_count;

	__forceinline u32 Lock()			{ return Push_IRQ(); }
	__forceinline void Unlock(u32 t)	{ Pop_IRQ(t);  }

  public:

	TaskList() : _first(0), _last(0), _cur(0), _count(0) {}

	void	Add(Task* t);
	void	Update();

	bool	Empty() { return _first == 0; }

	u32		Count() { return _count; }

};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void TaskList::Add(Task* task)
{
	if (task == 0)	return;
	if (task->_proc == 0) return;

	u32 t = Lock();

	if (_last == 0)
	{
		_cur = _first = _last = task;
	}
	else
	{
		_last->_next = task;
		_last = task;
	};

	task->_next = _first;

	_count++;

	Unlock(t);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void TaskList::Update()
{
	DEBUG_ASSERT(_cur != 0);

	_cur->Call();

	_cur = _cur->_next;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // TASKLIST_H__15_03_2034__13_39
