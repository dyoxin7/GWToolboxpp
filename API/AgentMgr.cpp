#include "AgentMgr.h"

GWAPI::AgentMgr::AgentArray GWAPI::AgentMgr::GetAgentArray()
{
	AgentArray* agRet = (AgentArray*)MemoryMgr::agArrayPtr;
	if (agRet->size() == 0) throw 1;
	return *agRet;
}

std::vector<GWAPI::AgentMgr::Agent*> * GWAPI::AgentMgr::GetParty() {
	std::vector<Agent*>* party = new std::vector<Agent*>(GetPartySize());
	AgentArray agents = GetAgentArray();

	for (size_t i = 0; i < agents.size(); ++i) {
		if (agents[i]->Allegiance == 1
			&& (agents[i]->TypeMap & 0x20000)) {

			party->push_back(agents[i]);
		}
	}

	return party;
}

size_t GWAPI::AgentMgr::GetPartySize() {
	size_t ret = 0;
	for (BYTE i = 0; i < 3; ++i) {
		ret += *MemoryMgr::ReadPtrChain<size_t*>(MemoryMgr::GetContextPtr(), 3, 0x4C, 0x54, 0x0C + 0x10 * i);
	}
	return ret;
}

DWORD GWAPI::AgentMgr::GetDistance(Agent* a, Agent* b) {
	return (DWORD)sqrtl((DWORD)(a->X - b->X) * (DWORD)(a->X - b->X) + (DWORD)(a->Y - b->Y) * (DWORD)(a->Y - b->Y));
}

DWORD GWAPI::AgentMgr::GetSqrDistance(Agent* a, Agent* b) {
	return (DWORD)(a->X - b->X) * (DWORD)(a->X - b->X) + (DWORD)(a->Y - b->Y) * (DWORD)(a->Y - b->Y);
}

GWAPI::AgentMgr::AgentMgr(GWAPIMgr* obj) : parent(obj)
{
	_ChangeTarget = (ChangeTarget_t)MemoryMgr::ChangeTargetFunction;
	_Move = (Move_t)MemoryMgr::MoveFunction;
}

void GWAPI::AgentMgr::ChangeTarget(Agent* Agent)
{
	parent->GameThread->Enqueue(_ChangeTarget, Agent->Id);
}

void GWAPI::AgentMgr::Move(float X, float Y, DWORD ZPlane /*= 0*/)
{
	static MovePosition* pos = new MovePosition();

	pos->X = X;
	pos->Y = Y;
	pos->ZPlane = ZPlane;

	parent->GameThread->Enqueue(_Move, pos);
}
