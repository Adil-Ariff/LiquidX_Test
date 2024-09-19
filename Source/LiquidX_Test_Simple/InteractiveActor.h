// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveActor.generated.h"

UCLASS()
class LIQUIDX_TEST_SIMPLE_API AInteractiveActor : public AActor
{
	GENERATED_BODY()
	
public:
    AInteractiveActor();

    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
    void Interact();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    bool bIsInteractable = true;

};
