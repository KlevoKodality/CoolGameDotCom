#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HeroCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class COOLGAMEDOTCOM_API AHeroCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHeroCharacter();

protected:
	virtual void BeginPlay() override;

	// Компоненты изометрической камеры
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> TopDownCamera;

	// Параметры механики рывка (Dash)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Dash")
	float DashDistance; // Сила импульса рывка

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Dash")
	float DashCooldown; // Время перезарядки (сек)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Dash")
	float InvulnerabilityDuration; // Время окна неуязвимости (сек)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Dash")
	bool bCanDash;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Dash")
	bool bIsInvulnerable;

	FTimerHandle CooldownTimerHandle;
	FTimerHandle InvulnerabilityTimerHandle;

	// Сброс флагов по таймерам
	void ResetDash();
	void EndInvulnerability();

	// Плавный разворот к курсору
	void RotateTowardsCursor();

public:
	virtual void Tick(float DeltaTime) override;

	// Функция выполнения рывка (вызывается по нажатию клавиши)
	UFUNCTION(BlueprintCallable, Category = "Combat|Dash")
	void PerformDash();

	// Событие-мостик для спавна визуального следа/партиклов в Blueprint
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat|Dash")
	void OnDashExecuted();

	UFUNCTION(BlueprintPure, Category = "Combat|Dash")
	bool IsInvulnerable() const { return bIsInvulnerable; }
};
