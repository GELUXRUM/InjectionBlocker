#include "detourxs/detourxs.h"
#include "Hooks.h"

namespace Hooks
{
	typedef void(mem_LeveledItem_AddFormSig)(std::uint64_t*, RE::BSScript::IVirtualMachine*, __int64, RE::TESForm*, RE::TESForm*, __int16);
	REL::Relocation<mem_LeveledItem_AddFormSig> OriginalFunction_Item;
	DetourXS itemHook;

	void Hookedmem_LeveledItem_AddForm(std::uint64_t* a_unk, RE::BSScript::IVirtualMachine* a_vm, __int64 a_unk1, RE::TESForm* a_item, RE::TESForm* a_unk2, __int16 a_unk3)
	{
		if (iniData.whitelist.find(a_item) != iniData.whitelist.end()) {
			logger::warn("{} is a whitelisted item", a_item->formID);
			OriginalFunction_Item(a_unk, a_vm, a_unk1, a_item, a_unk2, a_unk3);
			return;
		}

		if (iniData.bBlockAllItems) {
			logger::warn("Blocked an item injection: {}", a_item->formID);
			return;
		}

		if (iniData.bBlockAmmo && a_item->formType == RE::ENUM_FORM_ID::kAMMO) {
			logger::warn("Blocked an ammo injection: {}", a_item->formID);
			return;
		}

		if (iniData.bBlockArmor && a_item->formType == RE::ENUM_FORM_ID::kARMO) {
			logger::warn("Blocked an armor injection: {}", a_item->formID);
			return;
		}

		if (iniData.bBlockWeapons && a_item->formType == RE::ENUM_FORM_ID::kWEAP) {
			logger::warn("Blocked a weapon injection: {}", a_item->formID);
			return;
		}

		OriginalFunction_Item(a_unk, a_vm, a_unk1, a_item, a_unk2, a_unk3);
	}

	typedef void(mem_LeveledActor_AddFormSig)(std::uint64_t*, RE::BSScript::IVirtualMachine*, __int64, RE::TESForm*, RE::TESForm*);
	REL::Relocation<mem_LeveledActor_AddFormSig> OriginalFunction_Actor;
	DetourXS actorHook;

	void Hookedmem_LeveledActor_AddForm(std::uint64_t* a_unk, RE::BSScript::IVirtualMachine* a_vm, __int64 a_unk1, RE::TESForm* a_actor, RE::TESForm* a_unk2)
	{
		if (iniData.whitelist.find(a_actor) != iniData.whitelist.end()) {
			logger::warn("{} is a whitelisted actor", a_actor->formID);
			return;
		}

		if (iniData.bBlockNPCs && a_actor->formType == RE::ENUM_FORM_ID::kNPC_) {
			logger::warn("Blocked an NPC injection: {}", a_actor->formID);
			return;
		}

		OriginalFunction_Actor(a_unk, a_vm, a_unk1, a_actor, a_unk2);
	}

	void Install()
	{
		REL::Relocation<mem_LeveledItem_AddFormSig> itemFuncLocation{ REL::ID(903957) };
		if (itemHook.Create(reinterpret_cast<LPVOID>(itemFuncLocation.address()), &Hookedmem_LeveledItem_AddForm)) {
			OriginalFunction_Item = reinterpret_cast<std::uintptr_t>(itemHook.GetTrampoline());
		} else {
			logger::warn("Failed to create Item hook");
		}

		REL::Relocation<mem_LeveledActor_AddFormSig> actorFuncLocation{ REL::ID(1200614) };
		if (actorHook.Create(reinterpret_cast<LPVOID>(actorFuncLocation.address()), &Hookedmem_LeveledActor_AddForm)) {
			OriginalFunction_Actor = reinterpret_cast<std::uintptr_t>(actorHook.GetTrampoline());
		} else {
			logger::warn("Failed to create Actor hook");
		}
	}
}
