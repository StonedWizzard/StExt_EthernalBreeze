#include <StonedExtension.h>

namespace Gothic_II_Addon
{
    void SetThrowingAi(oCAIArrow* throwableAi, zCVob* throwableItem, zCVob* attacker, zCVob* target)
    {
        //DEBUG_MSG("SetThrowingAi - set ai");
        throwableAi->ClearIgnoreCDVob();
        throwableAi->AddIgnoreCDVob(attacker);

        throwableAi->vob = throwableItem;
        if (throwableItem) throwableItem->AddRef();

        throwableAi->arrow = dynamic_cast<oCItem*>(throwableItem);
        if (throwableAi->arrow) throwableAi->arrow->AddRef();

        throwableAi->owner = dynamic_cast<oCNpc*>(attacker);
        if (throwableAi->owner) throwableAi->owner->AddRef();

        zMAT4 trafo = attacker->GetTrafoModelNodeToWorld("ZS_RIGHTHAND");
        trafo.PostRotateY(-90);

        throwableItem->SetSleeping(FALSE);
        throwableItem->SetPhysicsEnabled(TRUE);
        throwableItem->SetTrafoObjToWorld(trafo);
        throwableItem->GetRigidBody()->gravityOn = FALSE;        
        throwableItem->SetCollDet(TRUE);

        zVEC3 at = (target->GetPositionWorld()) - (throwableItem->GetPositionWorld());
        at.Normalize();
        throwableItem->SetHeadingAtWorld(at);
        zMAT4 mat = throwableItem->trafoObjToWorld;
        mat.PostRotateY(90.0);
        throwableItem->SetTrafoObjToWorld(mat);
        throwableItem->GetRigidBody()->SetVelocity(at * ThrowableVelocity);

        throwableItem->SetCollisionClass(zCCollObjectProjectile::s_oCollObjClass);
        throwableAi->arrow->SetFlag(ITM_FLAG_NFOCUS);
        //throwableAi->CreateTrail(throwableAi->arrow);
        //DEBUG_MSG("SetThrowingAi - set ai done");
    }

    void DoThrow(oCItem* item, oCNpc* attacker, oCNpc* target)
    {
        //DEBUG_MSG("DoThrow - do throw");
        item->SetSleeping(TRUE);
        if (!(item->GetHomeWorld() != 0)) attacker->GetHomeWorld()->AddVob(item);

        oCAIArrow* aiArrow = zNEW(oCAIArrow);
        item->SetAI(aiArrow);
        item->SetVobName("StExt_Throwable_" + attacker->name[0]);
        SetThrowingAi(aiArrow, item, attacker, target);
        item->SetSleeping(FALSE);
        zRELEASE(aiArrow);
        //DEBUG_MSG("DoThrow - do throw end");
    }

    void ThrowItem(ThrowItemDescriptor& itemDescriptor)
    {
        //DEBUG_MSG("StExt_ThrowItem - start throwing");
        oCItem* item = dynamic_cast<oCItem*>(ogame->GetGameWorld()->CreateVob(zTVobType::zVOB_TYPE_ITEM, itemDescriptor.throwInstance));
        if (!item)
        {
            DEBUG_MSG("ThrowItem - item is null!");
            return;
        }
        zVEC3 vec = itemDescriptor.attacker->GetPositionWorld() + itemDescriptor.attacker->trafoObjToWorld.GetAtVector() * 100;

        item->nutrition = 1; // (was 1) true mean that damage calculate from item, not stats
        item->SetPositionWorld(vec);
        item->damageTotal = itemDescriptor.damage;
        item->damageTypes = itemDescriptor.damageType;
        //item->hitp = 999;

        //DEBUG_MSG("StExt_ThrowItem - end throwing");
        DoThrow(item, itemDescriptor.attacker, itemDescriptor.target);
    }
}