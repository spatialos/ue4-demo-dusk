#pragma once

#include "ToolInterface.generated.h"

/** Class needed to support InterfaceCast<IToStringInterface>(Object) */
UINTERFACE(MinimalAPI)
class UToolInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IToolInterface
{
	GENERATED_IINTERFACE_BODY()

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Tool")
	void PrimaryUseBegin(AActor* target, FHitResult hit);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Tool")
	void PrimaryUseEnd(AActor* target, FHitResult hit);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Tool")
	void OnInteractedWith(AActor* target);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Tool")
	void OnZoom(float zoom);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Tool")
	TArray<AActor*> GetTargets();
};