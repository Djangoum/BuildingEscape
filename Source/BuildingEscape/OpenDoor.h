// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TriggerVolume.h" 
#include "OpenDoor.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UOpenDoor : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOpenDoor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Open(float DeltaTime);
	void Close(float DeltaTime);

	float TotalMassOfActors() const;
	void FindAudioComponent();
	void FindPressurePlate();

	bool OpenDoorSound = false;
	bool CloseDoorSound = true;

private:
	float OriginalYaw;
	float DoorLastOpened = 0.f;
	float DoorCloseDelay = 0.1f;

	UPROPERTY(EditAnywhere)
	float TargetYaw = 120.f;

	UPROPERTY(EditAnywhere)
	float MassToOpenDoor = 50.f;

	UPROPERTY(EditAnywhere)
	ATriggerVolume* PressurePlate;

	UPROPERTY(EditAnywhere)
	AActor* ActorThatOpen;

	UPROPERTY()
	UAudioComponent* AudioComponent = nullptr;
};
