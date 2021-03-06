// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "GrabberComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner();

	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("OpenDoor.cpp -> BeginPlay -> %s is missing pressure plate"), *GetOwner()->GetName());
	}

}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// poll trigger vol
	if (GetTotalMassOfActorsOnPlate() > TriggerMass) //TODO make in to parameter
	{
		//if actor that opens is in volume
		OnOpen.Broadcast();
	}
	else {
		OnClose.Broadcast();
	}
}

float UOpenDoor::GetTotalMassOfActorsOnPlate()
{
	float totalMass = 0.f;
	TArray<AActor*> OverlappingActors;
	// Find all overlapping actors

	if (!PressurePlate) { return totalMass; }
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);
	// Iterate through add masses

	// Ignore errors, seem to be IDE errors rather than breaking errors
	for (const auto& Actor : OverlappingActors)
	{
		totalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}

	UE_LOG(LogTemp, Warning, TEXT("%f%"), totalMass);
	return totalMass;
}

