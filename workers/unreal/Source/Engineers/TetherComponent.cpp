// Fill out your copyright notice in the Description page of Project Settings.

#include "Engineers.h"
#include "TetherComponent.h"


void UTetherComponent::OnRegister()
{
	Super::OnRegister();
	Tethered = true;
}

