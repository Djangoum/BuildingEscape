// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"

#define OUT

UOpenDoor::UOpenDoor()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("WTF bro there is no audio component in %s"), *GetOwner()->GetName());
	}
}

void UOpenDoor::FindPressurePlate()
{
	AActor* owner = GetOwner();

	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the open door component on it, but no pressure plate was set"), *owner->GetName());
	}
}

void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	AActor* owner = GetOwner();

	FRotator CurrentRotation = owner->GetActorRotation();

	OriginalYaw = CurrentRotation.Yaw;

	ActorThatOpen = GetWorld()->GetFirstPlayerController()->GetPawn();

	FindAudioComponent();
	FindPressurePlate();
}

void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TotalMassOfActors() > MassToOpenDoor) 
	{
		Open(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else if ((GetWorld()->GetTimeSeconds() - DoorLastOpened) > DoorCloseDelay)
	{
		Close(DeltaTime);
	}
}

void UOpenDoor::Open(float DeltaTime)
{
	AActor* owner = GetOwner();
	FRotator CurrentRotation = owner->GetActorRotation();

	float newYaw = FMath::FInterpTo(GetOwner()->GetActorRotation().Yaw, OriginalYaw + TargetYaw, DeltaTime, 2);

	CurrentRotation.Yaw = newYaw;

	owner->SetActorRotation(CurrentRotation);

	CloseDoorSound = false;
	if (AudioComponent && !OpenDoorSound)
	{
		AudioComponent->Play();
		OpenDoorSound = true;
	}
}

void UOpenDoor::Close(float DeltaTime)
{
	AActor* owner = GetOwner();
	FRotator CurrentRotation = owner->GetActorRotation();

	float newYaw = FMath::FInterpTo(GetOwner()->GetActorRotation().Yaw, OriginalYaw, DeltaTime, 2);

	CurrentRotation.Yaw = newYaw;

	owner->SetActorRotation(CurrentRotation);

	OpenDoorSound = false;
	if (AudioComponent && !CloseDoorSound)
	{
		AudioComponent->Play();
		CloseDoorSound = true;
	}
}

float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;

	TArray<AActor*> OverlappingActors;
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		UPrimitiveComponent* PrimitiveComponent = OverlappingActor->FindComponentByClass<UPrimitiveComponent>();

		TotalMass += PrimitiveComponent->CalculateMass();
	}

	return TotalMass;
}