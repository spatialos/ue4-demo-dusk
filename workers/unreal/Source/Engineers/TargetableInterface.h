#pragma once

#include "TargetableInterface.generated.h"

/** Class needed to support InterfaceCast<IToStringInterface>(Object) */
UINTERFACE(MinimalAPI)
class UTargetableInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class ITargetableInterface
{
	GENERATED_IINTERFACE_BODY()

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Targeting")
	bool IsTargetable();
};