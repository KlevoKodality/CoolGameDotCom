#include "HealthComponent.h" 

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	MaxHealth = 100.0f;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	AreYouDead = false;
	CurrentHealth = MaxHealth;
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeDamage);
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHealthComponent::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) {
	CurrentHealth = CurrentHealth - DamageTaken;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth < 0.0f) {
		CurrentHealth = 0.0f;

	}
	if (CurrentHealth > 100.0f) {
		CurrentHealth = 100.0f;
	}


	if (CurrentHealth == 0.0f and AreYouDead==false) {
		AreYouDead = true;
		OnDeath.Broadcast();
	}
	

}
