#include "ArenaManager.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

AArenaManager::AArenaManager()
{
	PrimaryActorTick.bCanEverTick = false;

	EntryTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("EntryTrigger"));
	RootComponent = EntryTrigger;
	EntryTrigger->SetBoxExtent(FVector(150.0f, 150.0f, 100.0f));

	EntryTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	EntryTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	EntryTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	CurrentWave = 0;
	TotalWaves = 2; // По умолчанию 2 волны
	AliveEnemiesCount = 0;
	bArenaStarted = false;

	// Добавим 2 точки спавна по умолчанию
	SpawnPoints.Add(FVector(300.0f, 300.0f, 50.0f));
	SpawnPoints.Add(FVector(-300.0f, 300.0f, 50.0f));
}

void AArenaManager::BeginPlay()
{
	Super::BeginPlay();
	EntryTrigger->OnComponentBeginOverlap.AddDynamic(this, &AArenaManager::OnEntryTriggerOverlap);
}

void AArenaManager::OnEntryTriggerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	// Реагируем только на игрока (наследник ACharacter) и только один раз
	if (bArenaStarted || !OtherActor->IsA(ACharacter::StaticClass())) return;

	bArenaStarted = true;
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Игрок вошел! Арена закрыта!"));

	CloseDoors();      // Блокируем двери
	SpawnNextWave();   // Запускаем 1-ю волну
}

void AArenaManager::SpawnNextWave()
{
	CurrentWave++;
	AliveEnemiesCount = 0;

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow,
		FString::Printf(TEXT("Волна %d из %d началась!"), CurrentWave, TotalWaves));

	for (const FVector& Offset : SpawnPoints)
	{
		FVector SpawnLocation = GetActorLocation() + Offset;
		FRotator SpawnRotation = FRotator::ZeroRotator;

		if (EnemyClassToSpawn)
		{
			APawn* NewEnemy = GetWorld()->SpawnActor<APawn>(EnemyClassToSpawn, SpawnLocation, SpawnRotation);
			if (NewEnemy)
			{
				AliveEnemiesCount++;
				// Подписываемся на удаление/смерть моба
				NewEnemy->OnDestroyed.AddDynamic(this, &AArenaManager::OnSpawnedEnemyDestroyed);
			}
		}
		else
		{
			// Если класс врага еще не выбран, просто выводим текст в консоль
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan,
				FString::Printf(TEXT("Спавн точки в %s (Класс моба не выбран)"), *SpawnLocation.ToString()));
		}
	}
}

void AArenaManager::OnSpawnedEnemyDestroyed(AActor* DestroyedActor)
{
	AliveEnemiesCount--;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White,
		FString::Printf(TEXT("Враг убит! Осталось врагов: %d"), AliveEnemiesCount));

	CheckWaveProgress();
}

void AArenaManager::CheckWaveProgress()
{
	if (AliveEnemiesCount <= 0)
	{
		if (CurrentWave < TotalWaves)
		{
			// Есть следующая волна — запускаем через 2 секунды паузы
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Волна зачищена! Передышка..."));
			FTimerHandle WavePauseHandle;
			GetWorldTimerManager().SetTimer(WavePauseHandle, this, &AArenaManager::SpawnNextWave, 2.0f, false);
		}
		else
		{
			// Все волны побеждены!
			GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Green, TEXT("АРЕНА ПРОЙДЕНА! Двери открыты!"));
			OpenDoors();
		}
	}
}
