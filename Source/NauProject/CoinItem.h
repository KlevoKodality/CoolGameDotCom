#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoinItem.generated.h"
class USphereComponent;
class UStaticMeshComponent;
class USoundBase;

UCLASS()
class NAUPROJECT_API ACoinItem : public AActor
{
	GENERATED_BODY()

public:
	ACoinItem();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin|Components")
	USphereComponent* SphereCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin|Components")
	UStaticMeshComponent* CoinMesh;
	// Скорость вращения монеты на сцене
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin|Movement")
	float RotationSpeed = 100.0f;
	// Звук подбора
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin|Audio")
	USoundBase* PickupSound;
	// Функция обработки касания
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
public:
	virtual void Tick(float DeltaTime) override;
};
