#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h" 

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class COOLGAMEDOTCOM_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	// События (диспетчеры)
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Health|Events")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Health|Events")
	FOnDeath OnDeath;

	// Переменные здоровья
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health|Variables")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health|Variables")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health|Variables")
	float DamageTaken;

	bool AreYouDead = false;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};