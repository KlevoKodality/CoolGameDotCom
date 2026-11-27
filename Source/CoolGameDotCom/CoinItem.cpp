#include "CoinItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ACoinItem::ACoinItem()
{
	PrimaryActorTick.bCanEverTick = true;

	// 1. Коллизия триггера
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetSphereRadius(50.0f);
	RootComponent = SphereCollision;

	// Настраиваем коллизию триггера: реагировать на персонажей и физические объекты (кубик)
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereCollision->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);

	// 2. Модель монетки
	CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinMesh"));
	CoinMesh->SetupAttachment(RootComponent);
	CoinMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACoinItem::BeginPlay()
{
	Super::BeginPlay();

	// Подписываемся на событие пересечения коллизий
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ACoinItem::OnOverlapBegin);
}

void ACoinItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Вращаем монетку для красивого визуала
	AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
}

void ACoinItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	// Игнорируем пустой указатель и саму монетку
	if (!OtherActor || OtherActor == this) return;

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}

	// Удаляем монетку с уровня
	Destroy();
}
