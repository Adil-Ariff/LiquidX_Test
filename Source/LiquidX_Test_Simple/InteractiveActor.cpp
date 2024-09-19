// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveActor.h"

// Sets default values
AInteractiveActor::AInteractiveActor()
{
    PrimaryActorTick.bCanEverTick = true;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;
}

void AInteractiveActor::BeginPlay()
{
    Super::BeginPlay();
}

void AInteractiveActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}


