// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "GrabberComponent.h"
#include "Classes/Components/PrimitiveComponent.h"
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

}

void UOpenDoor::OpenDoor()
{
	//Set the door rotation
	Owner->SetActorRotation(FRotator(0.f, OpenAngle, 0.f));
}

void UOpenDoor::CloseDoor()
{
	//Set the door rotation
	Owner->SetActorRotation(FRotator(0.f, CloseAngle, 0.f));
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// poll trigger vol
	if (GetTotalMassOfActorsOnPlate() > 40.f) //TODO make in to parameter
	{
		//if actor that opens is in volume
		OpenDoor();
		LastDoorOpenTime = GetWorld()->GetTimeSeconds();
	}

	// check if its time to close the door
	if (GetWorld()->GetTimeSeconds() >= LastDoorOpenTime + DoorCloseDelay) {
		CloseDoor();
	}
}

float UOpenDoor::GetTotalMassOfActorsOnPlate()
{
	float totalMass = 0.f;
	TArray<AActor*> OverlappingActors;
	// Find all overlapping actors
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);
	// Iterate through add masses

	// Ignore errors, seem to be IDE errors rather than breaking errors
	for (const auto* Actor : OverlappingActors)
	{
		totalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}

	UE_LOG(LogTemp, Warning, TEXT("%f%"), totalMass);
	return totalMass;
}

