// Fill out your copyright notice in the Description page of Project Settings.


#include "ABaseMeleeWeapon.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

// Sets default values
AABaseMeleeWeapon::AABaseMeleeWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WeaponRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
	RootComponent = WeaponRoot;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HitBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("HitboxCollision"));
	HitBoxCollision->SetupAttachment(WeaponMesh);
	HitBoxCollision->SetBoxExtent(FVector(10.0f, 10.0f, 50.0f));
	HitBoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitBoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	HitBoxCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	WeaponBuff = 1.0f;
	WeaponDamage = 25.0f;
	KnockbackForce = 1000.0f;
}

// Called when the game starts or when spawned
void AABaseMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	HitBoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AABaseMeleeWeapon::OnHitboxBeginOverlap);
}

void AABaseMeleeWeapon::OnHitboxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;
	if (OtherActor == GetOwner() || OtherActor == GetInstigator()) return;
	if (HitActorsThisSwing.Contains(OtherActor)) return;
	HitActorsThisSwing.Add(OtherActor);
	UGameplayStatics::ApplyDamage(OtherActor, WeaponDamage * WeaponBuff, GetInstigatorController(), this, UDamageType::StaticClass());

	ACharacter* TargetCharacter = Cast<ACharacter>(OtherActor);
	if (TargetCharacter) {
		FVector LaunchDirection = OtherActor->GetActorLocation() - GetActorLocation();
		LaunchDirection.Z = 10.0f;
		LaunchDirection.Normalize();
		TargetCharacter->LaunchCharacter(LaunchDirection * KnockbackForce, true, true);
	}
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,
		FString::Printf(TEXT("Udar mechom po: %s na %.0f HP"), *OtherActor->GetName(), WeaponDamage * WeaponBuff));

}

// Called every frame
void AABaseMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AABaseMeleeWeapon::EnableHitbox()
{
	HitActorsThisSwing.Empty();
	HitBoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Hitbox: ON"));
}

void AABaseMeleeWeapon::DisableHitbox()
{
	HitBoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitActorsThisSwing.Empty();

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Hitbox: OFF"));
}

