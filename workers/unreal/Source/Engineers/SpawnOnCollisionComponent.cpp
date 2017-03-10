// Fill out your copyright notice in the Description page of Project Settings.

#include "Engineers.h"
#include "SpawnOnCollisionComponent.h"


USpawnOnCollisionComponent::USpawnOnCollisionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), Collided(false)
{

}


