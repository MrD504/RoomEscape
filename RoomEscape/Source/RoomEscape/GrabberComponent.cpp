// Fill out your copyright notice in the Description page of Project Settings.

// DO I NEED ROOMESCAPE HEADER?
#include "GrabberComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/Public/DrawDebugHelpers.h"
#define OUT

// Sets default values for this component's properties
UGrabberComponent::UGrabberComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabberComponent::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
}

///Look for attached physics handle
void UGrabberComponent::FindPhysicsHandleComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle)
	{
		// Physics handle found
		UE_LOG(LogTemp, Warning, TEXT("Physics handle %s loaded"), *(PhysicsHandle->GetName()));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No physics handle found for %s"), *(GetOwner()->GetName()));
	}
}

/// Look for attached Input component (only appears at runtime)
void UGrabberComponent::SetupInputComponent()
{
	UserInput = GetOwner()->FindComponentByClass<UInputComponent>();

	if (UserInput)
	{
		// Physics handle found
		///Bind the input axis
		UserInput->BindAction("Grab", IE_Pressed, this, &UGrabberComponent::Grab);
		UserInput->BindAction("Grab", IE_Released, this, &UGrabberComponent::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("User input component not found for %s"), *(GetOwner()->GetName()));
	}
}

/// When grab input pressed search for any moveable objects in range
void UGrabberComponent::Grab()
{
	/// LINE TRACE and see if we reach any actors with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();
	/// If we hit something then attach a physics hanle
	if (ActorHit)
	{
		// check to see if physics handle is defined. Note: always do this or you will get major crash
		if (!PhysicsHandle) return;
		PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab, 
			NAME_None, //No bones needed
			ComponentToGrab->GetOwner()->GetActorLocation()
		);
	}
}

void UGrabberComponent::Release()
{
	// release physics handle
	// check to see if physics handle is defined. Note: always do this or you will get major crash
	if (!PhysicsHandle) return;
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabberComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!PhysicsHandle) { return; }
	if (PhysicsHandle->GetGrabbedComponent())
	{
		PhysicsHandle->SetTargetLocation(GetLineTracePoints().v2);
	}
	
}

FHitResult UGrabberComponent::GetFirstPhysicsBodyInReach() const
{
	

	/// Set up query 
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	/// Line-trace (aka Ray-cast) out to reach distance
	FHitResult HitResult;

	FTwoVectors TracePoints = GetLineTracePoints();

	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		TracePoints.v1,
		TracePoints.v2,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	/// See what we hit
	AActor* ActorHit = HitResult.GetActor();
	return HitResult;
}

FTwoVectors UGrabberComponent::GetLineTracePoints() const
{
	FVector StartLocation;
	FRotator PlayerViewPointRotation;
	/// Get player view point this tick
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT StartLocation,
		OUT PlayerViewPointRotation
	);

	FVector EndLocation = StartLocation + PlayerViewPointRotation.Vector() * Reach;

	return FTwoVectors(StartLocation, EndLocation);
}