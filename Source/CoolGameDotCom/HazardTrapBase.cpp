#include "HazardTrapBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

AHazardTrapBase::AHazardTrapBase()
{
	PrimaryActorTick.bCanEverTick = false;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	DamageArea = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageArea"));
	DamageArea->SetupAttachment(RootComponent);
	DamageArea->SetBoxExtent(FVector(100.0f, 100.0f, 50.0f));

	// Настройка коллизии: реагируем на пересечение с пешками (игрок и мобы)
	DamageArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	DamageArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	Damage = 15.0f;
	ActivationDelay = 1.0f;
	bIsActive = false;
}

void AHazardTrapBase::BeginPlay()
{
	Super::BeginPlay();
	DamageArea->OnComponentBeginOverlap.AddDynamic(this, &AHazardTrapBase::OnOverlapBegin);
}

void AHazardTrapBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	// Если ловушка уже в процессе удара или сработала на саму себя — выходим
	if (bIsActive || OtherActor == this) return;

	bIsActive = true;

	// Запускаем визуальный сигнал предупреждения в Blueprint (мигание плиты)
	PlayWarningEffect();

	// Заводим таймер до удара шипов
	GetWorldTimerManager().SetTimer(TrapTimerHandle, this, &AHazardTrapBase::ActivateTrap, ActivationDelay, false);
}

void AHazardTrapBase::ActivateTrap()
{
	// Запускаем анимацию вылета шипов в Blueprint
	PlayAttackEffect();

	// Получаем всех, кто сейчас стоит на ловушке
	TArray<AActor*> OverlappingActors;
	DamageArea->GetOverlappingActors(OverlappingActors);

	for (AActor* Victim : OverlappingActors)
	{
		if (Victim && Victim != this)
		{
			// Наносим универсальный урон движка (работает и на игрока, и на врагов)
			UGameplayStatics::ApplyDamage(Victim, Damage, nullptr, this, UDamageType::StaticClass());

			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange,
				FString::Printf(TEXT("Ловушка ударила: %s на %.0f HP"), *Victim->GetName(), Damage));
		}
	}

	// Сбрасываем активность через 1 секунду после удара, чтобы ловушка могла сработать снова
	FTimerHandle ResetHandle;
	GetWorldTimerManager().SetTimer(ResetHandle, [this]() { bIsActive = false; }, 1.0f, false);
}