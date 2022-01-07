// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

#define OUT

UGrabber::UGrabber()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	SetupInputComponent();
}

void UGrabber::Grab()
{
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	AActor* ActorHit = HitResult.GetActor();

	if (ActorHit && PhysicsHandle)
	{
		std::tuple<FVector, FVector> LineTraceTuple = GetPlayerViewPointAndLineTraceEnd();

		PhysicsHandle->GrabComponentAtLocation(
			ComponentToGrab,
			NAME_None,
			std::get<1>(LineTraceTuple)
		);
	}
}

void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("key is released"));

	if (!PhysicsHandle) return;
	PhysicsHandle->ReleaseComponent();
}

void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("WTF bro there is no physics handle in %s"), *GetOwner()->GetName());
	}
}

void UGrabber::SetupInputComponent()
{
	Input = GetOwner()->FindComponentByClass<UInputComponent>();

	if (Input)
	{
		Input->BindAction("Grab", EInputEvent::IE_Pressed, this, &UGrabber::Grab);
		Input->BindAction("Grab", EInputEvent::IE_Released, this, &UGrabber::Release);
	}
}

void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	std::tuple<FVector, FVector> LineTraceTuple = GetPlayerViewPointAndLineTraceEnd();

	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(std::get<1>(LineTraceTuple));
	}
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	std::tuple<FVector, FVector> LineTraceTuple = GetPlayerViewPointAndLineTraceEnd();

	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	if (GetWorld()->LineTraceSingleByObjectType(OUT Hit, std::get<0>(LineTraceTuple), std::get<1>(LineTraceTuple), FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), TraceParams))
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor %s is hit"), *Hit.GetActor()->GetName());
	}

	return Hit;
}

std::tuple<FVector, FVector> UGrabber::GetPlayerViewPointAndLineTraceEnd() const
{
	FVector ViewPointVec; FRotator ViewPointRotator;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT ViewPointVec,
		OUT ViewPointRotator
	);

	FVector LineTraceEnd = ViewPointVec + ViewPointRotator.Vector() * Reach;

	return { ViewPointVec, LineTraceEnd };
}