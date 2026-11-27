#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h" // Подключаем наш интерфейс!
#include "BaseAltar.generated.h"

// Опережающие объявления (Forward Declarations) экономят время компиляции
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class COOLGAMEDOTCOM_API ABaseAltar : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	ABaseAltar();

protected:
	virtual void BeginPlay() override;

	// 1. Модель постамента
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> AltarMesh;

	// 2. Сфера триггера (зона, куда входит игрок)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> InteractionRadius;

	// 3. Флаг: использован ли уже этот алтарь
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Altar State")
	bool bIsAlreadyUsed;

	// 4. События входа и выхода из зоны триггера
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	/**
	 * Реализация метода интерфейса из IInteractableInterface.
	 * Обратите внимание на обязательный суффикс _Implementation!
	 */
	virtual void ExecuteInteraction_Implementation(AActor* Interactor) override;

	/**
	 * BlueprintImplementableEvent — функция-мостик.
	 * Мы вызываем ее из C++, а всю визуальную верстку виджета сделаем в Blueprint без мучений с кодом!
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Altar UI")
	void OpenBoonMenu(AActor* Interactor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Altar UI")
	void TogglePrompt(bool bShow);
};
