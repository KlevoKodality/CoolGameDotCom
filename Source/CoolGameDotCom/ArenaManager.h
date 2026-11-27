#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArenaManager.generated.h"

class UBoxComponent;

UCLASS()
class YOURPROJECTNAME_API AArenaManager : public AActor
{
	GENERATED_BODY()

public:
	AArenaManager();

protected:
	virtual void BeginPlay() override;

	// Зона на входе в комнату, запускающая бой
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> EntryTrigger;

	// Точки на полу, где появляются противники (смещения относительно арены)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena Setup", meta = (MakeEditWidget = true))
	TArray<FVector> SpawnPoints;

	// Какой C++ или Blueprint класс врага спавнить
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena Setup")
	TSubclassOf<APawn> EnemyClassToSpawn;

	// Номер текущей волны (начиная с 0)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arena State")
	int32 CurrentWave;

	// Сколько всего волн должно быть
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena Setup")
	int32 TotalWaves;

	// Счетчик живых противников в текущей волне
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arena State")
	int32 AliveEnemiesCount;

	// Защита от повторного запуска арены
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arena State")
	bool bArenaStarted;

	UFUNCTION()
	void OnEntryTriggerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	// Обработчик гибели заспавненного врага
	UFUNCTION()
	void OnSpawnedEnemyDestroyed(AActor* DestroyedActor);

public:
	// Запуск волны (можно вызывать вручную или по кнопке для теста)
	UFUNCTION(BlueprintCallable, Category = "Arena Logic")
	void SpawnNextWave();

	// Проверка: зачищена ли волна
	UFUNCTION(BlueprintCallable, Category = "Arena Logic")
	void CheckWaveProgress();

	// События-мостики для управления дверьми в Blueprint
	UFUNCTION(BlueprintImplementableEvent, Category = "Arena Events")
	void CloseDoors();

	UFUNCTION(BlueprintImplementableEvent, Category = "Arena Events")
	void OpenDoors();
};
