// Fill out your copyright notice in the Description page of Project Settings.

#include "Engineers.h"
#include "TargetingComponent.h"
#include "TargetableInterface.h"
//#include "FractalGrowthComponent.h"


template<class Intrfc>
TScriptInterface<Intrfc> GetScriptInterface(AActor* actor)
{
	const UClass* interfc = Intrfc::UClassType::StaticClass();

	for (const UClass* cls = actor->GetClass(); cls; cls = cls->GetSuperClass())
	{
		// SomeInterface might be a base interface of our implemented interface
		for (TArray<FImplementedInterface>::TConstIterator It(cls->Interfaces); It; ++It)
		{
			if (It->bImplementedByK2 && It->Class->IsChildOf(interfc))
			{
				Intrfc* inty = reinterpret_cast<Intrfc*>(actor);
				if (inty != nullptr)
				{
					TScriptInterface<Intrfc> intrfc;
					intrfc.SetObject(actor);
					intrfc.SetInterface(inty);
					return intrfc;
				}
			}
		}
	}
	return TScriptInterface<Intrfc>();
}

template<class Intrfc>
TScriptInterface<Intrfc> GetNativeInterface(AActor* actor)
{
	const UClass* intrfc = Intrfc::UClassType::StaticClass();

	for (UClass* CurrentClass = actor->GetClass(); CurrentClass != NULL; CurrentClass = CurrentClass->GetSuperClass())
	{
		for (TArray<FImplementedInterface>::TIterator It(CurrentClass->Interfaces); It; ++It)
		{
			// See if this is the implementation we are looking for, and it was done natively, not in K2
			FImplementedInterface& ImplInterface = *It;
			if (!ImplInterface.bImplementedByK2 && ImplInterface.Class->IsChildOf(intrfc))
			{
				Intrfc* inty = reinterpret_cast<Intrfc*>((uint8*)actor + It->PointerOffset);
				if (inty != nullptr)
				{
					TScriptInterface<Intrfc> ret;
					ret.SetObject(actor);
					ret.SetInterface(inty);
					return ret;
				}
			}
		}
	}
	return TScriptInterface<Intrfc>();
}

//***********************************************************************************************************************
UTargetingComponent::UTargetingComponent(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

//***********************************************************************************************************************
void UTargetingComponent::OnRegister()
{
	Super::OnRegister();
	Camera = Cast<UCameraComponent>(GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()));

	SetComponentTickEnabled(true);

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	SetActive(true);
}

//***********************************************************************************************************************
void UTargetingComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (Camera != nullptr)
	{
		FVector start = Camera->GetComponentLocation(); // Transform().GetLocation();
		FVector end = start + Camera->GetComponentRotation().RotateVector(FVector::ForwardVector)*3000.0f;

		FCollisionQueryParams params(FName(TEXT("PlayerTargeting")), false);
		params.bReturnPhysicalMaterial = false;
		
		// We do nothing if not class provided, rather than giving ALL actors!

		bool hit = false;

//		for (TActorIterator<AActor> It(World); It; ++It)
//		{
//			AActor* act = *It;
//			if (!act->IsPendingKill())
//			{
//				auto comp = act->GetComponentByClass(UFractalGrowthComponent::StaticClass());
//				if (comp != nullptr)
//				{
//					UFractalGrowthComponent* fracy = Cast<UFractalGrowthComponent>(comp);
//					if (fracy)
//					{
//						FTransform tr = fracy->GetComponentTransform();
//
//						FVector startMs = tr.InverseTransformPosition(start);
//						FVector endMs = tr.InverseTransformPosition(end);
//
//						uint32 boneid = fracy->Raycast(startMs, endMs);
//						if (boneid!=0)
//						{
//							auto comp = act->GetComponentByClass(UFractalGrowthComponent::StaticClass());
//							Hit.Actor = act;
////							Hit.FaceIndex = boneid;
//							Hit.Item = boneid;
//							hit = true;
//						}
//					}
//				}
//			}
//		}
		
		// todo: set up a trace that will pick up just targets

		if (!hit)
		{
			FHitResult res;
			if (GetWorld()->LineTraceSingleByChannel(res, start, end, ECC_PhysicsBody, params))
			{
				if (res.Actor.IsValid())
				{
					AActor* target = nullptr;

					TScriptInterface<ITargetableInterface> scriptInterface = GetScriptInterface<ITargetableInterface>(res.Actor.Get());
					if (scriptInterface == nullptr)
					{
						scriptInterface = GetNativeInterface<ITargetableInterface>(res.Actor.Get());
					}
					if (scriptInterface != nullptr && scriptInterface->Execute_IsTargetable(res.Actor.Get()))
					{
						target = res.Actor.Get();
					}

					res.Actor = target;
				}

				bool takeHit = true;

				// test against the render mesh so we can fish out the piece idx without tumbling down the UE4 destructible rabbit hole.

				//TODO: 4.14
				//if (res.Actor != nullptr)
				//{
				//	UFractalGrowthComponent* frac = Cast<UFractalGrowthComponent>(res.Actor->GetComponentByClass(UFractalGrowthComponent::StaticClass()));
				//	if (frac != nullptr)
				//	{
				//		FTransform tr = frac->GetComponentTransform();

				//		FVector startMs = tr.InverseTransformPosition(start);
				//		FVector endMs = tr.InverseTransformPosition(end);

				//		uint32 boneid = frac->Raycast(startMs, endMs);
				//		if (boneid != 0)
				//			res.Item = boneid;
				//		else
				//			takeHit = false;
				//	}
				//}

				Hit = takeHit ? res : FHitResult();
			}
			else
				Hit = FHitResult();
		}
	}
}

//***********************************************************************************************************************
bool UTargetingComponent::GetTargetPoint(FHitResult& theHit) const
{
	theHit = Hit;
	return Hit.bBlockingHit;
}

