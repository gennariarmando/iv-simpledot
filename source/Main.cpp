#include "plugin.h"
#include "common.h"
#include "Rage.h"
#include "T_CB_Generic.h"
#include "CTxdStore.h"
#include "CHud.h"
#include "CHudColours.h"
#include "CCutsceneMgr.h"
#include "Utility.h"
#include "CTaskSimpleAimGun.h"

using namespace plugin;

class SimpleDotIV {
public:
    static inline CSprite2d dotSprite = {};
    static inline void DrawDot() {
        bool isDotShowing = CHud::Components[aHudComponentInfo[HUD_WEAPON_DOT].m_nIndex]->IsDisplaying();
        if (!isDotShowing)
            return;

        float x = (SCREEN_WIDTH / 2);
        float y = (SCREEN_HEIGHT / 2);
        float w = ScaleX(2.0f);
        float h = ScaleY(2.0f);

        auto playa = FindPlayerPed(0);
        if (playa) {
            CTaskSimpleAimGun* taskSimpleAimGun = playa->m_pPedIntelligence->m_TaskMgr.FindActiveTaskByType<CTaskSimpleAimGun>(TASK_AIM_GUN);
            if (taskSimpleAimGun) {
                if (dotSprite.m_pTexture) {
                    CPed* ped = dynamic_cast<CPed*>(taskSimpleAimGun->GetAt(0, 0));
                    rage::Color32 col = { 255, 255, 255, 255 };
                    if (ped && ped->m_bNotInVehicle)
                        col = CHudColours::Get(HUD_COLOUR_REDDARK, 255);

                    dotSprite.Push();
                    CSprite2d::Draw(rage::Vector4(x - w, y - h, x + w, y + h), col);
                    CSprite2d::Pop();
                }
            }
        }
    }

    SimpleDotIV() {
        plugin::Events::initEngineEvent += []() {
            int32_t slot = CTxdStore::AddTxdSlot("simpledot");
            CTxdStore::LoadTxd(slot, "platform:/textures/simpledot");
            CTxdStore::AddRef(slot);
            CTxdStore::PushCurrentTxd();
            CTxdStore::SetCurrentTxd(slot);
        
            dotSprite.SetTexture("dot");
            CTxdStore::PopCurrentTxd();
        };
        
        plugin::Events::shutdownEngineEvent += []() {
            dotSprite.Delete();

            int32_t slot = CTxdStore::FindTxdSlot("simpledot");
            CTxdStore::RemoveTxdSlot(slot);
        };

        plugin::Events::drawHudEvent.before += []() {
            if (CCutsceneMgr::IsRunning())
                return;

            auto base = new T_CB_Generic_NoArgs(DrawDot);
            base->Append();
        };
    }
} simpleDotIV;
