#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

// Технический класс для распознавания интерфейса редактором и Blueprints
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Интерфейс взаимодействия. Начинается с буквы 'I' по стандарту Unreal Engine.
 */
class COOLGAMEDOTCOM_API IInteractableInterface
{
	GENERATED_BODY()

public:
	/**
	 * BlueprintNativeEvent означает:
	 * 1. Логику функции можно написать на C++ (через суффикс _Implementation).
	 * 2. При желании логику можно переопределить или дополнить в Blueprints.
	 * BlueprintCallable позволяет вызывать эту функцию из любого Blueprint-графа.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void ExecuteInteraction(AActor* Interactor);
};
