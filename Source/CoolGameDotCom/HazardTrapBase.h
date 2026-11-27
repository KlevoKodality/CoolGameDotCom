#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HazardTrapBase.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class COOLGAMEDOTCOM_API AHazardTrapBase : public AActor
{
	GENERATED_BODY()

public:
	AHazardTrapBase();

protected:
	virtual void BeginPlay() override;

	// Корневой меш основания плиты
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BaseMesh;

	// Зона поражения ловушки
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> DamageArea;

	// Величина урона за удар
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap Settings")
	float Damage;

	// Время предупреждения перед ударом (в секундах)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap Settings")
	float ActivationDelay;

	// Флаг перезарядки ловушки
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap State")
	bool bIsActive;

	FTimerHandle TrapTimerHandle;

	// Обработка наступания на ловушку
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

public:
	// Функция, которая наносит фактический урон
	UFUNCTION(BlueprintCallable, Category = "Trap Logic")
	void ActivateTrap();

	// События-мостики для запуска анимаций и эффектов в Blueprints
	UFUNCTION(BlueprintImplementableEvent, Category = "Trap Events")
	void PlayWarningEffect();

	UFUNCTION(BlueprintImplementableEvent, Category = "Trap Events")
	void PlayAttackEffect();
};