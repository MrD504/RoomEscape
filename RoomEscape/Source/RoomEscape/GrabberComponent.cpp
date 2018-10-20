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
		UE_LOG(LogTemp, Warning, TEXT("User Input %s loaded"), *(UserInput->GetName()));

		///Bind the input axis
		UserInput->BindAction("Grab", IE_Pressed, this, &UGrabberComponent::Grab);
		UserInput->BindAction("Grab", IE_Released, this, &UGrabberComponent::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("User input component not found for %s"), *(GetOwner()->GetName()));
	}
}

void UGrabberComponent::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab pressed"));

	/// LINE TRACE and see if we reach any actors with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();
	/// If we hit something then attach a physics hanle
	// attach physics handle
	if (ActorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor hit"));

		PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab, 
			NAME_None, 
			ComponentToGrab->GetOwner()->GetActorLocation()
		);
	}
}

void UGrabberComponent::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab released"));
	// TODO release physics handle
	PhysicsHandle->ReleaseComponent();
}

// Called every frame
void UGrabberComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotator;
	/// Get player view point this tick
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotator
	);

	// if the physics handle is attached
		// move the object that we're holding
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotator.Vector() * Reach;

	if (PhysicsHandle->GetGrabbedComponent())
	{
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}
	
}

FHitResult UGrabberComponent::GetFirstPhysicsBodyInReach() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotator;
	/// Get player view point this tick
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotator
	);

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotator.Vector() * Reach;

	/// Set up query 
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	/// Line-trace (aka Ray-cast) out to reach distance
	FHitResult Hit;

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	/// See what we hit
	AActor* ActorHit = Hit.GetActor();
	if (ActorHit) {
		UE_LOG(LogTemp, Warning, TEXT("I hit %s"), *(ActorHit->GetName()));
	}

	return Hit;
}