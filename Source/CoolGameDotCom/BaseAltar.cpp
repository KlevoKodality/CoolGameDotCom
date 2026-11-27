#include "BaseAltar.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Internationalization/Internationalization.h"

ABaseAltar::ABaseAltar()
{
	PrimaryActorTick.bCanEverTick = false;

	// Создаем модель алтаря и делаем ее корневым компонентом
	AltarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AltarMesh"));
	RootComponent = AltarMesh;

	// Создаем сферу взаимодействия и крепим ее к модели
	InteractionRadius = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionRadius"));
	InteractionRadius->SetupAttachment(RootComponent);
	InteractionRadius->SetSphereRadius(200.0f);

	// Настраиваем коллизию сферы: включаем генерацию событий и нужные каналы
	InteractionRadius->SetGenerateOverlapEvents(true);
	InteractionRadius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionRadius->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionRadius->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionRadius->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);

	bIsAlreadyUsed = false;
}

void ABaseAltar::BeginPlay()
{
	Super::BeginPlay();

	// Дублируем настройки коллизии, чтобы обойти старый кеш в Blueprint алтаря
	InteractionRadius->SetGenerateOverlapEvents(true);
	InteractionRadius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionRadius->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionRadius->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionRadius->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);

	// Привязываем функции C++ к событиям входа и выхода сферы
	InteractionRadius->OnComponentBeginOverlap.AddDynamic(this, &ABaseAltar::OnOverlapBegin);
	InteractionRadius->OnComponentEndOverlap.AddDynamic(this, &ABaseAltar::OnOverlapEnd);
}

void ABaseAltar::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	// Если алтарь уже активирован или коснулся сам себя — игнорируем
	if (bIsAlreadyUsed || !OtherActor || OtherActor == this) return;

	// Если в зону попал физический объект (куб) или любой другой не-игрок — активируем алтарь
	if (OtherComp && OtherComp->GetCollisionObjectType() == ECC_PhysicsBody)
	{
		ExecuteInteraction_Implementation(OtherActor);
		return;
	}

	// Если вошел игрок (Pawn) — показываем подсказку «[E] Взаимодействовать»
	if (OtherActor->IsA(APawn::StaticClass()))
	{
		TogglePrompt(true);
	}
}

void ABaseAltar::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == this) return;

	// Прячем подсказку только если из зоны вышел именно игрок
	if (OtherActor->IsA(APawn::StaticClass()))
	{
		TogglePrompt(false);
	}
}

void ABaseAltar::ExecuteInteraction_Implementation(AActor* Interactor)
{
	// Если алтарь уже использован, второй раз активировать нельзя
	if (bIsAlreadyUsed)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("The altar is already empty!"));
		}
		return;
	}

	bIsAlreadyUsed = true;
	TogglePrompt(false);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("The altar is activated! Opening the gifts..."));
	}

	// Вызываем Blueprint-событие, которое выведет меню с карточками
	OpenBoonMenu(Interactor);
}
