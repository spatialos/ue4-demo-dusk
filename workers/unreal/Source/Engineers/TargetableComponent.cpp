// Fill out your copyright notice in the Description page of Project Settings.

#include "Engineers.h"
#include "TargetableComponent.h"


UTargetableComponent::UTargetableComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

bool UTargetableComponent::IsTargetable()
{
//	GetOwner()->Implements<UIsTarge>
	return false;
}


