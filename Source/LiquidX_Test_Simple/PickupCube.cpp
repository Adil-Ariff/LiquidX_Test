// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupCube.h"

// Sets default values
APickupCube::APickupCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;

    MeshComponent->SetSimulatePhysics(false);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

}

// Called when the game starts or when spawned
void APickupCube::BeginPlay()
{
	Super::BeginPlay();
    CurrentHealth = MaxHealth;
}

// Called every frame
void APickupCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float APickupCube::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float DamageApplied = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
    CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageApplied);

    if (CurrentHealth <= 0)
    {
        Destroy();
    }

    return DamageApplied;
}

