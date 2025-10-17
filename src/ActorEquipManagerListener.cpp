#include "ActorEquipManagerListener.h"

class ActorEquipManagerSink : public RE::BSTEventSink<RE::ActorEquipManagerEvent::Event>
{
public:
    static ActorEquipManagerSink* GetSingleton()
    {
        static ActorEquipManagerSink singleton;
        return std::addressof(singleton);
    }

private:
    RE::BSEventNotifyControl ProcessEvent(const RE::ActorEquipManagerEvent::Event& aEvent, RE::BSTEventSource<RE::ActorEquipManagerEvent::Event>* aSource) override
    {
        // REX::INFO("ActorEquipManagerEvent");

        return RE::BSEventNotifyControl::kContinue;
    }
};

void ActorEquipManagerListener::Register()
{
    auto *mgr = RE::ActorEquipManager::GetSingleton();

    if (mgr == NULL)
    {
        REX::WARN("ActorEquipManager is null.");

        return;
    }

    mgr->RegisterSink(ActorEquipManagerSink::GetSingleton());

    REX::INFO("ActorEquipManager event listener registered.");
}